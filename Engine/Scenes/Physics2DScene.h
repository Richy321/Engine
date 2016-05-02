#pragma once
#include "../Managers/SceneManager.h"
#include "../Physics/RigidBody2DComponent.h"
#include "../Core/Components/SphereColliderComponent.h"
#include "../Physics/PhysicsManager.h"
#include "../Core/Components/BoxColliderComponent.h"
#include "../Physics/Manifold.h"
#include "../Physics/PhysicsMaterials.h"

using namespace Core;

class Physics2DScene : public Managers::SceneManager
{
public:
	std::shared_ptr<Camera2D> camera2D;
	std::shared_ptr<CameraFPS> camera;

	std::shared_ptr<DirectionalLight> directionalLight;
	std::vector<std::shared_ptr<SpotLight>> spotLights;
	std::vector<std::shared_ptr<PointLight>> pointLights;

	const float gravityScale = 5.0f;
	const vec2 gravity;

	std::vector<std::shared_ptr<RigidBody2DComponent>> physicsObjects;

	std::vector<std::shared_ptr<Manifold>> contacts;


	bool isUseCamera3D = false;

	const float floorWidth = 30.0f;
	const float floorDepth = 30.0f;

	const std::string defaultCheckeredTexture = "Default Checkered";

	float m_scale = 0.0f;

	const static int physicsIterations = 10;


	bool hasLMBClicked = false;

