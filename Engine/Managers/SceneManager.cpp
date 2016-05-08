#include "SceneManager.h"
#include "../Core/GameObject.h"
using namespace Managers;
using namespace Core;

SceneManager::SceneManager(Initialisation::WindowInfo windowInfo) : timer(new Timer), lastUpdateTime(0.0f), lastUpdateCommsTime(0.0f)
{
	this->windowInfo = windowInfo;
	mousePosX = mousePosY = -1;
	mouseDeltaX = mouseDeltaY = 0;
}

SceneManager::~SceneManager()
{

}
std::map<unsigned char, bool> SceneManager::keyState;


void SceneManager::Initialise()
{
	if (!ShaderManager::GetInstance().Initialise())
		printf("Failed to Initialise Shader Manager");

	glEnable(GL_DEPTH_TEST);
	ShaderManager::GetInstance().CreateProgram("basicColor", "Shaders\\basicColor.vert", "Shaders\\basicColor.frag");
	ShaderManager::GetInstance().CreateProgram("basicLighting", "Shaders\\basicLighting.vert", "Shaders\\basicLighting.frag");
	timer->Start();
}

void SceneManager::OnFixedTimeStep()
{
	//Call specific physics GO functions
	for (std::shared_ptr<GameObject>& go : gameObjectManager)
		go->UpdatePhysics(fixedTimeStep);
}

void SceneManager::notifyBeginFrame()
{
	timer->Update();

	float elapsedTime = timer->GetElapsedTime();

	static double timeStepAccumulator = 0;
	timeStepAccumulator += elapsedTime;

	//avoid spiral of death (if fixed timestep tick takes a long time, elapsed time will increase endlessly)
	clamp(timeStepAccumulator, 0.0, 0.1);
	while (timeStepAccumulator >= fixedTimeStep)
	{
		OnFixedTimeStep();
		timeStepAccumulator -= fixedTimeStep;
	}

	/* Network Manager updates registered network components on seperate thread
	float nowTime = timer->GetRunningTime();
	if (nowTime > lastUpdateCommsTime + CommsTickDurationMs * 0.001)
	{
		for (std::shared_ptr<GameObject>& go : gameObjectManager)
			go->UpdateNetworkComms(elapsedTime);

		OnCommsUpdate(nowTime - lastUpdateCommsTime);

		lastUpdateCommsTime = nowTime;
	}*/

	for(std::shared_ptr<GameObject>& go : gameObjectManager)
		go->Update(elapsedTime);
	OnUpdate(elapsedTime);
}


void SceneManager::UpdateShaderUniforms() const
{
	Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetViewMatrix(mainCamera.lock()->view);
	Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetProjectionMatrix(mainCamera.lock()->projection);

	vec3 cameraPos = vec3(mainCamera.lock()->GetWorldTransform()[3]);
	Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetEyeWorldPos(cameraPos);
	Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularIntensity(1.0f);
	Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularPower(32.0f);

	if(directionalLight != nullptr)
		ShaderManager::GetInstance().litTexturedMeshEffect->SetDirectionalLight(directionalLight);
	Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetPointLights(pointLights);
	Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetSpotLights(spotLights);
}

void SceneManager::notifyDisplayFrame()
{
	Check_GLError();
	UpdateShaderUniforms();

	Check_GLError();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	for (size_t i = 0; i < gameObjectManager.size(); i++)
	{
		assert(!mainCamera.expired());
		if (!mainCamera.expired())
		{
			std::shared_ptr<Camera> mainCamPtr = mainCamera.lock();
			gameObjectManager[i]->Render(mainCamPtr);
		}
	}
	
	{
		std::lock_guard<std::mutex> lock(mutexGameObjectManager);
		for (std::shared_ptr<GameObject>& go : gameObjectManager)
		{
			assert(!mainCamera.expired());
			if (!mainCamera.expired())
			{
				std::shared_ptr<Camera> mainCamPtr = mainCamera.lock();
				go->Render(mainCamPtr);
			}
		}
	}

}

void SceneManager::notifyEndFrame()
{
	//nothing here for the moment
}

void SceneManager::notifyReshape(int width,
	int height,
	int previous_width,
	int previous_height)
{
	//todo - handle resizing
}

void SceneManager::notifyErrorCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) 
{
	std::cout << "---------------------opengl-callback-start------------" << std::endl;
	std::cout << "message: " << message << std::endl;
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	}
	std::cout << std::endl;

	std::cout << "id: " << id << std::endl;
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	}
	std::cout << std::endl;
	std::cout << "---------------------opengl-callback-end--------------" << std::endl;
}




