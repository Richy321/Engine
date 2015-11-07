#pragma once
#include "ShaderManager.h"
#include "MeshManager.h"
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
		Managers::MeshManager* meshManager;
		std::vector<std::reference_wrapper<Core::GameObject>> gameObjectManager;
	};
}

