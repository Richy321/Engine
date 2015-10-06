#pragma once
#include "../Mesh.h"
namespace Rendering
{
	namespace Primitives
	{
		class Triangle : public Mesh
		{
		public:
			Triangle();
			~Triangle();

			void Create();
			virtual void Update() override final;
			virtual void Draw() override final;
		};
	}
}