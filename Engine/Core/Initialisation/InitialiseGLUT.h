#pragma once
#include "../IListener.h"
#include "InitialiseGLEW.h"
#include <iostream>
#include "WindowInfo.h"
#include "FramebufferInfo.h"
#include "ContextInfo.h"
#include <memory>

namespace Core
{
	namespace Initialisation
	{
		class InitialiseGLUT
		{
		private:
			static std::shared_ptr<IListener> listener;
			static WindowInfo windowInformation;

		public: 
			static void Initialise(const WindowInfo& window,
				const ContextInfo& context,
				const Core::Initialisation::FramebufferInfo& framebufferInfo);

		public:
			static void SetListener(std::shared_ptr<IListener>& iListener);
			static void Run();//called from outside
			static void Close();

			static void EnterFullscreen();
			static void ExitFullscreen();

			//used to print info about GL
			static void PrintOpenGLInfo(const WindowInfo& windowInfo,
				const ContextInfo& context);
		private:
			static void IdleCallback();
			static void DisplayCallback();
			static void ResizeCallback(int width, int height);
			static void CloseCallback();

			static void ProcessNormalKeysCallback(unsigned char key, int x, int y);
			static void ProcessNormalKeysUpCallback(unsigned char key, int x, int y);
			static void ProcessSpecialKeysCallback(int key, int x, int y);

			static void ProcessMouseStateCallback(int button, int state, int x, int y);
			static void ProcessMouseActiveMoveCallback(int x, int y);
			static void ProcessMousePassiveMoveCallback(int x, int y);
			static void ProcessMouseWindowEntryCallback(int state);

			static void APIENTRY ProcessErrorCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam);
		};
	}
}
