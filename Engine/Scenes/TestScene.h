#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/ShaderEffects/LitTexturedMeshEffect.h"
using namespace Core;

class TestScene : public Managers::SceneManager
{
public:

	std::shared_ptr<CameraFPS> camera;
	GLuint gViewUniform;
	GLuint gProjectionUniform;
	GLuint gWP;
	//std::shared_ptr<GameObject> cube;
	std::shared_ptr<GameObject> model;

	std::shared_ptr<DirectionalLight> directionalLight;
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

		directionalLight = std::make_shared<DirectionalLight>();
		directionalLight->Color = vec3(1.0f, 1.0f, 1.0f);
		directionalLight->AmbientIntensity = 0.01f;
		directionalLight->DiffuseIntensity = 0.75f;
		directionalLight->Direction = vec3(0.0f, 0.0, -1.0);

		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		SetMainCamera(camera);
		camera->Translate(0.0f, 0.0f, 20.0f);

		//cube = std::make_shared<GameObject>();
		//cube->AddComponent(AssetManager::GetInstance().CreateCubePrimitiveMeshComponent());
		//cube->Translate(0.0f, 0.0f, 0.0f);

		model = std::make_shared<GameObject>();
		model->AddComponent(AssetManager::GetInstance().LoadMeshFromFile(std::string("Resources/Models/Dwarf/dwarf.x")));
		model->Translate(0.0f, 0.0f, -10.0f);

		gameObjectManager.push_back(model);
		//gameObjectManager.push_back(cube);
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

	virtual void notifyDisplayFrame() override
	{
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetDirectionalLight(directionalLight);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetViewMatrix(camera->view);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetProjectionMatrix(camera->projection);

		vec3 cameraPos = vec3(camera->GetWorldTransform()[3]);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetEyeWorldPos(cameraPos);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularIntensity(1.0f);
		Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularPower(32.0f);

		SceneManager::notifyDisplayFrame();
	}
};