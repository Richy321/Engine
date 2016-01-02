#pragma once
#include "../Dependencies/glm/glm.hpp"
#include "../Dependencies/glm/gtc/matrix_transform.hpp"
#include "Components/MeshComponent.h"

#include "../Dependencies/assimp/Importer.hpp"      // C++ importer interface
#include "../Dependencies/assimp/scene.h"           // Output data structure
#include "../Dependencies/assimp/postprocess.h"     // Post processing fla

using namespace glm;

namespace Core
{
	class AssetManager
	{
	public:
		static AssetManager GetInstance()
		{
			static AssetManager Instance;
			return Instance;
		}

		std::vector<MeshComponent> meshComponents;
		AssetManager()
		{
		}

		~AssetManager()
		{
		}

		std::unique_ptr<MeshComponent> LoadMeshFromFile(const std::string& filename) const
		{
			// Create a logger instance 
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filename,
				aiProcess_CalcTangentSpace |
				aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType);
			if(!scene)
			{
				printf("AssImp Failed: %s", importer.GetErrorString());
				assert(scene);
			}

			std::unique_ptr<MeshComponent> rval = std::make_unique<MeshComponent>(std::weak_ptr<IGameObject>());
			return rval;
		}
	};
}