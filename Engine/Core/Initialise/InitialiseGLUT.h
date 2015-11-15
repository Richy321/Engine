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

			static void EnterFullscreen();
			static void ExitFullscreen();

			//used to print info about GL
			static void PrintOpenGLInfo(const Core::WindowInfo& windowInfo,
				const Core::ContextInfo& context);
		private:
			static void IdleCallback();
			static void DisplayCallback();
			static void ResizeCallback(int width, int height);
			static void CloseCallback();

			static void ProcessNormalKeysCallback(unsigned char key, int x, int y);
			static void ProcessSpecialKeysCallback(int key, int x, int y);

			static void ProcessMouseStateCallback(int button, int state, int x, int y);
			static void ProcessMouseActiveMoveCallback(int x, int y);
			static void ProcessMousePassiveMoveCallback(int x, int y);
			static void ProcessMouseWindowEntryCallback(int state);
		};
	}
}
