#include "SceneManager.h"
#include "../Core/GameObject.h"
#include "../Core/Components/MeshComponent.h"
using namespace Managers;
using namespace Core;

SceneManager::SceneManager()
{
	glEnable(GL_DEPTH_TEST);
	ShaderManager::GetInstance().CreateProgram("colorShader", "Shaders\\test.vert", "Shaders\\basicColor.frag");

	GameObject* triangleGO = new GameObject();

	triangleGO->AddComponent(*MeshComponent::CreateTrianglePrimitive());
	triangleGO->AddComponent(*MeshComponent::CreateQuadPrimitive());

	gameObjectManager.push_back(*triangleGO);

	
	triangleGO->world.Translate(0.5, 0.0f, 0.0f);
	triangleGO->world.RotateZ(90.0f);
}

SceneManager::~SceneManager()
{

}

void SceneManager::notifyBeginFrame()
{
	for each (GameObject& go in gameObjectManager)
	{
		go.Update(0.0f);
	}
}

void SceneManager::notifyDisplayFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	for each (GameObject& go in gameObjectManager)
	{
		go.Render();
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




