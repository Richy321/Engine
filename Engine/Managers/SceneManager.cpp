#include "SceneManager.h"
#include "../Core/GameObject.h"
using namespace Managers;
using namespace Core;

SceneManager::SceneManager(Core::WindowInfo windowInfo) : timer(new Timer), lastUpdateTime(0.0f)
{
	this->windowInfo = windowInfo;
}

SceneManager::~SceneManager()
{

}

void SceneManager::Initialise()
{
	glEnable(GL_DEPTH_TEST);
	ShaderManager::GetInstance().CreateProgram("colorShader", "Shaders\\test.vert", "Shaders\\basicColor.frag");
	timer->Start();
}

void SceneManager::notifyBeginFrame()
{
	timer->Update();
	OnUpdate(timer->GetElapsedTime());
	//call these from here for the time being
	OnPhysicsUpdate(); 
	OnCommsUpdate();
	for(std::shared_ptr<GameObject>& go : gameObjectManager)
	{
		go->Update(0.0f);
	}
}

void SceneManager::notifyDisplayFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	for(std::shared_ptr<GameObject>& go : gameObjectManager)
	{
		if(!mainCamera.expired())
		{
			std::shared_ptr<Camera> mainCamPtr = mainCamera.lock();
			go->Render(mainCamPtr->view, mainCamPtr->projection);
		}
		else
		{
			mat4 identity;
			go->Render(identity, identity);
		}
	}
}

void SceneManager::notifyEndFrame()
{
	//nothing here for the moment
}

void SceneManager::notifyReshape(int width,
	int height,
	int previous_width,
	int previous_height)
{
	//nothing here for the moment 
}




