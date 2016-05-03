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
		pointLights[1]->DiffuseIntensity = 0.75f;
		pointLights[1]->Color = vec3(0.0f, 0.5f, 1.0f);
		pointLights[1]->Position = vec3(7.0f, 1.0f, 1.0f);
		pointLights[1]->Attenuation.Linear = 0.1f;
	}

	void InitialiseCamera()
	{
		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		SetMainCamera(camera);
		//camera->Translate(floorWidth * 0.5f, 3.0f, floorDepth * 1.3f);
	}

	void OnFixedTimeStep() override
	{

	}

};

