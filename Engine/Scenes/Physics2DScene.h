#pragma once
#include "../Managers/SceneManager.h"
#include "../Physics/RigidBody2DComponent.h"
#include "../Core/Components/SphereColliderComponent.h"
#include "../Physics/PhysicsManager.h"
#include "../Core/Components/BoxColliderComponent.h"

using namespace Core;

class Physics2DScene : public Managers::SceneManager
{
public:
	std::shared_ptr<Camera2D> camera2D;
	std::shared_ptr<CameraFPS> camera;

	std::shared_ptr<DirectionalLight> directionalLight;
	std::vector<std::shared_ptr<SpotLight>> spotLights;
	std::vector<std::shared_ptr<PointLight>> pointLights;


	std::vector<std::shared_ptr<GameObject>> physicsObjects;

	bool isUseCamera3D = false;

	const float floorWidth = 30.0f;
	const float floorDepth = 30.0f;

	const std::string defaultCheckeredTexture = "Default Checkered";

	float m_scale = 0.0f;


	Physics2DScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo)
	{
		if(isUseCamera3D)
			camera = std::make_shared<CameraFPS>();
		else
			camera2D = std::make_shared<Camera2D>();
	}

	~Physics2DScene()
	{
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
		pointLights[1]->Position = vec3(7.0f, 1.0f, 1.0f);
		pointLights[1]->Attenuation.Linear = 0.1f;

		spotLights.push_back(std::make_shared<SpotLight>());
		spotLights[0]->DiffuseIntensity = 0.9f;
		spotLights[0]->Color = vec3(1.0f, 0.0f, 0.0f);
		spotLights[0]->Position = vec3(floorWidth * 0.5f, 5.0f, floorDepth * 0.5f);
		spotLights[0]->Direction = vec3(0.0, -1.0f, 0.0f);
		spotLights[0]->Attenuation.Linear = 0.1f;
		spotLights[0]->Cutoff = 20.0f;
	}

	void InitialiseCamera()
	{
		if (isUseCamera3D)
		{
			camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
			SetMainCamera(camera);
			camera->Translate(floorWidth * 0.5f, 3.0f, floorDepth * 1.3f);
		}
		else
		{
			camera2D->SetOrthographicProjection(-static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.width), -static_cast<float>(windowInfo.height), static_cast<float>(windowInfo.height), 5, 1000);
			camera2D->SetZoom(0.03f);
			SetMainCamera(camera2D);
			camera2D->Translate(floorWidth * 0.5f, 0.0f, floorDepth * 1.3f);
		}
	}

	void Initialise() override
	{
		SceneManager::Initialise();

		InitialiseLights();
		InitialiseTextures();
		InitialiseCamera();

		std::shared_ptr<GameObject> floor2 = std::make_shared<GameObject>();
		floor2->AddComponent(AssetManager::GetInstance().CreateQuadPrimitiveMeshComponent(floorWidth, floorDepth, defaultCheckeredTexture));
		floor2->Translate(0, floorWidth*0.5f, -floorWidth*0.5f);
		floor2->RotateX(glm::radians(90.0f));
		gameObjectManager.push_back(floor2);

		std::shared_ptr<GameObject> square = std::make_shared<GameObject>();
		square->AddComponent(AssetManager::GetInstance().CreateSimpleQuadPrimitiveMeshComponent(5.0f, 5.0f));
		gameObjectManager.push_back(square);

		std::shared_ptr<GameObject> square2 = std::make_shared<GameObject>();
		square2->AddComponent(AssetManager::GetInstance().CreateSimpleQuadPrimitiveMeshComponent(5.0f, 5.0f));
		square2->Translate(10.0f, 0.0f, 0.0f);
		gameObjectManager.push_back(square2);

		std::shared_ptr<GameObject> circle = CreateCirclePhysicsObject();
		circle->Translate(20.0f, 0.0f, 0.0f);
		gameObjectManager.push_back(circle);

		for (int i = 0; i < 1; i++)
		{
			//std::shared_ptr<GameObject> box = std::make_shared<GameObject>();
			//box->AddComponent(AssetManager::GetInstance().CreateQuadPrimitiveMeshComponent(100,100, defaultCheckeredTexture));
			//gameObjectManager.push_back(box);
			//physicsObjects.push_back(box);

			//std::shared_ptr<GameObject> sphere = std::make_shared<GameObject>();
			//sphere->AddComponent(AssetManager::GetInstance().CreateCirclePrimitiveMeshComponent(1.0f, 8));
			//gameObjectManager.push_back(sphere);
			//physicsObjects.push_back(sphere);
		}
	}

	void OnUpdate(float deltaTime) override
	{
		if (isUseCamera3D)
			camera->Update(deltaTime);
		else
			camera2D->Update(deltaTime);
		m_scale += 0.0057f;
		pointLights[0]->Position = vec3(floorWidth * 0.25f, 1.0f, floorDepth * (cosf(m_scale) + 1.0f) / 2.0f);
		pointLights[1]->Position = vec3(floorWidth * 0.75f, 1.0f, floorDepth * (sinf(m_scale) + 1.0f) / 2.0f);

	}

	virtual void notifyDisplayFrame() override
	{
		Check_GLError();
		vec3 cameraPos;
		if (isUseCamera3D)
		{
			//Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetViewMatrix(camera->view);
			//Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetProjectionMatrix(camera->projection);
			cameraPos = vec3(camera->GetWorldTransform()[3]);
		}
		else
		{
			//Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetViewMatrix(camera2D->view);
			//Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetProjectionMatrix(camera2D->projection);
			cameraPos = vec3(camera2D->GetWorldTransform()[3]);

			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->Enable();
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetDirectionalLight(directionalLight);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetPointLights(pointLights);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetSpotLights(spotLights);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularIntensity(1.0f);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularPower(32.0f);
		}
		
		Check_GLError();
		Check_GLError();
		SceneManager::notifyDisplayFrame();
	}


	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);

		if(isUseCamera3D)
			camera->OnKey(key, x, y);
		else
			camera2D->OnKey(key, x, y);
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		if (isUseCamera3D)
		{
			camera->OnMouseMove(deltaX, deltaY);
		}
		//else
		//	camera2D->OnMouseMove(deltaX, deltaY);
	}

	std::shared_ptr<GameObject> CreateCirclePhysicsObject()
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->AddComponent(AssetManager::GetInstance().CreateCirclePrimitiveMeshComponent(2.5, 32));
		
		std::shared_ptr<RigidBody2DComponent> rigidBodyComponent = std::make_shared<RigidBody2DComponent>(go);
		go->AddComponent(rigidBodyComponent);

		std::shared_ptr<SphereColliderComponent> sphereColliderComponent = std::make_shared<SphereColliderComponent>(go, 2.5f, vec3(0.0f, 0.0f, 0.0f));
		go->AddComponent(sphereColliderComponent);

		PhysicsManager::ComputeSphereMass(rigidBodyComponent, sphereColliderComponent);

		return go;
	}

	std::shared_ptr<GameObject> CreatePolygonPhysicsObject()
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> polygonMesh = AssetManager::GetInstance().CreateRandomPolygonPrimitiveMeshComponent();
		go->AddComponent(polygonMesh);

		std::shared_ptr<RigidBody2DComponent> rigidBodyComponent = std::make_shared<RigidBody2DComponent>(go);
		go->AddComponent(rigidBodyComponent);

		std::shared_ptr<BoxColliderComponent> boxColliderComponent = std::make_shared<BoxColliderComponent>(go, polygonMesh->rootMeshNode->meshes[0]->ComputeAABB());
		go->AddComponent(boxColliderComponent);

		PhysicsManager::ComputePolygonMass(rigidBodyComponent, polygonMesh->rootMeshNode->meshes[0]->vertices);

		return go;
	}


};

