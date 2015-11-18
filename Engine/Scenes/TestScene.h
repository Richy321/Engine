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

		//assert(gViewUniform != 0xFFFFFFFF);
		//assert(gProjectionUniform != 0xFFFFFFFF);

		camera->SetPerspectiveProjection(30.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 0.1f, 1000.0f);
		SetMainCamera(camera);
		
		cube = std::make_shared<GameObject>();
		cube->AddComponent(MeshComponent::CreateCubePrimitive());
		//cube->world.Translate(-0.25f, -0.25f, 0.0f);
		cube->world.Scale(0.05f);

		cubeBack = std::make_shared<GameObject>();
		cubeBack->AddComponent(MeshComponent::CreateCubePrimitive());
		cubeBack->world.Translate(0.0f, 0.25f, 1.0f);
		cubeBack->world.Scale(0.05f);
		
		
		gameObjectManager.push_back(cubeBack);
		gameObjectManager.push_back(cube);
	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);

		cube->world.RotateY(deltaTime * 75);

		cubeBack->world.RotateY(deltaTime * -75);

		mat4 wp = cube->world * camera->projection;

		//glUniformMatrix4fv(gViewUniform, 1, GL_FALSE, camera->view.GetMatrixFloatValues());
		//glUniformMatrix4fv(gProjectionUniform, 1, GL_FALSE, camera->projection.GetMatrixFloatValues());
		//glUniformMatrix4fv(gWP, 1, GL_FALSE, wp.GetMatrixFloatValues());
	}
};

