#pragma once
#include <fstream>

namespace networking
{
	class FlowControl
	{
	public:

		FlowControl()
		{
			printf("flow control initialized\n");
			Reset();
		}

		void Reset()
		{
			mode = Bad;
			penaltyTime = 4.0f;
			goodConditionsTime = 0.0f;
			penaltyReductionAccumulator = 0.0f;
		}


		void Update(float deltaTime, float rtt)
		{
			const float RTT_Threshold = 250.0f;

			if (mode == Good)
			{
				if (rtt > RTT_Threshold)
				{
					printf("*** dropping to bad mode ***\n");
					mode = Bad;
					if (goodConditionsTime < 10.0f && penaltyTime < 60.0f)
					{
						penaltyTime *= 2.0f;
						if (penaltyTime > 60.0f)
							penaltyTime = 60.0f;

						printf("penalty time increased to %.1f\n", penaltyTime);
						goodConditionsTime = 0.0f;
						penaltyReductionAccumulator = 0.0f;
						return;
					}

					goodConditionsTime += deltaTime;
					penaltyReductionAccumulator += deltaTime;

					if (penaltyReductionAccumulator > 10.0f && penaltyTime > 1.0f)
					{
						penaltyTime *= 0.5f;
						if (penaltyTime < 1.0f)
							penaltyTime = 1.0f;

						printf("penalty time reduced to %.1f\n", penaltyTime);
						penaltyReductionAccumulator = 0.0f;
					}
				}
			}
			if (mode == Bad)
			{
				if (rtt <= RTT_Threshold)
					goodConditionsTime += deltaTime;
				else
					goodConditionsTime = 0.0f;

				if (goodConditionsTime > penaltyTime)
				{
					printf("*** upgrading to good mode ***\n");
					goodConditionsTime = 0.0f;
					penaltyReductionAccumulator = 0.0f;
					mode = Good;
					return;
				}
			}
		}

		float GetSendRate()
		{
			return mode == Good ? 30.0f : 10.0f;
		}

	private:
		enum Mode
		{
			Good,
			Bad
		};

		Mode mode;
		float penaltyTime;
		float goodConditionsTime;
		float penaltyReductionAccumulator;
	};
}

