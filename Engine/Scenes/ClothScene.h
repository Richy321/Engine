#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
//#include "Physics/Cloth/ClothComponent.h"
#include "Physics/ParticleSystem/ParticleSystemComponent.h"
#include "../Physics/Cloth/ClothComponent.h"
using namespace Core;

class ClothScene : public Managers::SceneManager
{
public:

	std::shared_ptr<CameraFPS> camera;
	std::vector<std::shared_ptr<PointLight>> pointLights;
	const std::string defaultCheckeredTexture = "Default Checkered";

	std::shared_ptr<GameObject> cloth;
	float m_scale = 0.0f;
	const float clothWidth = 100.0f;
	const float clothDepth = 100.0f;

	std::shared_ptr<ClothComponent> clothComponent;

	std::vector<std::shared_ptr<ICollider>> sphereColliders;


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

		//clearColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
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
		directionalLight->DiffuseIntensity = 0.9f;
		directionalLight->Direction = vec3(0.0f, -0.7, -1.0);
	}

	void InitialiseCamera()
	{
		camera->useWASD = true;
		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 0.01f, 1000.0f);
		SetMainCamera(camera);
		//camera->RotateX(-90.0f);
		camera->Translate(0.0f, 23.0f, 50 * 1.7f);
	}

	void CreateSphere(float radius, vec3 pos)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> meshComponent = AssetManager::GetInstance().CreateSpherePrimitiveMeshComponent(radius, 24, 24);
		//meshComponent->rootMeshNode->meshes[0]->renderWireframe = true;
		go->AddComponent(meshComponent);
		std::shared_ptr<SphereColliderComponent> sphereCollider = std::make_shared<SphereColliderComponent>(go, radius);
		go->AddComponent(sphereCollider);
		go->Translate(pos);
		gameObjectManager.push_back(go);
		sphereColliders.push_back(sphereCollider);
	}

	void InitialiseSceneObjects()
	{
		cloth = std::make_shared<GameObject>();
		clothComponent = std::make_shared<ClothComponent>(std::weak_ptr<GameObject>(),vec2(clothWidth, clothDepth), vec2(
			30, 30));
		clothComponent->SetTexture(defaultCheckeredTexture);
		cloth->AddComponent(clothComponent);
		cloth->Translate(-clothWidth *0.5f, 50.0f, -clothDepth * 0.5f);
		gameObjectManager.push_back(cloth);

		CreateSphere(15.0f, vec3(0.0f, 0.0f, 0.0f));
	}

	void OnFixedTimeStep() override
	{
		SceneManager::OnFixedTimeStep();
	}

	void OnUpdate(float deltaTime) override
	{
		clothComponent->AddForce(vec3(0.0f, -9.8f, 0.0f)); //gravity
		//clothComponent->AddWindForce(vec3(0.5f, 0.0f, 0.2f));
		HandleCollisions();
		m_scale += 0.0057f;
		camera->Update(deltaTime);
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);
		camera->OnKey(key, x, y);

		if (key == 'r')
			clothComponent->Reset();
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		camera->OnMouseMove(deltaX, deltaY);
	}

	virtual void notifyDisplayFrame() override
	{
		SceneManager::notifyDisplayFrame();
	}

	void HandleCollisions()
	{
		clothComponent->HandleCollisions(sphereColliders);
	}
};

