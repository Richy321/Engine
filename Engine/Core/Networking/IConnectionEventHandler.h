#pragma once
class IConnectionEventHandler : public std::enable_shared_from_this<IConnectionEventHandler>
{
public:
	virtual void OnStart() = 0;
	virtual void OnStop() = 0;
	virtual void OnConnect() = 0;
	virtual void OnDisconnect() = 0;

	IConnectionEventHandler()
	{
	}

	virtual ~IConnectionEventHandler()
	{
	}
};