	Physics2DScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo), gravity(0.0f, -10.0f * gravityScale)
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
			camera2D->SetOrthographicProjection(-static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.width), -static_cast<float>(windowInfo.height), static_cast<float>(windowInfo.height), 5, 100);
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
	
		//InitialisePolyDemoScene();
		InitialiseFrictionDemoScene();
	}

	void InitialiseFrictionDemoScene()
	{
		vec2 floorPos(15.0f, -15.0f);
		std::shared_ptr<GameObject> floor = CreateStaticPolyPhysicsObject(floorPos, 0.0f, 30.0f, 2.0f);
		gameObjectManager.push_back(floor);

		vec2 floorPosLeft(0.5f, -11.5f);
		std::shared_ptr<GameObject> floorLeft = CreateStaticPolyPhysicsObject(floorPosLeft, 0.0f, 1.0f, 5.0f);
		gameObjectManager.push_back(floorLeft);

		vec2 floorPosRight(29.5f, -11.5f);
		std::shared_ptr<GameObject> floorRight = CreateStaticPolyPhysicsObject(floorPosRight, 0.0f, 1.0f, 5.0f);
		gameObjectManager.push_back(floorRight);

		/*vec2 posZero = vec2(0.0f, 00.0f);
		std::shared_ptr<GameObject> circleStatic = CreateCirclePhysicsObject(1.0f, 0.2f, posZero, 0.0f, true);
		gameObjectManager.push_back(circleStatic);*/
	}

	void InitialisePolyDemoScene()
	{
		vec2 pos2 = vec2(17.0f, 20.0f);
		std::shared_ptr<GameObject> poly1 = CreatePolygonPhysicsObject(pos2, 0.0f);
		gameObjectManager.push_back(poly1);

		vec2 floorPos(15.0f, -15.0f);
		std::shared_ptr<GameObject> floor = CreateStaticPolyPhysicsObject(floorPos, 0.0f, 30.0f, 2.0f);
		gameObjectManager.push_back(floor);

		vec2 floorPosLeft(2.5f, -9.5f);
		std::shared_ptr<GameObject> floorLeft = CreateStaticPolyPhysicsObject(floorPosLeft, 0.0f, 2.0f, 5.0f);
		gameObjectManager.push_back(floorLeft);

		vec2 floorPosRight(27.5f, -13.5f);
		std::shared_ptr<GameObject> floorRight = CreateStaticPolyPhysicsObject(floorPosRight, 0.0f, 2.0f, 5.0f);
		gameObjectManager.push_back(floorRight);
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


		if(hasLMBClicked)
		{

			POINT mouse; 
			GetCursorPos(&mouse); 
			HWND hWnd = WindowFromDC(wglGetCurrentDC());
			ScreenToClient(hWnd, &mouse);
			printf("mouse: %i,%i\n", mouse.x, mouse.y);
			vec2 pos;

			pos = UnprojectGLM(mouse.x, mouse.y);
		
			std::shared_ptr<GameObject> circle = CreateCirclePhysicsObject(1.0f, 0.2f, pos, 0.0f);
			gameObjectManager.push_back(circle);
			hasLMBClicked = false;
		}
	}

	void GenerateCollisionManifolds()
	{
		//Generate contact info
		contacts.clear();
		for (size_t i = 0; i < physicsObjects.size(); i++)
		{
			std::shared_ptr<RigidBody2DComponent> bodyA = physicsObjects[i];

			for (size_t j = i + 1; j < physicsObjects.size(); ++j)
			{
				std::shared_ptr<RigidBody2DComponent> bodyB = physicsObjects[j];

				std::shared_ptr<Manifold> manifold = std::make_shared<Manifold>(bodyA, bodyB);

				manifold->Solve();
				if (manifold->contacts.size() > 0)
					contacts.push_back(manifold);
			}
		}
	}

	void IntegrateForces(std::shared_ptr<RigidBody2DComponent>& body, float dt)
	{
		if (body->inverseMass == 0.0f)
			return;

		body->velocity += (body->force * body->inverseMass + gravity) * (dt * 0.5f); //apply current forces and gravity
		body->angularVelocity += body->torque * body->inverseInertia * (dt *0.5f);
	}

	void IntegrateVelocity(std::shared_ptr<RigidBody2DComponent>& body, float dt)
	{
		if (body->inverseMass == 0.0f)
			return;

		body->position += body->velocity * dt;
		body->orient += body->angularVelocity * dt;

		//pass to game object world transform
		body->GetParentGameObject().lock()->SetPosition2D(body->position);
		body->GetParentGameObject().lock()->SetOrientation2D(body->orient); //possibly set directly from body->u
		//body->GetParentGameObject().lock()->SetOrientation2D(body->u);
		
		IntegrateForces(body, dt);
	}

	void OnFixedTimeStep() override
	{
		GenerateCollisionManifolds();

		//Integrate forces
		for (size_t i = 0; i < physicsObjects.size(); i++)
			IntegrateForces(physicsObjects[i], fixedTimeStep);

		//Initialise collisions
		for (size_t i = 0; i < contacts.size(); i++)
			contacts[i]->Initialise(fixedTimeStep, gravity);
		
		//Solve collisions
		for (size_t j = 0; j < physicsIterations; ++j)
			for (size_t i = 0; i < contacts.size(); ++i)
				contacts[i]->ApplyImpulse();

		//Integrate velocities
		for (size_t i = 0; i < physicsObjects.size(); ++i)
			IntegrateVelocity(physicsObjects[i], fixedTimeStep);

		//Correct positions (due to floating point errors)
		for (size_t i = 0; i < contacts.size(); i++)
			contacts[i]->PositionalCorrection();

		//Reset forces
		for (size_t i = 0; i < physicsObjects.size(); ++i)
		{
			physicsObjects[i]->force.x = 0.0f;
			physicsObjects[i]->force.y = 0.0f;
			physicsObjects[i]->torque = 0.0f;
		}
	}

	void notifyDisplayFrame() override
	{
		Check_GLError();
		vec3 cameraPos;
		if (isUseCamera3D)
		{
			cameraPos = vec3(camera->GetWorldTransform()[3]);
		}
		else
		{
			cameraPos = vec3(camera2D->GetWorldTransform()[3]);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->Enable();
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetDirectionalLight(directionalLight);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetPointLights(pointLights);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetSpotLights(spotLights);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularIntensity(1.0f);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularPower(32.0f);
		}

		Check_GLError();
		SceneManager::notifyDisplayFrame();

		Check_GLError();
		RenderContacts();
		Check_GLError();
	}

	void RenderContacts()
	{
		for(auto i : contacts)
		{
			mat4 ident;
			i->Render(this->mainCamera.lock(), ident);
		}
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		SceneManager::notifyProcessNormalKeys(key, x, y);

		if(isUseCamera3D)
			camera->OnKey(key, x, y);
		else
			camera2D->OnKey(key, x, y);
	}

	void notifyProcessMouseState(int button, int state, int x, int y) override
	{
		if(button == GLUT_LEFT_BUTTON)
		{
			if (state == GLUT_UP)
				hasLMBClicked = true;
		}
	}

	vec2 UnprojectScreenSpaceCoords(int x, int y)
	{
		mat4 vp = mainCamera.lock()->projection * mainCamera.lock()->view;
		mat4 pv = inverse(vp);

		vec4 in;
		float winZ = 1.0;

		in[0] = (2.0f*((float)(x - 0) / (this->windowInfo.width - 0))) - 1.0f,
		in[1] = 1.0f - (2.0f*((float)(y - 0) / (this->windowInfo.height - 0)));
		in[2] = 2.0* winZ - 1.0;
		in[3] = 1.0;

		vec4 pos = in * pv;
		pos.w = 1.0f / pos.w;
		pos.x *= pos.w;
		pos.y *= pos.w;
		pos.z *= pos.w;

		return vec2(pos);
	}

	vec2 UnprojectGLUT(int x, int y)
	{
		vec3 pos;

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		
		GLdouble modelview[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

		GLdouble projection[16];
		glGetDoublev(GL_PROJECTION_MATRIX, projection);

		GLfloat winX, winY, winZ;
		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		GLdouble posX, posY, posZ;
		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		return vec2(posX, posY);
	}

	vec2 UnprojectGLM(int x, int y)
	{
		GLint viewport[4];
		GLfloat winY, z;
		glGetIntegerv(GL_VIEWPORT, viewport);
		
		winY = viewport[3] - (float)y;
		glReadPixels(x, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
		glm::vec3 screen = glm::vec3(x, winY, z);

		vec4 viewportGLM(viewport[0], viewport[1], viewport[2], viewport[3]);

		vec3 pos = glm::unProject(screen, camera2D->view, camera2D->projection, viewportGLM);
		printf("GLM pos: %f,%f,%f\n", pos.x, pos.y, pos.z);

		return vec2(pos);
	}

	void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		if (isUseCamera3D)
		{
			//camera->OnMouseMove(deltaX, deltaY);
		}
		//else
		//	camera2D->OnMouseMove(deltaX, deltaY);
	}

	std::shared_ptr<GameObject> CreateCirclePhysicsObject(float density, float restitution, vec2 position, float orientation, bool isStatic = false)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		go->AddComponent(AssetManager::GetInstance().CreateCirclePrimitiveMeshComponent(2.5f, 32));
		
		std::shared_ptr<RigidBody2DComponent> rigidBodyComponent = std::make_shared<RigidBody2DComponent>(go);
		go->AddComponent(rigidBodyComponent);
		rigidBodyComponent->physicsMaterial->density = density;
		rigidBodyComponent->physicsMaterial->restitution = restitution;
		rigidBodyComponent->position = position;
		rigidBodyComponent->SetOrientation2D(orientation);
		go->SetPosition2D(position);
		go->SetOrientation2D(orientation);

		std::shared_ptr<SphereColliderComponent> sphereColliderComponent = std::make_shared<SphereColliderComponent>(go, 2.5f);
		go->AddComponent(sphereColliderComponent);

		PhysicsManager::ComputeSphereMass(rigidBodyComponent, sphereColliderComponent);

		if (isStatic)
			rigidBodyComponent->SetStatic();

		physicsObjects.push_back(rigidBodyComponent);

		return go;
	}

	std::shared_ptr<GameObject> CreatePolygonPhysicsObject(vec2 position, float orientation)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> polygonMesh = AssetManager::GetInstance().CreateRandomPolygonPrimitiveMeshComponent(3,6, 2.5f);
		go->AddComponent(polygonMesh);

		std::shared_ptr<RigidBody2DComponent> rigidBodyComponent = std::make_shared<RigidBody2DComponent>(go);
		go->AddComponent(rigidBodyComponent);
		rigidBodyComponent->position = position;
		rigidBodyComponent->SetOrientation2D(orientation);
		go->SetPosition2D(position);
		go->SetOrientation2D(orientation);


		PhysicsManager::ComputePolygonMass(rigidBodyComponent, polygonMesh->rootMeshNode->meshes[0]->vertices);
		//ComputePolygonMass centers vertices around the centroid, need to re-bind
		polygonMesh->rootMeshNode->meshes[0]->BuildAndBindVertexPositionColorBuffer();

		std::vector<vec2> verts2D;
		std::vector<vec2> norms2D;

		for(auto i : polygonMesh->rootMeshNode->meshes[0]->vertices)
			verts2D.push_back(vec2(i));

		for (auto i : polygonMesh->rootMeshNode->meshes[0]->normals)
			norms2D.push_back(vec2(i));

		std::shared_ptr<PolygonColliderComponent> polyColliderComponent = std::make_shared<PolygonColliderComponent>(go, verts2D, norms2D);
		go->AddComponent(polyColliderComponent);

		physicsObjects.push_back(rigidBodyComponent);

		return go;
	}

	std::shared_ptr<GameObject> CreateStaticPolyPhysicsObject(vec2 position, float orientation, float width, float height)
	{
		std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
		std::shared_ptr<MeshComponent> polygonMesh = AssetManager::GetInstance().CreateSimpleQuadPrimitiveMeshComponent(width, height);
		go->AddComponent(polygonMesh);
		std::shared_ptr<RigidBody2DComponent> rigidBodyComponent = std::make_shared<RigidBody2DComponent>(go);
		go->AddComponent(rigidBodyComponent);
		rigidBodyComponent->position = position;
		rigidBodyComponent->SetOrientation2D(orientation);
		go->SetPosition2D(position);
		go->SetOrientation2D(orientation);

		std::vector<vec2> verts2D;
		std::vector<vec2> norms2D;

		for (auto i : polygonMesh->rootMeshNode->meshes[0]->vertices)
			verts2D.push_back(vec2(i));

		for (auto i : polygonMesh->rootMeshNode->meshes[0]->normals)
			norms2D.push_back(vec2(i));

		std::shared_ptr<PolygonColliderComponent> polyColliderComponent = std::make_shared<PolygonColliderComponent>(go, verts2D, norms2D);
		go->AddComponent(polyColliderComponent);
		
		physicsObjects.push_back(rigidBodyComponent);
		
		rigidBodyComponent->SetStatic();
		
		return go;
	}
};

