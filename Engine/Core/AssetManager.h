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
		std::vector<MeshComponent> meshComponents;
		AssetManager()
		{
		}

		~AssetManager()
		{
		}

		void LoadMeshFromFile(std::string filename)
		{
			
		}
	};
}