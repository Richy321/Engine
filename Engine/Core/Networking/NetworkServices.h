#pragma once
#include "PlatformDetection.h"
#include "INetworkService.h"
#include "NetworkServicesWindows.h"
#include "NetworkServicesUnix.h"
#include "ISocket.h"

namespace networking
{
	class NetworkServices
	{
	public:
		static INetworkService& GetInstance()
		{
		#if PLATFORM == PLATFORM_WINDOWS
			static INetworkService& services = NetworkServicesWindows();
			return services;
		#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
			static INetworkService services = NetworkServicesUnix();
			return services;
		#endif
		}
	};
}

