//-----------------------------------------//
//	Timer.h							       //
//	Timer Class							   //
//	uses QueryPerformanceFrequency/counter //
//										   //
//	RJ Fox 11/2009					       //
//-----------------------------------------//

#pragma once

#include <windows.h>

namespace Core
{
	class Timer
	{
	private:
		INT64		m_ticksPerSecond;	//frequency of the counter per second.
		INT64		m_currentTime;		//current counter time.
		INT64		m_lastTime;			//last time it updated.
		INT64		m_lastFPSUpdate;	//internal time the last fps update was
		INT64		m_FPSUpdateInterval;//interval to update the fps 
		INT64		m_FPSUpdate;		//
		INT64		m_stopTime;			//
		UINT		m_numFrames;		//stores frame ctr between updates.
		float		m_runningTime;		//total elapsed time of the counter
		float		m_timeElapsed;		//how long the counter has been running since last update (in seconds)
		float		m_fps;				//frames per second
		int			m_timerState;		//counter time it stopped
	public:
		Timer()
		{
			//get times a second the counter will increment
			QueryPerformanceFrequency((LARGE_INTEGER *)&m_ticksPerSecond);
			m_currentTime = m_lastTime = m_lastFPSUpdate = m_stopTime = m_numFrames = 0;
			m_runningTime = m_timeElapsed = m_fps = 0.0f;
			m_FPSUpdateInterval = m_ticksPerSecond >> 1;	//bitshift right (div by 2)
			m_timerState = STOPPED;
		}
		void Start()
		{
			if (m_timerState == STARTED)
			{
				//already running
				return;
			}
			//Get counter value when timer was started and put in m_lastTime.
			QueryPerformanceCounter((LARGE_INTEGER *)&m_lastTime);
			m_timerState = STARTED;
		}
		void Stop()
		{
			if (m_timerState != STARTED)
			{
				//already stopped
				return;
			}
			m_stopTime = 0;
			//Get counter value when timer was stopped and put in stopTime.
			QueryPerformanceCounter((LARGE_INTEGER *)&m_stopTime);

			//calc running time.
			m_runningTime += (float)(m_stopTime - m_lastTime) / (float)m_ticksPerSecond;
			m_timerState = STOPPED;
		}
		void Pause()
		{
			if (m_timerState == STOPPED || m_timerState == PAUSED)
			{
				//already stopped / paused
				return;
			}
			m_stopTime = 0;
			//Get counter value when timer was stopped and put in stopTime.
			QueryPerformanceCounter((LARGE_INTEGER *)&m_stopTime);

			//calc running time.
			m_runningTime += (float)(m_stopTime - m_lastTime) / (float)m_ticksPerSecond;
			m_timerState = PAUSED;
		}
		void Resume()
		{
			if (m_timerState != PAUSED)
			{
				//already running or wasn't running in the first place
				return;
			}


			m_lastTime = m_stopTime;
			m_timerState = STARTED;
		}

		///Updates the frames per second + total running time
		void Update()
		{
			if (m_timerState != STARTED)
			{
				return;	//not running
			}

			//Get the current time
			QueryPerformanceCounter((LARGE_INTEGER *)&m_currentTime);


			m_timeElapsed = (float)(m_currentTime - m_lastTime) / (float)m_ticksPerSecond;
			m_runningTime += m_timeElapsed;

			//Update FPS
			m_numFrames++;
			if (m_currentTime - m_lastFPSUpdate >= m_FPSUpdateInterval)
			{
				float currentTime = (float)m_currentTime / (float)m_ticksPerSecond;
				float lastTime = (float)m_lastFPSUpdate / (float)m_ticksPerSecond;
				m_fps = (float)m_numFrames / (currentTime - lastTime);
				m_lastFPSUpdate = m_currentTime;
				m_numFrames = 0;
			}

			m_lastTime = m_currentTime;
		}
		enum timerState { STARTED = 1, STOPPED = 2, PAUSED = 3 };

		int getTimerState() { return m_timerState; }
		float GetFPS() { return m_fps; }
		float GetRunningTime() { return m_runningTime; }
		float GetElapsedTime()
		{
			if (m_timerState != STARTED)
				return 0.0f;
			else
				return m_timeElapsed;
		}
		long GetTime()
		{
			INT64 time;
			QueryPerformanceCounter((LARGE_INTEGER *)&time);
			return (long)time;
		}
	};
}