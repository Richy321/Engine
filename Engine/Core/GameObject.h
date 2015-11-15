#pragma once
#include "../Math/mat4.h"
#include "IComponent.h"
#include "Components/Interfaces/IRenderableComponent.h"
#include <vector>
#include <algorithm>

using namespace Math;
namespace Core
{
	class GameObject
	{
	private:
		GLuint gWorldLocation;
	public:
		mat4 world;
		std::vector<std::reference_wrapper<Core::IComponent>> components;

		GameObject()
		{
		}

		~GameObject()
		{
		}

		void Initialise()
		{
			gWorldLocation = glGetUniformLocation(Managers::ShaderManager::GetInstance().GetShader("colorShader"), "gWorld");

			assert(gWorldLocation != 0xFFFFFFFF);
		}

		void Update(float deltaTime)
		{
			glUniformMatrix4fv(gWorldLocation, 1, GL_FALSE, world.GetMatrixFloatValues());

			for each (IComponent &comp in components)
			{
				comp.Update();
			}
		}

		void UpdatePhysics()
		{
			
		}

		void UpdateNetworkComms()
		{
			
		}

		void Render()
		{
			for each (IComponent &comp in components)
			{
				if (comp.GetComponentFlags() | ComponentFlags::Renderable)
				{
					static_cast<IRenderableComponent&>(comp).Render();
				}
			}
		}


		void AddComponent(IComponent &component)
		{
			components.push_back(component);
		}

		/*void RemoveComponent(IComponent &component)
		{
			components.erase(std::remove(components.begin(), components.end(), component), components.end());
		}*/

		void RemoveComponent(int componentID)
		{
			components.erase(std::remove_if(components.begin(), components.end(), [&](const IComponent& c) {  return c.GetID() == componentID; }));
		}
	};
}

