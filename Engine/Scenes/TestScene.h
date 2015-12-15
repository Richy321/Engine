#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"
#include "../CameraFPS.h"
using namespace Core;

class TestScene : public Managers::SceneManager
{
public:

	std::shared_ptr<CameraFPS> camera;
	GLuint gViewUniform;
	GLuint gProjectionUniform;
	GLuint gWP;
	std::shared_ptr<GameObject> cube;
	std::shared_ptr<GameObject> cubeBack;

	TestScene(WindowInfo windowInfo) : SceneManager(windowInfo), camera(new CameraFPS())
	{
	}

	~TestScene()
	{
	}

	void Initialise() override
	{
		SceneManager::Initialise();
		CaptureCursor(true);

		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		SetMainCamera(camera);
		
		camera->Translate(0.0f, 0.0f, 20.0f);
		cube = std::make_shared<GameObject>();
		cube->AddComponent(MeshComponent::CreateCubePrimitive());
		cube->Translate(0.0f, 0.0f, 0.0f);
		
		cubeBack = std::make_shared<GameObject>();
		cubeBack->AddComponent(MeshComponent::CreateCubePrimitive());
		cubeBack->Translate(2.0f, 0.0f, -20.0f);
		cubeBack->Scale(5.5f);
		
		gameObjectManager.push_back(cubeBack);
		gameObjectManager.push_back(cube);
	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);

		cube->RotateY(deltaTime * 1.0f);
		cubeBack->RotateY(deltaTime * -1.1f);
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		camera->OnKey(key, x, y);
		
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		camera->OnMouseMove(deltaX, deltaY);
	}
};