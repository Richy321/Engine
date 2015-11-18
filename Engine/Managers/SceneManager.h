#pragma once
#include "ShaderManager.h"
#include "../Core/IListener.h"
#include "../Core/GameObject.h"
#include "../Core/Timer.h"
#include <memory>
#include "../Core/WindowInfo.h"
#include "../Core/Camera.h"

namespace Managers
{
	class SceneManager : public Core::IListener
	{
	protected:
		SceneManager(Core::WindowInfo winInfo);
		std::vector<std::shared_ptr<Core::GameObject>> gameObjectManager;
		std::unique_ptr<Core::Timer> timer;
		float lastUpdateTime;
		Core::WindowInfo windowInfo;

		std::weak_ptr<Core::Camera> mainCamera;
	public:
		~SceneManager();

		virtual void Initialise() override;

		virtual void OnUpdate(float deltaTime) {};
		virtual void OnPhysicsUpdate() {};
		virtual void OnCommsUpdate() {};

		virtual void notifyBeginFrame() override;
		virtual void notifyDisplayFrame() override;
		virtual void notifyEndFrame() override;
		virtual void notifyReshape(int width, int height, int previous_width, int previous_height) override;

		virtual void notifyProcessNormalKeys(unsigned char key, int x, int y) override {}
		virtual void notifyProcessSpecialKeys(int key, int x, int y) override {}

		virtual void notifyProcessMouseState(int button, int state, int x, int y) override {}
		virtual void notifyProcessMouseActiveMove(int x, int y) override {}
		virtual void notifyProcessMousePassiveMove(int x, int y) override {}
		virtual void notifyProcessMouseWindowEntryCallback(int state) override {}

		virtual void SetMainCamera(std::weak_ptr<Core::Camera> cam) { mainCamera = cam; }
	};
}

