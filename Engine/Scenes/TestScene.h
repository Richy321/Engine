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

		camera->SetPerspectiveProjection(30.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		//camera->SetOrthographicProjection(static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), -1, 1);
		SetMainCamera(camera);
		camera->GetWorldTransform().Translate(2.0f, 0.0f, 20.0f);

		cube = std::make_shared<GameObject>();
		cube->AddComponent(MeshComponent::CreateCubePrimitive());
		//cube->world.Translate(-0.25f, -0.25f, 0.0f);
		//cube->world.Translate(0.0f, 0.0f, -0.90f);
		cube->world.Scale(0.5f);

		cubeBack = std::make_shared<GameObject>();
		cubeBack->AddComponent(MeshComponent::CreateCubePrimitive());
		cubeBack->world.Translate(0.0f, 0.0f, -10.0f);
		cubeBack->world.Scale(1.5f);
		
		gameObjectManager.push_back(cubeBack);
		gameObjectManager.push_back(cube);
	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);

		cube->world.RotateY(deltaTime * 75);
		cubeBack->world.RotateY(deltaTime * -75);
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		mat4 camTransform;
		const float cameraMovementSpeed = 0.1f;

		if(key == 'a')
			mainCamera.lock()->GetWorldTransform().Translate(-cameraMovementSpeed, 0.0f, 0.0f);
		if(key == 'd')
			mainCamera.lock()->GetWorldTransform().Translate(cameraMovementSpeed, 0.0f, 0.0f);
		
		if (key == 'w')
			mainCamera.lock()->GetWorldTransform().Translate(0.0f, 0.0f, -cameraMovementSpeed);
		if (key == 's')
			mainCamera.lock()->GetWorldTransform().Translate(0.0f, 0.0f, cameraMovementSpeed);

		if (key == 'q')
			mainCamera.lock()->GetWorldTransform().Translate(0.0f, cameraMovementSpeed, 0.0f);
		if (key == 'e')
			mainCamera.lock()->GetWorldTransform().Translate(0.0f, -cameraMovementSpeed, 0.0f);
	}
};

