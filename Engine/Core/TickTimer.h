#pragma once
#include <thread>
#include <vector>
#include <algorithm>
#include <atomic>

namespace Core
{
	class TickTimer
	{
	private:
		std::atomic<bool> isActive = false;
	public:
		std::vector<std::function<void()>> callbackFunctions;
		std::chrono::milliseconds interval = std::chrono::milliseconds(5);
		std::thread thread;

		~TickTimer()
		{
			printf("Timer Destructor");
			isActive = false;
			//Thread.Terminate will be called on destruction if still running
		}

		void Start()
		{
			isActive = true;
			thread = std::thread(&TickTimer::Tick, this);
		}

		void Tick()
		{
			while (isActive)
			{
				std::this_thread::sleep_for(interval);

				for each (auto function in callbackFunctions)
					if (function != nullptr)
						function();
			}
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
			thread.join();
		}
	};
}
