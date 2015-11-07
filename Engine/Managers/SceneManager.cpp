#include "SceneManager.h"
#include "../Core/GameObject.h"
using namespace Managers;
using namespace Core;

SceneManager::SceneManager()
{
	glEnable(GL_DEPTH_TEST);
	shaderManager = new ShaderManager();
	shaderManager->CreateProgram("colorShader", "Shaders\\basicPositionColor.vert", "Shaders\\basicColor.frag");

	meshManager = new MeshManager();

	GameObject* triangleGO = new GameObject();

	gameObjectManager.push_back(*triangleGO);
}

SceneManager::~SceneManager()
{
	delete shaderManager;
	delete meshManager;
}

void SceneManager::notifyBeginFrame()
{
	//nothing here for the moment
	meshManager->Update();
}

void SceneManager::notifyDisplayFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	meshManager->Draw();
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