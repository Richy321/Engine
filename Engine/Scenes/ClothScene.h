#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
using namespace Core;

class ClothScene : public Managers::SceneManager
{
public:

	std::shared_ptr<CameraFPS> camera;
	std::vector<std::shared_ptr<PointLight>> pointLights;
	const std::string defaultCheckeredTexture = "Default Checkered";


	ClothScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo)
	{
		camera = std::make_shared<CameraFPS>();
	}

	~ClothScene()
	{
	}

	void Initialise() override
	{
		SceneManager::Initialise();

		InitialiseLights();
		InitialiseTextures();
		InitialiseCamera();
		InitialiseSceneObjects();
	}

	void InitialiseTextures() const
	{
		AssetManager::GetInstance().LoadTextureFromFile("Resources/checkered.jpg", defaultCheckeredTexture, GL_BGRA, GL_RGBA, 0, 0);
	}

	void InitialiseLights()
	{
		pointLights.push_back(std::make_shared<PointLight>());
		pointLights[0]->DiffuseIntensity = 0.75f;
		pointLights[0]->Color = vec3(1.0f, 0.5f, 0.0f);
		pointLights[0]->Position = vec3(3.0f, 1.0f, 5.0f);
		pointLights[0]->Attenuation.Linear = 0.1f;

		pointLights.push_back(std::make_shared<PointLight>());
		pointLights[1]->DiffuseIntensity = 1.75f;
		pointLights[1]->Color = vec3(0.0f, 0.5f, 1.0f);
		pointLights[1]->Position = vec3(0.0f, 0.0f, 10.0f);
		pointLights[1]->Attenuation.Linear = 0.1f;
	}

	void InitialiseCamera()
	{
		camera->useWASD = true;
		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		SetMainCamera(camera);
		camera->Translate(0.0f, 0.0f, 50.0f);
	}

	void InitialiseSceneObjects()
	{
		//std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		//std::shared_ptr<MeshComponent> meshComponent = AssetManager::GetInstance().CreateSpherePrimitiveMeshComponent(10, 5, 5);
		//meshComponent->rootMeshNode->meshes[0]->renderWireframe = true;
		//go->AddComponent(meshComponent);
		//gameObjectManager.push_back(go);

		std::shared_ptr<GameObject> go2 = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> meshComponent2 = AssetManager::GetInstance().CreateIcospherePrimitiveMeshComponent(1, 5.0f);
		meshComponent2->rootMeshNode->meshes[0]->renderWireframe = true;
		go2->AddComponent(meshComponent2);
		go2->Translate(11.0f, 0.0f, 0.0f);
		gameObjectManager.push_back(go2);
	}

	void OnFixedTimeStep() override
	{

	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);

	}

	void OnCommsUpdate(float deltaTime) const
	{
		//Not used unless threaded network comms is turned off
		networking::ClientNetworkManager::GetInstance()->UpdateComms();
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);
		camera->OnKey(key, x, y);
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		camera->OnMouseMove(deltaX, deltaY);
	}

};

