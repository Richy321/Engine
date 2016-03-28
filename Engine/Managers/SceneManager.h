#pragma once
#include "../Dependencies/glm/glm.hpp"
#include "ShaderManager.h"
#include "../Core/IListener.h"
#include "../Core/GameObject.h"
#include "../Core/Timer.h"
#include <memory>
#include "../Core/Initialisation/WindowInfo.h"
#include "../Core/Camera.h"
#include "../Core/Lights.h"
#include <mutex>


using namespace glm;

namespace Managers
{
	class SceneManager : public Core::IListener
	{
	private:
		bool captureCursor = false;
		bool isWarpingCursor = false;
	protected:
		SceneManager(Core::Initialisation::WindowInfo winInfo);
		std::vector<std::shared_ptr<Core::GameObject>> gameObjectManager;
		
		std::unique_ptr<Core::Timer> timer;
		float lastUpdateTime;
		float lastUpdateCommsTime;
		Core::Initialisation::WindowInfo windowInfo;

		std::weak_ptr<Core::Camera> mainCamera;
		int mousePosX;
		int mousePosY;
		int mouseDeltaX;
		int mouseDeltaY;
		bool paused = false;

		std::mutex mutexGameObjectManager;
	public:
		static std::map<unsigned char, bool> keyState;

		~SceneManager();

		virtual void Initialise() override;

		virtual void OnUpdate(float deltaTime) {};
		virtual void OnPhysicsUpdate() {};
		virtual void OnCommsUpdate(float deltaTime) {};

		virtual void notifyBeginFrame() override;
		virtual void notifyDisplayFrame() override;
		virtual void notifyEndFrame() override;
		virtual void notifyReshape(int width, int height, int previous_width, int previous_height) override;

		virtual void notifyProcessNormalKeys(unsigned char key, int x, int y) override 
		{
			if (key == 27) //escape
				exit(0);

			if (key == 'p')
				paused = !paused;

			keyState[key] = true;
		}

		virtual void notifyProcessNormalKeysUp(unsigned char key, int x, int y) override 
		{
			keyState[key] = false;
		}

		virtual void notifyProcessSpecialKeys(int key, int x, int y) override {}

		virtual void notifyProcessMouseState(int button, int state, int x, int y) override {}
		virtual void notifyProcessMouseActiveMove(int x, int y) override {}
		void notifyProcessMousePassiveMove(int x, int y) override 
		{
			if (isWarpingCursor)
			{
				isWarpingCursor = false;
				return;
			}
			if (mousePosX != -1 && mousePosY != -1) //ignore first delta
			{
				mouseDeltaX = x - mousePosX;
				mouseDeltaY = y - mousePosY;
			}

			if (captureCursor)
			{
				isWarpingCursor = true;
				glutWarpPointer(windowInfo.width / 2, windowInfo.height / 2);
				x = windowInfo.width / 2;
				y = windowInfo.height / 2;
			}

			mousePosX = x;
			mousePosY = y;
			OnMousePassiveMove(mousePosX, mousePosY, mouseDeltaX, mouseDeltaY);
		}
		virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) {}

		virtual void notifyProcessMouseWindowEntryCallback(int state) override {}

		virtual void notifyErrorCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam) override;


		virtual void SetMainCamera(std::weak_ptr<Core::Camera> cam) { mainCamera = cam; }

		void CaptureCursor(bool capture)
		{
			captureCursor = capture;
			glutSetCursor(!capture ? GLUT_CURSOR_INHERIT : GLUT_CURSOR_NONE);
		}
	};
}

