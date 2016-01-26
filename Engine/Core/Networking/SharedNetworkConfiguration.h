#pragma once
enum ConnectionType
{
	Unreliable,
	Reliable,
	MultiUnreliable
};

const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x99887766;
const float TimeOut = 100.0f;
const int CommsTickDurationMs = 250;

const int MaxBufferLength = 512;

const static ConnectionType clientConnectionType = Unreliable;
const static ConnectionType serverConnectionType = MultiUnreliable;

const static networking::Address ServerAddress(127, 0, 0, 1, ServerPort);

