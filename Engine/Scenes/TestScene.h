#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/ShaderEffects/LitTexturedMeshEffect.h"
#include "../Core/Camera2D.h"
using namespace Core;

class TestScene : public Managers::SceneManager
{
public:

	std::shared_ptr<Camera2D> camera;
	std::shared_ptr<GameObject> cube;
	std::shared_ptr<GameObject> model;
	
	std::shared_ptr<GameObject> sphere;

	std::shared_ptr<GameObject> particles;

	//std::shared_ptr<DirectionalLight> directionalLight;
	//std::vector<std::shared_ptr<SpotLight>> spotLights;
	//std::vector<std::shared_ptr<PointLight>> pointLights;

	const float floorWidth = 30.0f;
	const float floorDepth = 30.0f;
	float m_scale = 0.0f;

	std::vector<std::shared_ptr<GameObject>> otherPlayers;
	std::vector<std::shared_ptr<GameObject>> physicsObjects;

	const std::string defaultCheckeredTexture = "Default Checkered";
	TestScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo)
	{
		camera = std::make_shared<Camera2D>();
	}

	~TestScene()
	{
	}

	void Initialise() override
	{
		SceneManager::Initialise();
		//CaptureCursor(true);
	
		//InitialiseTextures();
		//InitialiseLights();

		//camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		//SetMainCamera(camera);
		camera->SetOrthographicProjection(-1.0, 1.0, -1.0, 1.0, 5, 100);
		SetMainCamera(camera);
		//camera->Translate(floorWidth * 0.5f, 3.0f, floorDepth * 1.3f);

		//cube = std::make_shared<GameObject>();
		//cube->AddComponent(AssetManager::GetInstance().CreateCubePrimitiveMeshComponent());
		//cube->Scale(2.0f);
		//cube->Translate(0.0f, 0.0f, 0.0f);
		//gameObjectManager.push_back(cube);

		//for (int i = 0; i < 1; i++)
		//{
			std::shared_ptr<GameObject> box = std::make_shared<GameObject>();
			std::shared_ptr<MeshComponent> boxComp = AssetManager::GetInstance().CreateQuadPrimitiveMeshComponent(floorWidth, floorDepth);
			box->AddComponent(boxComp);
			gameObjectManager.push_back(box);
			//physicsObjects.push_back(box);

		//	std::shared_ptr<GameObject> sphere = std::make_shared<GameObject>();
		//	sphere->AddComponent(AssetManager::GetInstance().CreateCirclePrimitiveMeshComponent(1.0f, 8));
		//	gameObjectManager.push_back(sphere);
		//	physicsObjects.push_back(sphere);
		//}
		//model = InitialisePlayer();

		//InitialiseParticles(particles);

		//sphere = std::make_shared<GameObject>();
		//sphere->AddComponent(AssetManager::GetInstance().CreateIcospherePrimitiveMeshComponent());
		//sphere->Scale(2.0f);
		//sphere->Translate(5.0f, 0.0f, 5.0f);
		//gameObjectManager.push_back(sphere);
		//physicsObjects.push_back(sphere);
		std::shared_ptr<GameObject> floor = std::make_shared<GameObject>();
		floor->AddComponent(AssetManager::GetInstance().CreateQuadPrimitiveMeshComponent(floorWidth, floorDepth, defaultCheckeredTexture));
		gameObjectManager.push_back(floor);
		physicsObjects.push_back(floor);
	}

	//void InitialiseParticles(std::shared_ptr<GameObject> particles)
	//{
	//	particles = std::make_shared<GameObject>();
	//	particles->AddComponent(AssetManager::GetInstance().CreateIcospherePrimitiveMeshComponent(0));
	//	//particles->Scale(2.0f);
	//	particles->Translate(floorWidth / 2.0f, 0.0f, floorDepth / 2.0f);
	//	gameObjectManager.push_back(particles);
	//}

	//std::shared_ptr<GameObject> InitialisePlayer()
	//{
	//	model = std::make_shared<GameObject>();
	//	model->AddComponent(AssetManager::GetInstance().LoadMeshFromFile(std::string("Resources/Models/Dwarf/dwarf.x")));
	//	model->Translate(floorWidth / 2.0f, 0.0f, floorDepth / 2.0f);
	//	model->Scale(0.05f);
	//	gameObjectManager.push_back(model);
	//	return model;
	//}

	/*void InitialiseTextures()
	{
		AssetManager::GetInstance().LoadTextureFromFile("Resources/checkered.jpg", defaultCheckeredTexture, GL_BGRA, GL_RGBA, 0, 0);
	}*/

	/*void InitialiseLights()
	{
		directionalLight = std::make_shared<DirectionalLight>();
		directionalLight->Color = vec3(1.0f, 1.0f, 1.0f);
		directionalLight->AmbientIntensity = 0.01f;
		directionalLight->DiffuseIntensity = 0.2f;
		directionalLight->Direction = vec3(0.0f, 0.0, -1.0);

		pointLights.push_back(std::make_shared<PointLight>());
		pointLights[0]->DiffuseIntensity = 0.75f;
		pointLights[0]->Color = vec3(1.0f, 0.5f, 0.0f);
		pointLights[0]->Position = vec3(3.0f, 1.0f, 5.0f);
		pointLights[0]->Attenuation.Linear = 0.1f;

		pointLights.push_back(std::make_shared<PointLight>());
		pointLights[1]->DiffuseIntensity = 0.75f;
		pointLights[1]->Color = vec3(0.0f, 0.5f, 1.0f);
		pointLights[0]->Position = vec3(7.0f, 1.0f, 1.0f);
		pointLights[1]->Attenuation.Linear = 0.1f;

		spotLights.push_back(std::make_shared<SpotLight>());
		spotLights[0]->DiffuseIntensity = 0.9f;
		spotLights[0]->Color = vec3(1.0f, 0.0f, 0.0f);
		spotLights[0]->Position = vec3(floorWidth * 0.5f, 5.0f, floorDepth * 0.5f);
		spotLights[0]->Direction = vec3(0.0, -1.0f, 0.0f);
		spotLights[0]->Attenuation.Linear = 0.1f;
		spotLights[0]->Cutoff = 20.0f;
	}*/

	void OnUpdate(float deltaTime) override
	{
		//m_scale += 0.0057f;
		camera->Update(deltaTime);
		//pointLights[0]->Position = vec3(floorWidth * 0.25f, 1.0f, floorDepth * (cosf(m_scale) + 1.0f) / 2.0f);
		//pointLights[1]->Position = vec3(floorWidth * 0.75f, 1.0f, floorDepth * (sinf(m_scale) + 1.0f) / 2.0f);

		//cube->RotateY(deltaTime * 1.0f);
	}

	/*void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);
		camera->OnKey(key, x, y);	
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		camera->OnMouseMove(deltaX, deltaY);
	}*/

	/*
	virtual void notifyDisplayFrame() override
	{
		Check_GLError();
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->Enable();
		vec3 cameraPos = vec3(camera->GetWorldTransform()[3]);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetEyeWorldPos(cameraPos);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularIntensity(1.0f);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularPower(32.0f);

		//Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetDirectionalLight(directionalLight);
		//Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetPointLights(pointLights);
		//Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetSpotLights(spotLights);
		Check_GLError();
		SceneManager::notifyDisplayFrame();
		Check_GLError();
	}*/
};