#pragma once
#include "../Dependencies/glm/glm.hpp"
#include "Mesh.h"

namespace Core
{
	class MeshNode
	{
	public:
		mat4 toParent;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<std::shared_ptr<MeshNode>> children;

		MeshNode()
		{

		}

		~MeshNode()
		{

		}

		void AddMesh(std::shared_ptr<Mesh> mesh)
		{
			meshes.push_back(mesh);
		}

		void AddChild(std::shared_ptr<MeshNode> child)
		{
			children.push_back(child);
		}

		void Render(std::shared_ptr<Camera> mainCamera, const mat4& toWorld)
		{
			mat4 m = toParent * toWorld;

			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->Render(mainCamera, m);

			for (size_t i = 0; i < children.size(); i++)
				children[i]->Render(mainCamera, m);
		}

		void Update()
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->Update();
		}

	};
}
