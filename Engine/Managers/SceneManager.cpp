#include "SceneManager.h"
#include "../Core/GameObject.h"
#include "../Core/Components/MeshComponent.h"
using namespace Managers;
using namespace Core;

SceneManager::SceneManager()
{
	glEnable(GL_DEPTH_TEST);
	shaderManager = new ShaderManager();
	shaderManager->CreateProgram("colorShader", "Shaders\\basicPositionColor.vert", "Shaders\\basicColor.frag");

	GameObject* triangleGO = new GameObject();

	MeshComponent* tri = MeshComponent::CreateTrianglePrimitive();
	tri->SetProgram(ShaderManager::GetShader("colorShader"));
	triangleGO->AddComponent(*tri);

	MeshComponent* quad = MeshComponent::CreateQuadPrimitive();
	quad->SetProgram(ShaderManager::GetShader("colorShader"));
	triangleGO->AddComponent(*quad);


	gameObjectManager.push_back(*triangleGO);
}

SceneManager::~SceneManager()
{
	delete shaderManager;
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