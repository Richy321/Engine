#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "../Core/CameraFPS.h"
using namespace Core;

class TestScene : public Managers::SceneManager
{
public:

	std::shared_ptr<CameraFPS> camera;
	GLuint gViewUniform;
	GLuint gProjectionUniform;
	GLuint gWP;
	std::shared_ptr<GameObject> cube;
	std::shared_ptr<GameObject> model;

	DirectionalLight* directionalLight;
	std::vector<SpotLight> spotLights;
	std::vector<PointLight> pointLights;

	TestScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo)
	{
		camera = std::make_shared<CameraFPS>();
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
		cube->AddComponent(AssetManager::GetInstance().CreateCubePrimitiveMeshComponent());
		cube->Translate(0.0f, 0.0f, 0.0f);

		model = std::make_shared<GameObject>();
		model->AddComponent(AssetManager::GetInstance().LoadMeshFromFile(std::string("Resources/Models/Dwarf/dwarf.x")));
		model->Translate(0.0f, 0.0f, -10.0f);

		gameObjectManager.push_back(model);
		gameObjectManager.push_back(cube);
	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);

		//cube->RotateY(deltaTime * 1.0f);
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