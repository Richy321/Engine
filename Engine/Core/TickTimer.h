#pragma once
#include <thread>
#include <vector>
#include <algorithm>
namespace Core
{
	class TickTimer
	{
	private:
		bool isActive = false;
	public:
		std::vector<std::function<void()>> callbackFunctions;
		std::chrono::milliseconds interval = std::chrono::milliseconds(5);

		void Start()
		{
			isActive = true;
			std::thread([&]
			{
				while (isActive)
				{
					std::this_thread::sleep_for(interval);

					for each (auto function in callbackFunctions)
						function();
				}
			}).detach();
		}

		void SetTickIntervalMilliseconds(const std::chrono::milliseconds interval)
		{
			this->interval = interval;
		}

		void AddOnTickCallback(std::function<void()> callback)
		{
			callbackFunctions.push_back(callback);
		}

		/*void RemoveOnTickCallback(std::function<void()> callback)
		{
			callbackFunctions.erase(std::remove(callbackFunctions.begin(), callbackFunctions.end(), callback), callbackFunctions.end());
		}*/

		void Stop()
		{
			isActive = false;
		}
	};
}
