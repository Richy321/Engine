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
	class ServerNetworkingManager : public INetworkManager, public IConnectionEventHandler
	{
	private:
		typedef std::map<GUID, std::shared_ptr<INetworkViewComponent>, Utils::GUIDComparer> NetworkIDMapType;
		NetworkIDMapType networkIDToComponent;

		typedef std::map<std::shared_ptr<Address>, std::vector<std::weak_ptr<INetworkViewComponent>>,Utils::SharedPtrAddressComparer> AddressToNetworkIDVectorMapType;
		AddressToNetworkIDVectorMapType addressToNetworkIDs;

		typedef std::map<std::shared_ptr<Address>, float, Utils::SharedPtrAddressComparer> AddressToFloatMap;
		AddressToFloatMap disconnectCooloffTimeouts; //Stores remaining cooloff period before the client can reconnect. (avoids accidental reconnects from rogue packets and connect/disconnect spam.

		std::shared_ptr<IMultiConnection> connection;
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> lastTime;
		FlowControl flowControl;
		std::unique_ptr<TickTimer> timer;

		std::mutex mutexConnectedNetViewMap;

		std::function<std::shared_ptr<INetworkViewComponent>(std::shared_ptr<MessageStructures::BaseMessage>)> onNetworkViewConnectCallback;
		std::function<void(GUID)> onNetworkViewDisconnectCallback;
		std::function<void()> doMessageProcessing;
		std::function<void(std::shared_ptr<Address> address)> onClientDisconnect;

	public:
		ServerNetworkingManager() : timer(std::make_unique<TickTimer>())
		{
			SetTickRate(std::chrono::milliseconds(5));
			timer->AddOnTickCallback(std::bind(&ServerNetworkingManager::UpdateComms, this));
		}

		~ServerNetworkingManager()
		{
			
		}

		static std::shared_ptr<ServerNetworkingManager>& GetInstance()
		{
			static std::shared_ptr<ServerNetworkingManager> instance = std::make_shared<ServerNetworkingManager>();
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
			/*case Unreliable:
				connection = NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);
				break;
			case Reliable:
				connection = NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
				break;*/
			case MultiUnreliable:
				connection =  NetworkServices::GetInstance().CreateMultiConnection(ProtocolId, TimeOut);
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
			ProcessMessages();
			SendUpdatedSnapshots();

			//clean-up disconnections
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedNetViewMap);
			
			std::vector<std::pair<GUID, bool>> toDelete;


			for (auto const& value : networkIDToComponent)
			{
				if (value.second->IsFlaggedForDeletion())
					toDelete.push_back(std::pair<GUID, bool>(value.second->GetUniqueID(), value.second->IsPrimaryPlayerView()));
			}

			for (auto const& value : toDelete)
				DisconnectNetworkView(value.first, value.second);

			connection->Update(deltaTimeSecs);

			for(auto& address : disconnectCooloffTimeouts)
			{
				address.second -= deltaTimeSecs;
			}
			
			for (AddressToFloatMap::iterator iter = disconnectCooloffTimeouts.begin(); iter != disconnectCooloffTimeouts.end(); )
			{
				if (iter->second < 0.0f)
					iter = disconnectCooloffTimeouts.erase(iter);
				else
					++iter;
			}

			//if (serverConnectionType == Reliable)
			//	flowControl.Update(deltaTimeSecs, static_cast<networking::ReliableConnection*>(connection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

			lastTime = nowTime;
		}

		void ProcessMessages()
		{
			if (doMessageProcessing != nullptr)
					doMessageProcessing();

			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedNetViewMap);

			for (NetworkIDMapType::iterator it = networkIDToComponent.begin(); it != networkIDToComponent.end(); ++it)
			{
				it->second->ProcessMessages();
			}



			//todo - collision detection confirmation...

			//todo - possible physics simulation

		}

		void ReceiveAndStorePackets()
		{
			//receive until theres no more packets left
			while (true)
			{
				unsigned char packet[256];
				std::shared_ptr<Address> sender = std::make_shared<Address>();
				int bytesRead = connection->ReceivePacket(packet, sizeof(packet), sender);
				if (bytesRead == 0)
					break;

				MessageStructures::BaseMessage tmpMsg;
				memcpy(&tmpMsg, packet, sizeof(MessageStructures::BaseMessage));
				std::shared_ptr<MessageStructures::BaseMessage> message = std::make_shared<MessageStructures::BaseMessage>(tmpMsg);

				ReadPacket(message, sender);
			}
		}

		void SendUpdatedSnapshots()
		{
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedNetViewMap);
			for (auto const& value : networkIDToComponent)
			{
				value.second->SendReceivedMessages(connection.get());
				value.second->ClearReceivedMessages();
			}
		}

		void ReadPacket(std::shared_ptr<MessageStructures::BaseMessage>& message, std::shared_ptr<Address>& sender)
		{
			if(message->simpleType == MessageStructures::Disconnect)
			{
				disconnectCooloffTimeouts[sender] = ReconnectCooloff;
			}

			//if new connection or a snapshot is received before a connect, make a new connection
			if(message->simpleType == MessageStructures::Connect ||
				message->simpleType == MessageStructures::SnapShot && 
				networkIDToComponent.find(message->uniqueID) == networkIDToComponent.end())
			{
				ConnectNetworkView(message, sender);
			}

			//route packet to network view 
			mutexConnectedNetViewMap.lock();
			if (networkIDToComponent.find(message->uniqueID) != networkIDToComponent.end())
				networkIDToComponent[message->uniqueID]->ReadPacket(message);
			mutexConnectedNetViewMap.unlock();
		}

		void AddNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			GUID id = component->GetUniqueID();
			std::lock_guard<std::mutex> lock(mutexConnectedNetViewMap);
			networkIDToComponent[id] = component;
		}

		void RemoveNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			RemoveNetworkViewComponent(component->GetUniqueID());
		}

		void RemoveNetworkViewComponent(GUID id)
		{
			std::lock_guard<std::mutex> lock(mutexConnectedNetViewMap);

			NetworkIDMapType::iterator it = networkIDToComponent.find(id);
			if (it != networkIDToComponent.end())
				networkIDToComponent.erase(it);
		}

		void OnStart() override
		{
			//start listening
		}

		void OnStop() override
		{
			//stop listening
		}

		void OnConnect(std::shared_ptr<Address> address) override
		{
			//client connected to server
			printf("Client connect: %s \n", address->toString().c_str());
		}


		void SendClientDisconnect(GUID playerNetViewID) const
		{
			if (connection->IsConnected())
			{
				MessageStructures::BaseMessage message;
				message.uniqueID = playerNetViewID;
				message.simpleType = MessageStructures::Disconnect;
				message.messageType = MessageStructures::Player;
				connection->SendPacket(reinterpret_cast<unsigned char*>(&message), sizeof(MessageStructures::BaseMessage));
			}
		}

		///Called when timeout occurs
		void OnDisconnect(std::shared_ptr<Address> address) override
		{
			//mutexConnectedNetViewMap.lock();
			//client timeout from server
			printf("Client disconnected: %s \n", address->toString().c_str());

			AddressToNetworkIDVectorMapType::iterator it = addressToNetworkIDs.find(address);

			if(it != addressToNetworkIDs.end())
			{
				disconnectCooloffTimeouts[address] = ReconnectCooloff;
				for (auto& netView : it->second)
				{
					if (!netView.expired())
					{
						GUID netViewID = netView.lock()->GetUniqueID();
						SendClientDisconnect(netViewID);
						DisconnectNetworkView(netViewID, false);
					}
				}
				addressToNetworkIDs.erase(it);
			}
			//mutexConnectedNetViewMap.unlock();

			if (onClientDisconnect != nullptr)
				onClientDisconnect(address);
		}

		void DisconnectNetworkView(GUID id, bool disconnect)
		{
			if (onNetworkViewDisconnectCallback != nullptr)
				onNetworkViewDisconnectCallback(id);


			if (disconnect)
			{
				std::shared_ptr<Address> owner = nullptr;
				for (auto& kvp : addressToNetworkIDs)
				{
					for (auto& networkID : kvp.second)
					{
						if (networkID.lock()->GetUniqueID() == id)
						{
							owner = kvp.first;
							break;
						}
					}
					if (owner != nullptr)
						break;
				}

				if (owner != nullptr)
				{
					connection->ForceDisconnectClient(owner);
				}
			}

			NetworkIDMapType::iterator it = networkIDToComponent.find(id);

			if (it != networkIDToComponent.end())
				networkIDToComponent.erase(it);

			//todo - clean up destroyed weak pointers
		}

		void ConnectNetworkView(std::shared_ptr<MessageStructures::BaseMessage> message, std::shared_ptr<Address>& owner)
		{
			AddressToFloatMap::iterator iter = disconnectCooloffTimeouts.find(owner);
			if(iter != disconnectCooloffTimeouts.end() && disconnectCooloffTimeouts[owner] > 0)
			{
				connection->ForceDisconnectClient(owner);

				DisconnectNetworkView(message->uniqueID, true);
				return;
			}

			if (onNetworkViewConnectCallback != nullptr)
			{
				std::shared_ptr<INetworkViewComponent> netView;

				netView = onNetworkViewConnectCallback(message);
				if (netView != nullptr)
					AddNetworkViewComponent(netView);

				addressToNetworkIDs[owner].push_back(netView);
			}
		}

		void SetOnNetworkViewConnectCallback(std::function<std::shared_ptr<INetworkViewComponent>(std::shared_ptr<MessageStructures::BaseMessage>)> callback)
		{
			onNetworkViewConnectCallback = callback;
		}

		void SetOnNetworkViewDisconnectCallback(std::function<void(GUID)> callback)
		{
			onNetworkViewDisconnectCallback = callback;
		}

		void SetDoMessageProcessingCallback(std::function<void()> callback)
		{
			doMessageProcessing = callback;
		}

		void SetOnClientDisconnectCallback(std::function<void(std::shared_ptr<Address>& address)> callback)
		{
			onClientDisconnect = callback;
		}
	};
}
