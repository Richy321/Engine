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
	GLuint gWP;
	std::shared_ptr<GameObject> cube;
	std::shared_ptr<GameObject> cubeBack;

	TestScene(WindowInfo windowInfo) : SceneManager(windowInfo), camera(new Camera)
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
		gWP = glGetUniformLocation(Managers::ShaderManager::GetInstance().GetShader("colorShader"), "gWP");

		//assert(gViewUniform != 0xFFFFFFFF);
		//assert(gProjectionUniform != 0xFFFFFFFF);

		camera->SetPerspectiveProjection(30.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 0.1f, 1000.0f);

		
		cube = std::make_shared<GameObject>();
		cube->AddComponent(MeshComponent::CreateBoxPrimitive(0.9f, 0.2f, 0.2f));
		//cube->world.Translate(-0.25f, -0.25f, 0.0f);
		//cube->world.Scale(0.05f);

		cubeBack = std::make_shared<GameObject>();
		cubeBack->AddComponent(MeshComponent::CreateBoxPrimitive(0.2f, 0.9f, 0.2f));
		//cubeBack->world.Translate(0.25f, 0.25f, 0.0f);
		//cubeBack->world.Scale(0.05f);
		
		gameObjectManager.push_back(cube);
		gameObjectManager.push_back(cubeBack);
	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);

		//cube->world.RotateZ(deltaTime * 25);

		//cubeBack->world.RotateY(deltaTime * -50);

		mat4 wp = cube->world * camera->projection;

		glUniformMatrix4fv(gViewUniform, 1, GL_FALSE, camera->view.GetMatrixFloatValues());
		glUniformMatrix4fv(gProjectionUniform, 1, GL_FALSE, camera->projection.GetMatrixFloatValues());
		glUniformMatrix4fv(gWP, 1, GL_FALSE, wp.GetMatrixFloatValues());
	}
};

