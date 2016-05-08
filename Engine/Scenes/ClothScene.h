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

	std::shared_ptr<GameObject> cloth;
	float m_scale = 0.0f;
	const float clothWidth = 30.0f;
	const float clothDepth = 30.0f;

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
		directionalLight = std::make_shared<DirectionalLight>();
		directionalLight->Color = vec3(1.0f, 1.0f, 1.0f);
		directionalLight->AmbientIntensity = 0.01f;
		directionalLight->DiffuseIntensity = 0.5f;
		directionalLight->Direction = vec3(0.0f, -1.0, -1.0);

		/*
		pointLights.push_back(std::make_shared<PointLight>());
		pointLights[0]->DiffuseIntensity = 0.75f;
		pointLights[0]->Color = vec3(1.0f, 0.5f, 0.0f);
		pointLights[0]->Position = vec3(3.0f, 1.0f, 5.0f);
		pointLights[0]->Attenuation.Linear = 0.1f;

		pointLights.push_back(std::make_shared<PointLight>());
		pointLights[1]->DiffuseIntensity = 0.75f;
		pointLights[1]->Color = vec3(0.0f, 0.5f, 1.0f);
		pointLights[1]->Position = vec3(7.0f, 1.0f, 1.0f);
		pointLights[1]->Attenuation.Linear = 0.1f;

		spotLights.push_back(std::make_shared<SpotLight>());
		spotLights[0]->DiffuseIntensity = 0.9f;
		spotLights[0]->Color = vec3(1.0f, 0.0f, 0.0f);
		spotLights[0]->Position = vec3(clothWidth * 0.5f, 5.0f, clothDepth * 0.5f);
		spotLights[0]->Direction = vec3(0.0, -1.0f, 0.0f);
		spotLights[0]->Attenuation.Linear = 0.1f;
		spotLights[0]->Cutoff = 20.0f;*/
	}

	void InitialiseCamera()
	{
		camera->useWASD = true;
		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		SetMainCamera(camera);
		camera->Translate(0.0f, 23.0f, clothDepth * 1.7f);
	}

	void InitialiseSceneObjects()
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> meshComponent = AssetManager::GetInstance().CreateSpherePrimitiveMeshComponent(5, 24, 24);
		//meshComponent->rootMeshNode->meshes[0]->renderWireframe = true;
		go->AddComponent(meshComponent);
		go->Translate(0.0f, 10.0f, 0.0f);
		gameObjectManager.push_back(go);

		cloth = std::make_shared<GameObject>();
		cloth->AddComponent(AssetManager::GetInstance().CreateQuadPrimitiveMeshComponent(clothWidth, clothDepth, defaultCheckeredTexture));
		cloth->Translate(-clothWidth *0.5f, 20.0f, -clothDepth * 0.5f);
		gameObjectManager.push_back(cloth);

		//std::shared_ptr<GameObject> go2 = std::make_shared<GameObject>();
		//std::shared_ptr<MeshComponent> meshComponent2 = AssetManager::GetInstance().CreateIcospherePrimitiveMeshComponent(1, 5.0f);
		////meshComponent2->rootMeshNode->meshes[0]->renderWireframe = true;
		//go2->AddComponent(meshComponent2);
		//go2->Translate(0.0f, 0.0f, 0.0f);
		//gameObjectManager.push_back(go2);
	}

	void OnFixedTimeStep() override
	{

	}

	void OnUpdate(float deltaTime) override
	{
		m_scale += 0.0057f;
		camera->Update(deltaTime);
		//pointLights[0]->Position = vec3(clothWidth * 0.25f, 1.0f, clothDepth * (cosf(m_scale) + 1.0f) / 2.0f);
		//pointLights[1]->Position = vec3(clothWidth * 0.75f, 1.0f, clothDepth * (sinf(m_scale) + 1.0f) / 2.0f);
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

	virtual void notifyDisplayFrame() override
	{
		SceneManager::notifyDisplayFrame();
	}

};

