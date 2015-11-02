#include "MeshManager.h"

using namespace Managers;
using namespace Rendering;

MeshManager::MeshManager()
{
	//triangle game object
	Primitives::Triangle* triangle = new Primitives::Triangle();
	triangle->SetProgram(ShaderManager::GetShader("colorShader"));
	triangle->Create();
	gameModelList["triangle"] = triangle;

	Primitives::Quad* quad = new Primitives::Quad();
	quad->SetProgram(ShaderManager::GetShader("colorShader"));
	quad->Create();
	gameModelList["quad"] = quad;
}

MeshManager::~MeshManager()
{
	for (auto model : gameModelList)
		delete model.second;

	gameModelList.clear();
}

void MeshManager::DeleteModel(const std::string& gameModelName)
{
	IRenderObject* model = gameModelList[gameModelName];
	model->Destroy();
	gameModelList.erase(gameModelName);
}

const IRenderObject& MeshManager::GetModel(const std::string& gameModelName) const
{
	return *gameModelList.at(gameModelName);
}

void MeshManager::Update()
{
	for (auto model : gameModelList)
		model.second->Update();
}

void MeshManager::Draw()
{
	for (auto model : gameModelList)
		model.second->Draw();
}