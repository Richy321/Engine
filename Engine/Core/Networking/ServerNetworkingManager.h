#pragma once
#include "IConnectionEventHandler.h"
#include <chrono>
#include "../TickTimer.h"
#include <mutex>
#include "MessageStructures.h"
#include <map>
#include "NetworkPlayer.h"
#include "../Utils.h"
#include "IConnection.h"
#include "FlowControl.h"
#include "SharedNetworkConfiguration.h"
#include "NetworkServices.h"
#include "../Components/Interfaces/IComponent.h"
#include "../Components/Interfaces/INetworkViewComponent.h"
#include "IClientNetworkManager.h"

namespace networking
{
	class ServerNetworkingManager : public IClientNetworkManager, public IConnectionEventHandler
	{
	private:
		ServerNetworkingManager() : timer(std::make_unique<Core::TickTimer>())
		{
			SetTickRate(std::chrono::milliseconds(5));
			timer->AddOnTickCallback(std::bind(&ServerNetworkingManager::UpdateComms, this));
		}

		typedef std::map<GUID, std::shared_ptr<INetworkViewComponent>, Core::Utils::GUIDComparer> PlayerConnectionMap;
		PlayerConnectionMap connectedPlayerMap;

		IConnection* connection;
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> lastTime;
		FlowControl flowControl;
		std::unique_ptr<TickTimer> timer;

		std::mutex mutexConnectedPlayerMap;

		std::function<void(std::shared_ptr<MessageStructures::BaseMessage>)> onNetworkViewConnectCallback;
		std::function<void(GUID, MessageStructures::MessageType)> onNetworkViewDisconnectCallback;

		GUID lastConnectedPlayer;
	public:

		~ServerNetworkingManager()
		{
			
		}

		static ServerNetworkingManager& GetInstance()
		{
			static ServerNetworkingManager instance;
			return instance;
		}


		void SetTickRate(std::chrono::milliseconds interval)
		{
			timer->SetTickIntervalMilliseconds(interval);
		}

		void InitialiseServerComms()
		{
			startTime = std::chrono::system_clock::now();
			lastTime = startTime;

			switch (serverConnectionType)
			{
			case Unreliable:
				connection = networking::NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);
				break;
			case Reliable:
				connection = networking::NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
				break;
			case MultiUnreliable:
				connection = networking::NetworkServices::GetInstance().CreateMultiConnection(ProtocolId, TimeOut);
				break;
			}

			connection->SetConnectionEventHandler(shared_from_this());

			if (!connection->Start(ServerPort))
			{
				printf("could not start connection on port %d\n", ServerPort);
				return;
			}

			connection->Listen();
			timer->Start();
		}

		void UpdateComms()
		{
			std::chrono::time_point<std::chrono::system_clock> nowTime = std::chrono::system_clock::now();
			std::chrono::milliseconds deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastTime);
			float deltaTimeSecs = deltaTime.count() * 0.001f;
			std::chrono::milliseconds fromStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime);

			ReceiveAndStorePackets();

			//RunSimulation();

			SendUpdatedSnapshots();

			connection->Update(deltaTimeSecs);

			if (serverConnectionType == Reliable)
				flowControl.Update(deltaTimeSecs, static_cast<networking::ReliableConnection*>(connection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

			lastTime = nowTime;
		}

		/*
		void RunSimulation()
		{
			{
				std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedPlayerMap);
				//update server visualisations
				for (auto const& value : connectedPlayerMap)
				{
					if (value.second->messages.size() > 0)
					{
						//just use last message received for the moment
						std::shared_ptr<networking::MessageStructures::BaseMessage> message = value.second->messages[value.second->messages.size() - 1];

						std::shared_ptr<Core::IComponent> component = value.second->relatedGameObject->GetComponentByType(Core::IComponent::NetworkView);
						if (component != nullptr)
						{
							std::shared_ptr<Core::INetworkViewComponent> netView = std::dynamic_pointer_cast<Core::INetworkViewComponent>(component);
							//todo - convert to std::shared_ptr<>
							netView->ReadPacket(*message.get());
						}
					}
				}
			}
			
			//todo - collision detection confirmation...

			//todo - possible physics simulation
		}*/

		void ReceiveAndStorePackets()
		{
			//receive until theres no more packets left
			while (true)
			{
				unsigned char packet[256];
				int bytesRead = connection->ReceivePacket(packet, sizeof(packet));
				if (bytesRead == 0)
					break;
				std::shared_ptr<networking::MessageStructures::BaseMessage> message = std::make_shared<MessageStructures::BaseMessage>();
				memcpy(message.get(), packet, sizeof(networking::MessageStructures::BaseMessage));

				ReadPacket(message);
			}
		}

		void SendUpdatedSnapshots()
		{
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedPlayerMap);
			for (auto const& value : connectedPlayerMap)
			{
				if (value.second->IsSendUpdates())
				{
					std::shared_ptr<MessageStructures::BaseMessage> message = std::make_shared<MessageStructures::BaseMessage>();
					value.second->BuildPacket(message);
					message->simpleType = MessageStructures::SimpleMessageType::SnapShot;
					message->messageType = networking::MessageStructures::PlayerSnapshot;
					message->uniqueID = value.first;

					connection->SendPacket(reinterpret_cast<unsigned char*>(&message), sizeof(MessageStructures::BaseMessage));
				}
			}
		}

		void ReadPacket(std::shared_ptr<networking::MessageStructures::BaseMessage> message)
		{
			//Add new player if not exists
			mutexConnectedPlayerMap.lock();
			if (message->messageType == networking::MessageStructures::PlayerConnect ||
				(message->messageType == networking::MessageStructures::PlayerSnapshot && connectedPlayerMap.find(message->uniqueID) == connectedPlayerMap.end()))
			{
				if (onNetworkViewConnectCallback != nullptr)
					onNetworkViewConnectCallback(message);
			}
			mutexConnectedPlayerMap.unlock();

			if (message->messageType == networking::MessageStructures::PlayerDisconnect)
			{
				if (onNetworkViewDisconnectCallback != nullptr)
					onNetworkViewDisconnectCallback(message->uniqueID, message->messageType);
			}
			else
			{
				mutexConnectedPlayerMap.lock();
				if(connectedPlayerMap.find(message->uniqueID) != connectedPlayerMap.end())
					connectedPlayerMap[message->uniqueID]->ReadPacket(message);
				mutexConnectedPlayerMap.unlock();

			}
		}

		void AddNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			//networkIDToComponent[component->GetUniqueID()] = component;
		}

		void RemoveNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			/*NetworkIDMapType::iterator it = networkIDToComponent.find(component->GetUniqueID());
			if (it != networkIDToComponent.end())
				networkIDToComponent.erase(it);*/
		}

		void OnStart()
		{
			//start listening
		}

		virtual void OnStop()
		{
			//stop listening
		}

		virtual void OnConnect()
		{
			//client connected to server
		}
		virtual void OnDisconnect()
		{
			//client timeout from server
		}
	};
}
