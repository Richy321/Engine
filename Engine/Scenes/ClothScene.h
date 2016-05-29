#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "Physics/ParticleSystem/ParticleSystemComponent.h"
#include "../Physics/Cloth/ClothComponent.h"
using namespace Core;

class ClothScene : public Managers::SceneManager
{
public:

	std::shared_ptr<CameraFPS> camera;
	std::vector<std::shared_ptr<PointLight>> pointLights;
	const std::string defaultCheckeredTexture = "Default Checkered";
	const std::string earthTexture = "Earth";
	const std::string solidTexture = "Solid";

	std::shared_ptr<GameObject> cloth;
	const float clothWidth = 7.0f;
	const float clothDepth = 7.0f;

	std::shared_ptr<ClothComponent> clothComponent;

	std::vector<std::shared_ptr<ICollider>> sphereColliders;
	bool isLMBDown = false;

	std::shared_ptr<GameObject> sphere;

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
		InitialiseSceneObjects();
		InitialiseCamera();
		//clearColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void InitialiseTextures() const
	{
		AssetManager::GetInstance().LoadTextureFromFile("Resources/checkered.jpg", defaultCheckeredTexture, GL_BGRA, GL_RGBA, 0, 0);
		AssetManager::GetInstance().LoadTextureFromFile("Resources/earth.jpg", earthTexture, GL_BGRA, GL_RGBA, 0, 0);
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
		//camera->RotateX(-90.0f);
		camera->Translate(0.0f, 0.0f, 10);
	}

	std::shared_ptr<GameObject> CreateSphere(float radius, vec3 pos)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> meshComponent = AssetManager::GetInstance().CreateSpherePrimitiveMeshComponent(radius, 24, 24);
		meshComponent->rootMeshNode->meshes[0]->materialID = solidTexture;
		go->AddComponent(meshComponent);
		std::shared_ptr<SphereColliderComponent> sphereCollider = std::make_shared<SphereColliderComponent>(go, radius);
		go->AddComponent(sphereCollider);
		go->Translate(pos);
		gameObjectManager.push_back(go);
		sphereColliders.push_back(sphereCollider);
		return go;
	}

	void InitialiseSceneObjects()
	{
		cloth = std::make_shared<GameObject>();
		clothComponent = std::make_shared<ClothComponent>(std::weak_ptr<GameObject>(),vec2(clothWidth, clothDepth), vec2(
			35, 35));
		clothComponent->SetTexture(defaultCheckeredTexture);
		cloth->AddComponent(clothComponent);
		cloth->Translate(-clothWidth *0.5f, 3.0f, -clothDepth * 0.5f);
		gameObjectManager.push_back(cloth);

		sphere = CreateSphere(1.5f, vec3(0.0f, 0.0f, 0.0f));

		std::shared_ptr<SphereColliderComponent> sphereCollider = std::dynamic_pointer_cast<SphereColliderComponent>(sphereColliders[0]);
		clothComponent->sphere = sphereCollider;
	}

	void OnFixedTimeStep() override
	{
		SceneManager::OnFixedTimeStep();
	}

	void OnUpdate(float deltaTime) override
	{
		//clothComponent->AddForce(vec3(0.0f, -9.8f, 0.0f)); //gravity
		//clothComponent->AddWindForce(vec3(0.5f, 0.0f, 0.2f));

		HandleCollisions();
		camera->Update(deltaTime);

		if(isLMBDown)
		{
			POINT mouse;
			GetCursorPos(&mouse);
			HWND hWnd = WindowFromDC(wglGetCurrentDC());
			ScreenToClient(hWnd, &mouse);
			printf("mouse: %i,%i\n", mouse.x, mouse.y);
			vec2 pos;

			pos = UnprojectGLM(mouse.x, mouse.y);
			sphere->SetPosition2D(pos);
		}
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);
		camera->OnKey(key, x, y);
	}

	void notifyProcessMouseState(int button, int state, int x, int y) override
	{
		isLMBDown = button == GLUT_LEFT_BUTTON;
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		//camera->OnMouseMove(deltaX, deltaY);
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

