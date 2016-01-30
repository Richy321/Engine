#pragma once
#include "SharedNetworkConfiguration.h"
#include "Address.h"
#include <cstdio>
#include "NetworkServices.h"
#include "FlowControl.h"
#include <chrono>
#include <map>
#include <memory>
#include "../Components/NetworkViewComponent.h"
#include "../Utils.h"
#include "IClientNetworkManager.h"
#include "Core/TickTimer.h"
#include <ctime>

namespace networking
{
	class ClientNetworkManager : public INetworkManager, public IConnectionEventHandler
	{
	private:
		IConnection* serverConnection;
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> lastTime;
		FlowControl flowControl;
		bool connected = false;
		std::unique_ptr<TickTimer> timer;

		std::function<void()> onComponentConnectCallback;
		std::function<void()> onComponentDisconnectCallback;

		std::mutex mutexConnectedNetViewMap;

		void AddOnComponentConnectCallback(std::function<void()> callback)
		{
			onComponentConnectCallback = callback;
		}

		void AddOnComponentDisconnectCallback(std::function<void()> callback)
		{
			onComponentDisconnectCallback = callback;
		}

		typedef std::map<GUID, std::shared_ptr<INetworkViewComponent>, Utils::GUIDComparer> NetworkIDMapType;
		NetworkIDMapType networkIDToComponent;
	public:
		static std::shared_ptr<ClientNetworkManager>& GetInstance()
		{
			static std::shared_ptr<ClientNetworkManager> instance = std::make_shared<ClientNetworkManager>();
			return instance;
		}

		ClientNetworkManager() : timer(std::make_unique<TickTimer>())
		{
			SetTickRate(std::chrono::milliseconds(5));
			timer->AddOnTickCallback(std::bind(&ClientNetworkManager::UpdateComms, this));
		}

		~ClientNetworkManager()
		{
			timer->Stop();
		}

		void InitialiseConnectionToServer()
		{
			switch (clientConnectionType)
			{
			case Unreliable:
				serverConnection = NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);
				break;
			case Reliable:
				serverConnection = NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
				break;
			//case MultiUnreliable:
			//	serverConnection = networking::NetworkServices::GetInstance().CreateMultiConnection(ProtocolId, TimeOut);
			//	break;
			}

			serverConnection->SetConnectionEventHandler(shared_from_this());

			int startClientPort = ClientPort;
			while(!serverConnection->Start(startClientPort) && startClientPort < startClientPort + 50)
			{
				printf("could not start connection on port %d\n", startClientPort);
				startClientPort++;
			}

			if (startClientPort == ClientPort + 50)
				return; //could not find a free port

			serverConnection->Connect(Address(127, 0, 0, 1, ServerPort));

			timer->Start();
			startTime = std::chrono::system_clock::now();
			lastTime = startTime;
		}

		void UpdateComms()
		{
			std::chrono::time_point<std::chrono::system_clock> nowTime = std::chrono::system_clock::now();
			std::chrono::milliseconds deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastTime);
			float deltaTimeSecs = deltaTime.count() * 0.001f;
			std::chrono::milliseconds fromStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime);
			 
			//printf("Comms update: %f \n", fromStartTime.count() * 0.001f);

			if (!connected && serverConnection->IsConnected())
			{
				printf("client connected to server\n");
				connected = true;
			}

			if (!connected && serverConnection->ConnectFailed())
			{
				//printf("connection failed\n");
			}

			if (serverConnection->IsConnected())
			{
				if (clientConnectionType == Reliable)
					flowControl.Update(deltaTimeSecs, static_cast<ReliableConnection*>(serverConnection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
			}

			const float sendRate = flowControl.GetSendRate();

			ReceiveAndRoutePackets();
			ProcessMessages();
			SendComponentPackets();

			serverConnection->Update(deltaTimeSecs);
			lastTime = nowTime;
		}

		void ReceiveAndRoutePackets()
		{
			while (true)
			{
				unsigned char packet[256];
				int bytes_read = serverConnection->ReceivePacket(packet, sizeof(packet));
				if (bytes_read == 0)
					break;

				std::shared_ptr<MessageStructures::BaseMessage> message = std::make_shared<MessageStructures::BaseMessage>();

				memcpy(message.get(), packet, sizeof(MessageStructures::BaseMessage));

				NetworkIDMapType::iterator it;

				it = networkIDToComponent.find(message->uniqueID);
				if (it == networkIDToComponent.end())
				{
					//New network component, pass back to caller
					OnComponentConnect();
				}

				if (it != networkIDToComponent.end())
					it->second->ReadPacket(message);
			}
		}

		void ProcessMessages()
		{
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedNetViewMap);

			for (NetworkIDMapType::iterator it = networkIDToComponent.begin(); it != networkIDToComponent.end(); ++it)
			{
				it->second->ProcessMessages();
			}
		}

		void SendComponentPackets()
		{
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedNetViewMap);
			//todo - look at grouping into one packet for all components (group into state snapshot)
			for (NetworkIDMapType::iterator it = networkIDToComponent.begin(); it != networkIDToComponent.end(); ++it)
			{
				if (it->second->IsSendUpdates())
				{
					std::shared_ptr<MessageStructures::BaseMessage> message = std::make_shared<MessageStructures::BaseMessage>();
					int size = it->second->BuildPacket(message);
					serverConnection->SendPacket(reinterpret_cast<unsigned char*>(message.get()), size);
				}
				it->second->ClearReceivedMessages();
			}
		}

		void Destroy()
		{
			delete serverConnection;
		}

		void AddNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			networkIDToComponent[component->GetUniqueID()] = component;
		}

		void RemoveNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			NetworkIDMapType::iterator it = networkIDToComponent.find(component->GetUniqueID());
			if (it != networkIDToComponent.end())
				networkIDToComponent.erase(it);
		}

		void SetTickRate(std::chrono::milliseconds interval)
		{
			timer->SetTickIntervalMilliseconds(interval);
		}

		void OnComponentConnect()
		{
			if (onComponentConnectCallback != nullptr)
				onComponentConnectCallback();
		}

		void OnComponentDisconnect()
		{
			if (onComponentDisconnectCallback != nullptr)
				onComponentDisconnectCallback();
		}

		void OnStart()
		{
			
		}

		virtual void OnStop()
		{
			
		}

		virtual void OnConnect(std::shared_ptr<Address> address)
		{
			//connected to server
		}
		virtual void OnDisconnect(std::shared_ptr<Address> address)
		{
			//timeout from server
		}

	};
}
