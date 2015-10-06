#pragma once
#include "../Mesh.h"
namespace Rendering
{
	namespace Primitives
	{
		class Quad :
			public Mesh
		{
		public:
			Quad();
			~Quad();

			void Create();
			virtual void Draw() override final;
			virtual void Update() override final;
		};
	}
}
