#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "Physics/ParticleSystem/ParticleSystemComponent.h"
#include "../Physics/Cloth/ClothComponent.h"
using namespace Core;

class RayTracerScene : public Managers::SceneManager
{
public:

	std::shared_ptr<CameraFPS> camera;
	std::vector<std::shared_ptr<PointLight>> pointLights;
	const std::string defaultCheckeredTexture = "Default Checkered";
	const std::string earthTexture = "Earth";
	const std::string solidTexture = "Solid";

	std::shared_ptr<GameObject> sphere;

	RayTracerScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo)
	{
		camera = std::make_shared<CameraFPS>();
	}

	~RayTracerScene()
	{
	}

	void Initialise() override
	{
		SceneManager::Initialise();

		InitialiseLights();
		InitialiseTextures();
		InitialiseSceneObjects();
		InitialiseCamera();
		//clearColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void InitialiseTextures() const
	{
		AssetManager::GetInstance().LoadTextureFromFile("Resources/checkered.jpg", defaultCheckeredTexture, GL_BGRA, GL_RGBA, 0, 0);
		AssetManager::GetInstance().LoadTextureFromFile("Resources/solid.jpg", solidTexture, GL_BGRA, GL_RGBA, 0, 0);
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
		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 50000.0f);
		SetMainCamera(camera);
		camera->Translate(0.0f, 0.0f, 10);
	}

	std::shared_ptr<GameObject> CreateSphere(float radius, vec3 pos)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> meshComponent = AssetManager::GetInstance().CreateSpherePrimitiveMeshComponent(radius, 24, 24);
		meshComponent->rootMeshNode->meshes[0]->materialID = solidTexture;
		go->AddComponent(meshComponent);
		go->Translate(pos);
		gameObjectManager.push_back(go);
		return go;
	}

	void InitialiseSceneObjects()
	{
		sphere = CreateSphere(1.5f, vec3(0.0f, 0.0f, 0.0f));
	}

	void OnFixedTimeStep() override
	{
		SceneManager::OnFixedTimeStep();
	}

	void OnUpdate(float deltaTime) override
	{
		camera->Update(deltaTime);
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);
		camera->OnKey(key, x, y);
	}

	void notifyProcessMouseState(int button, int state, int x, int y) override
	{
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
	}

	virtual void notifyDisplayFrame() override
	{
		SceneManager::notifyDisplayFrame();
	}
};

