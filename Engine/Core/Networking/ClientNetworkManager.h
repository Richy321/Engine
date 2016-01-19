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
	class ClientNetworkManager : public IClientNetworkManager
	{
	private:
		ClientNetworkManager(): timer(std::make_unique<TickTimer>())
		{
			SetTickRate(std::chrono::milliseconds(5));
			//timer->AddOnTickCallback(std::bind(&ClientNetworkManager::UpdateComms, this));
		}
		IConnection* serverConnection;
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> lastTime;
		FlowControl flowControl;
		bool connected = false;
		std::unique_ptr<TickTimer> timer;

		typedef std::map<GUID, std::shared_ptr<INetworkViewComponent>, Utils::GUIDComparer> NetworkIDMapType;
		NetworkIDMapType networkIDToComponent;
	public:
		static ClientNetworkManager& GetInstance()
		{
			static ClientNetworkManager instance;
			return instance;
		}

		~ClientNetworkManager()
		{
		}

		void InitialiseConnectionToServer()
		{
			if (isUseReliableConnection)
				serverConnection = networking::NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
			else
				serverConnection = networking::NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);

			if (!serverConnection->Start(ClientPort))
			{
				printf("could not start connection on port %d\n", ClientPort);
				return;
			}

			serverConnection->Connect(networking::Address(127, 0, 0, 1, ServerPort));

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
				printf("connection failed\n");
			}

			if (serverConnection->IsConnected())
			{
				if (isUseReliableConnection)
					flowControl.Update(deltaTimeSecs, static_cast<ReliableConnection*>(serverConnection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
			}

			const float sendRate = flowControl.GetSendRate();

			SendComponentPackets();

			ReceiveAndRoutePackets();
			

			serverConnection->Update(deltaTimeSecs);
			lastTime = nowTime;
		}

		void SendComponentPackets()
		{
			//todo - look at grouping into one packet for all components
			for (NetworkIDMapType::iterator it = networkIDToComponent.begin(); it != networkIDToComponent.end(); ++it)
			{
				MessageStructures::BaseMessage message;
				int size = it->second->BuildPacket(message);
				serverConnection->SendPacket(reinterpret_cast<unsigned char*>(&message), size);
			}
		}

		void ReceiveAndRoutePackets()
		{
			while (true)
			{
				unsigned char packet[256];
				int bytes_read = serverConnection->ReceivePacket(packet, sizeof(packet));
				if (bytes_read == 0)
					break;

				MessageStructures::BaseMessage message;
				memcpy(&message, packet, sizeof(MessageStructures::BaseMessage));

				NetworkIDMapType::iterator it;

				it = networkIDToComponent.find(message.uniqueID);
				if (it != networkIDToComponent.end())
				{
					it->second->ReadPacket(message);
					//printf("received packet from server\n");
				}
				else
				{
					printf("packet from server component GUID not found\n");
				}
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

		void NetworkCommsCallback()
		{
			
		}
	};
}
