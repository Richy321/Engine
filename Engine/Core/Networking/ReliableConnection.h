#pragma once
#include "Connection.h"
#include "ReliabilitySystem.h"

namespace networking
{
	class ReliableConnection : public Connection
	{
	public:
		ReliableConnection(unsigned int protocolId, float timeout, ISocket* socket, unsigned int max_sequence = 0xFFFFFFFF)
			: Connection(protocolId, timeout, socket)
		{
			ClearData();
		}

		~ReliableConnection()
		{
			if (IsRunning())
				Stop();
		}

		bool SendPacket(const unsigned char data[], const int size) override
		{
			unsigned char* packet = new unsigned char[HeaderSize + size];
			unsigned int seq = reliabilitySystem.GetLocalSequence();
			unsigned int ack = reliabilitySystem.GetRemoteSequence();
			unsigned int ack_bits = reliabilitySystem.GenerateAckBits();
			WriteHeader(packet, seq, ack, ack_bits);
			memcpy(packet + HeaderSize, data, size);
			if (!Connection::SendPacket(packet, size + HeaderSize))
				return false;
			reliabilitySystem.PacketSent(size);
			return true;
		}

		int ReceivePacket(unsigned char data[], int size) override
		{
			if (size <= HeaderSize)
				return false;

			unsigned char* packet = new unsigned char[HeaderSize + size];
			int receivedBytes = Connection::ReceivePacket(packet, size + HeaderSize);

			if (receivedBytes == 0)
				return false;
			if (receivedBytes <= HeaderSize)
				return false;

			unsigned int packet_sequence = 0;
			unsigned int packet_ack = 0;
			unsigned int packet_ack_bits = 0;

			ReadHeader(packet, packet_sequence, packet_ack, packet_ack_bits);
			reliabilitySystem.PacketReceived(packet_sequence, receivedBytes - HeaderSize);
			reliabilitySystem.ProcessAck(packet_ack, packet_ack_bits);
			memcpy(data, packet + HeaderSize, receivedBytes - HeaderSize);
			return receivedBytes - HeaderSize;

		}

		int GetHeaderSize() const
		{
			return Connection::GetHeaderSize() + reliabilitySystem.GetHeaderSize();
		}

		ReliabilitySystem& GetReliabilitySystem()
		{
			return reliabilitySystem;
		}

	protected:

		void WriteInteger(unsigned char * data, unsigned int value)
		{
			data[0] = (unsigned char)(value >> 24);
			data[1] = (unsigned char)((value >> 16) & 0xFF);
			data[2] = (unsigned char)((value >> 8) & 0xFF);
			data[3] = (unsigned char)(value & 0xFF);
		}

		void WriteHeader(unsigned char * header, unsigned int sequence, unsigned int ack, unsigned int ack_bits)
		{
			WriteInteger(header, sequence);
			WriteInteger(header + 4, ack);
			WriteInteger(header + 8, ack_bits);
		}

		void ReadInteger(const unsigned char * data, unsigned int & value)
		{
			value = (
				((unsigned int)data[0] << 24) |
				((unsigned int)data[1] << 16) |
				((unsigned int)data[2] << 8) |
				((unsigned int)data[3]));
		}

		void ReadHeader(const unsigned char * header, unsigned int & sequence, unsigned int & ack, unsigned int & ack_bits)
		{
			ReadInteger(header, sequence);
			ReadInteger(header + 4, ack);
			ReadInteger(header + 8, ack_bits);
		}

		virtual void OnStop()
		{
			ClearData();
			Connection::OnStop();
		}

		virtual void OnDisconnect()
		{
			ClearData();
			Connection::OnDisconnect(address);
		}

	private:
		ReliabilitySystem reliabilitySystem;	// reliability system: manages sequence numbers and acks, tracks network stats etc.
		static const int HeaderSize = 12;

		void ClearData()
		{
			reliabilitySystem.Reset();
		}
	};
}
