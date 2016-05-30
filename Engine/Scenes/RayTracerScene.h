#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "../Raytracer/RayTracer.h"
#include "../MaterialComponent.h"

using namespace Core;

class RayTracerScene : public Managers::SceneManager
{
public:
	std::unique_ptr<RayTracer> rayTracer;
	std::shared_ptr<CameraFPS> camera;
	const std::string defaultCheckeredTexture = "Default Checkered";
	const std::string earthTexture = "Earth";
	const std::string solidTexture = "Solid";

	float fov = 30.0f;
	uint imageWidth = 640;
	uint imageHeight = 480;

	//std::shared_ptr<PointLight> pl1;
	//std::shared_ptr<PointLight> pl2;

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
		//InitialiseSceneObjects();
		InitialiseDemoObjects();
		InitialiseCamera();
	}

	void InitialiseTextures() const
	{
		AssetManager::GetInstance().LoadTextureFromFile("Resources/checkered.jpg", defaultCheckeredTexture, GL_BGRA, GL_RGBA, 0, 0);
		AssetManager::GetInstance().LoadTextureFromFile("Resources/solid.jpg", solidTexture, GL_BGRA, GL_RGBA, 0, 0);
	}

	void InitialiseLights()
	{
		/*pl2 = std::make_shared<PointLight>();
		pl2->DiffuseIntensity = 10.75f;
		pl2->Color = vec3(1.0f, 0.5f, 0.0f);
		pl2->Position = vec3(3.0f, 1.0f, 5.0f);
		pl2->Attenuation.Linear = 0.1f;
		pl2->Position = vec3(10.0f, 10.0f, 10.0f);
		AddLight(pl2);*/

		/*directionalLight = std::make_shared<DirectionalLight>();
		directionalLight->Color = vec3(1.0f, 1.0f, 1.0f);
		directionalLight->AmbientIntensity = 0.01f;
		directionalLight->DiffuseIntensity = 0.9f;
		directionalLight->Direction = vec3(0.0f, -0.7, -1.0);*/
	}

	void InitialiseCamera()
	{
		camera->useWASD = true;
		camera->SetPerspectiveProjection(fov, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 50000.0f);
		SetMainCamera(camera);
	}

	std::shared_ptr<GameObject> CreateSphere(float radius, vec3 pos, bool reflective, float transparency, vec3 colour)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> meshComponent = AssetManager::GetInstance().CreateSpherePrimitiveMeshComponent(radius, 24, 24);
		meshComponent->rootMeshNode->meshes[0]->materialID = solidTexture;
		go->AddComponent(meshComponent);
		go->Translate(pos);

		std::shared_ptr<SphereColliderComponent> sphereCollider = std::make_shared<SphereColliderComponent>(go, radius);
		go->AddComponent(sphereCollider);

		std::shared_ptr<MaterialComponent> matComp = std::make_shared<MaterialComponent>(go);
		go->AddComponent(matComp);
		matComp->GetMaterial()->colour = colour;

		if(reflective)
			matComp->GetMaterial()->reflection = 1.0f;

		matComp->GetMaterial()->transparency = transparency;

		gameObjectManager.push_back(go);
		return go;
	}

	void InitialiseSceneObjects()
	{
		std::shared_ptr<PointLight>pl1 = std::make_shared<PointLight>();
		//pl1->DiffuseIntensity = 2.75f;
		pl1->DiffuseIntensity = 1.0f;
		pl1->Color = vec3(1.0f, 1.0f, 1.0f);
		pl1->Position = vec3(30.0f, 0.0f, -30.0f);
		pl1->Attenuation.Linear = 0.1f;
		AddLight(pl1);

		CreateSphere(1.5f, vec3(5.0f, 0.0f, -30.0f), true, 0.0f, Colours_RGB::Red);
		CreateSphere(3.5f, vec3(-5.0f, 0.0f, -30.0f), false, 0.0f, Colours_RGB::Red);

		rayTracer = std::make_unique<RayTracer>(fov, camera);
	}

	void InitialiseDemoObjects()
	{
		std::shared_ptr<PointLight> pl2 = std::make_shared<PointLight>();
		//pl1->DiffuseIntensity = 2.75f;
		pl2->DiffuseIntensity = 1.0f;
		pl2->Color = vec3(1.0f, 1.0f, 1.0f);
		pl2->Position = vec3(0.0f, 20.0f, -30.0f);
		pl2->Attenuation.Linear = 0.1f;
		AddLight(pl2);

		CreateSphere(10000, vec3(0.0, -10004, -20), false, 0.0f, vec3(0.20, 0.20, 0.20));
		CreateSphere(4, vec3(0.0, 0, -20), true, 0.5f, vec3(1.00, 0.32, 0.36));
		CreateSphere(2, vec3(5.0, -1, -15), true, 0.0f, vec3(0.90, 0.76, 0.46));
		CreateSphere(3, vec3(5.0, 0, -25), true, 0.0f, vec3(0.65, 0.77, 0.97));
		CreateSphere(3, vec3(-5.5, 0, -15), true, 0.0f, vec3(0.90, 0.90, 0.90));

		rayTracer = std::make_unique<RayTracer>(fov, camera);
	}

	void OnFixedTimeStep() override
	{
		SceneManager::OnFixedTimeStep();
	}

	void OnUpdate(float deltaTime) override
	{
		SceneManager::OnUpdate(deltaTime);
		camera->Update(deltaTime);
	}

	void OnFirstUpdate(float deltaTime) override
	{
		rayTracer->Render(gameObjectManager, GetLights(), windowInfo.width, windowInfo.height);
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);
		camera->OnKey(key, x, y);

		if(key == 'r' || key == 'R')
		{
			rayTracer->Render(gameObjectManager, GetLights(), windowInfo.width, windowInfo.height);
		}
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

	//std::vector<std::shared_ptr<Core::BaseLight>> GetLights()
	//{
	//	std::vector<std::shared_ptr<Core::BaseLight>> lights;

	//	if (directionalLight != nullptr)
	//		lights.push_back(directionalLight);

	//	for (auto i : spotLights)
	//		lights.push_back(i);

	//	for (auto i : pointLights)
	//		lights.push_back(i);

	//	return lights;
	//}
	
};

