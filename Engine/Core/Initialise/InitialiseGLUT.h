#pragma once
#include "../ContextInfo.h"
#include "../FrameBufferInfo.h"
#include "../WindowInfo.h"
#include "../IListener.h"
#include "InitialiseGLEW.h"
#include <iostream>
namespace Core
{
	namespace Initialisation
	{
		class InitialiseGLUT
		{
		private:
			static Core::IListener* listener;
			static Core::WindowInfo windowInformation;

		public: 
			static void Initialise(const Core::WindowInfo& window,
				const Core::ContextInfo& context,
				const Core::FramebufferInfo& framebufferInfo);

		public:
			static void SetListener(Core::IListener*& iListener);
			static void Run();//called from outside
			static void Close();

			void EnterFullscreen();
			void ExitFullscreen();

			//used to print info about GL
			static void PrintOpenGLInfo(const Core::WindowInfo& windowInfo,
				const Core::ContextInfo& context);
		private:
			static void IdleCallback();
			static void DisplayCallback();
			static void ResizeCallback(int width, int height);
			static void CloseCallback();
		};
	}
}
