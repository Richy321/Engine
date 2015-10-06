#pragma once
#include <map>
#include "ShaderManager.h"
#include "../Rendering/IRenderObject.h"
#include "../Rendering/Primitives/Triangle.h"
#include "../Rendering/Primitives/Quad.h"

using namespace Rendering;
namespace Managers
{
	class MeshManager
	{
	public:
		MeshManager();
		~MeshManager();

		void Draw();
		void Update();
		void DeleteModel(const std::string& gameModelName);
		const IRenderObject& GetModel(const std::string& gameModelName) const;

	private:
		std::map<std::string, IRenderObject*> gameModelList;
	};
}

