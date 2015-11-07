#pragma once
#include "ShaderManager.h"
#include "../Core/IListener.h"
#include "../Core/GameObject.h"

namespace Managers
{
	class SceneManager : public Core::IListener
	{
	public:
		SceneManager();
		~SceneManager();

		virtual void notifyBeginFrame();
		virtual void notifyDisplayFrame();
		virtual void notifyEndFrame();
		virtual void notifyReshape(int width,
			int height,
			int previous_width,
			int previous_height);
	private:
		Managers::ShaderManager* shaderManager;
		std::vector<std::reference_wrapper<Core::GameObject>> gameObjectManager;
	};
}

