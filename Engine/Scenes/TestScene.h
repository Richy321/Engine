#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"

using namespace Core;

class TestScene : public Managers::SceneManager
{
public:

	std::shared_ptr<Camera> camera;
	GLuint gViewUniform;
	GLuint gProjectionUniform;

	TestScene() : camera(new Camera)
	{
	}

	~TestScene()
	{
	}

	void Initialise() override
	{
		SceneManager::Initialise();

		gViewUniform = glGetUniformLocation(Managers::ShaderManager::GetInstance().GetShader("colorShader"), "gView");
		gProjectionUniform = glGetUniformLocation(Managers::ShaderManager::GetInstance().GetShader("colorShader"), "gProjection");
		//assert(gViewUniform != 0xFFFFFFFF);
		//assert(gProjectionUniform != 0xFFFFFFFF);

		GameObject *cube = new GameObject();

		cube->AddComponent(*MeshComponent::CreateCubePrimitive());
		
		gameObjectManager.push_back(*cube);

		cube->world.Scale(0.25f);
	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);

		glUniformMatrix4fv(gViewUniform, 1, GL_FALSE, camera->view.GetMatrixFloatValues());
		glUniformMatrix4fv(gProjectionUniform, 1, GL_FALSE, camera->projection.GetMatrixFloatValues());

		//gameObjectManager[0].get().world.RotateZ(deltaTime * 50);
		gameObjectManager[0].get().world.RotateY(deltaTime * 50);
		gameObjectManager[0].get().world.RotateX(deltaTime * 50);
	}
};

