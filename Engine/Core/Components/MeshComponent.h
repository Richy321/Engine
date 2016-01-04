#pragma once
#include "Interfaces/IRenderableComponent.h"
#include <vector>
#include "../Mesh.h"
#include "../MeshNode.h"

namespace Core
{
	class MeshComponent : public IRenderableComponent
	{
	public:

		std::shared_ptr<MeshNode> rootMeshNode;

		MeshComponent(std::weak_ptr<IGameObject> parent) : IRenderableComponent(parent)
		{
			rootMeshNode = std::make_shared<MeshNode>();
		}

		~MeshComponent()
		{
			MeshComponent::Destroy();
		}
		
		void Render(std::shared_ptr<Camera> mainCamera) override
		{
			rootMeshNode->Render(mainCamera, GetParentGameObject().lock()->GetWorldTransform());
		}

		void Update() override
		{
			rootMeshNode->Update();
		}

		void Destroy() override
		{
		}

		static std::unique_ptr<MeshComponent> CreateTrianglePrimitive()
		{
			std::unique_ptr<MeshComponent> triangleMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			triangleMesh->AddRootMesh(Mesh::CreateTrianglePrimitive());
			return triangleMesh;
		}

		static std::unique_ptr<MeshComponent> CreateCubePrimitive(float size = 1.0f)
		{
			std::unique_ptr<MeshComponent> cubeMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			cubeMesh->AddRootMesh(Mesh::CreateCubePrimitive(size));
			return cubeMesh;
		}

		static std::unique_ptr<MeshComponent> CreateBoxPrimitive(float width, float height, float depth)
		{
			std::unique_ptr<MeshComponent> boxMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			boxMesh->AddRootMesh(Mesh::CreateBoxPrimitive(width, height, depth));
			return boxMesh;
		}
		
		size_t AddRootMesh(std::shared_ptr<Mesh> mesh) const
		{
			rootMeshNode->AddMesh(mesh);
			return rootMeshNode->meshes.size() - 1;
		}

		void RemoveRootMesh(int pos) const
		{
			rootMeshNode->meshes.erase(rootMeshNode->meshes.begin() + pos);
		}
	};
}
