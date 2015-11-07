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
	public:
		mat4 worldTransform;
		std::vector<std::reference_wrapper<Core::IComponent>> components;

		GameObject()
		{
		}

		~GameObject()
		{
		}

		void Update(float deltaTime)
		{
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

		void RemoveComponent(IComponent &component)
		{
			//components.erase(std::remove(components.begin(), components.end(), component), components.end());
		}

		void RemoveComponent(int componentID)
		{
			components.erase(std::remove_if(components.begin(), components.end(), [&](const IComponent& c) {  return c.GetID() == componentID; }));
		}
	};
}

