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

		MeshComponent(std::weak_ptr<IGameObject> parent) : IComponent(parent), IRenderableComponent(parent)
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

		void Update(float deltaTime) override
		{
			rootMeshNode->Update();
		}

		void Destroy() override
		{
		}
	
		size_t AddRootMesh(std::shared_ptr<Core::Mesh> mesh) const
		{
			rootMeshNode->AddMesh(mesh);
			return rootMeshNode->meshes.size() - 1;
		}

		void RemoveRootMesh(int pos) const
		{
			rootMeshNode->meshes.erase(rootMeshNode->meshes.begin() + pos);
		}

		ComponentTypes GetComponentType() const override { return ComponentTypes::Mesh; }
	};
}
