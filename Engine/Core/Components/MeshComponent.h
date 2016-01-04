#pragma once
#include "Interfaces/IRenderableComponent.h"
#include <vector>
#include "../Mesh.h"

namespace Core
{
	class MeshComponent : public IRenderableComponent
	{
	public:

		std::vector<std::shared_ptr<Mesh>> meshes;

		MeshComponent(std::weak_ptr<IGameObject> parent) : IRenderableComponent(parent)
		{

		}

		~MeshComponent()
		{
			MeshComponent::Destroy();
		}
		
		void Render(std::shared_ptr<Camera> mainCamera) override
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->Render(mainCamera);
		}

		void Update() override
		{
			for (size_t i = 0; i < meshes.size(); i++)
			{
				meshes[i]->toParent = parentGameObject.lock()->GetWorldTransform();
				meshes[i]->Update();
			}
		}

		void Destroy() override
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->Destroy();

			meshes.clear();
		}

		static std::unique_ptr<MeshComponent> CreateTrianglePrimitive()
		{
			std::unique_ptr<MeshComponent> triangleMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			triangleMesh->AddMesh(Mesh::CreateTrianglePrimitive());
			return triangleMesh;
		}

		static std::unique_ptr<MeshComponent> CreateCubePrimitive(float size = 1.0f)
		{
			std::unique_ptr<MeshComponent> cubeMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			cubeMesh->AddMesh(Mesh::CreateCubePrimitive(size));
			return cubeMesh;
		}

		static std::unique_ptr<MeshComponent> CreateBoxPrimitive(float width, float height, float depth)
		{
			std::unique_ptr<MeshComponent> boxMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			boxMesh->AddMesh(Mesh::CreateBoxPrimitive(width, height, depth));
			return boxMesh;
		}
		
		size_t AddMesh(std::shared_ptr<Mesh> mesh)
		{
			meshes.push_back(mesh);
			return meshes.size() - 1;
		}

		void RemoveMesh(int pos)
		{
			meshes.erase(meshes.begin() + pos);
		}
	};
}
