#pragma once
#include "../Math/mat4.h"
#include "IGameObject.h"
#include "IComponent.h"
#include "Components/Interfaces/IRenderableComponent.h"
#include <vector>
#include <algorithm>

class IComponent;
using namespace Math;
namespace Core
{
	class GameObject : public IGameObject, public std::enable_shared_from_this<GameObject>
	{
	public:
		mat4 world;
		std::vector<std::unique_ptr<IComponent>> components;
		std::vector<std::weak_ptr<IGameObject>> children;
		std::weak_ptr<IGameObject> parent;

		std::vector<std::weak_ptr<IGameObject>>& GetChildren() override { return children; }
		std::weak_ptr<IGameObject>& GetParent() override { return parent; }

		mat4& GetWorldTransform() override { return world; }
		GameObject()
		{
		}

		virtual ~GameObject()
		{
		}

		virtual void Initialise()
		{
		}

		virtual void Update(float deltaTime)
		{
			for (auto& comp : components)
			{
				comp->Update();
			}
		}

		virtual void UpdatePhysics()
		{
			
		}

		virtual void UpdateNetworkComms()
		{
			
		}

		virtual void Render(std::shared_ptr<Camera> mainCamera)
		{
			for(auto& comp : components)
			{
				if (comp->GetComponentFlags() | ComponentFlags::Renderable)
				{
					static_cast<IRenderableComponent&>(*comp.get()).Render(mainCamera);
				}
			}
		}

		void AddComponent(std::unique_ptr<IComponent> component)
		{
			component->SetParentGameObject(shared_from_this());
			components.push_back(std::move(component));
		}

		void RemoveComponent(std::unique_ptr<IComponent> &component)
		{
			components.erase(std::remove(components.begin(), components.end(), component), components.end());
		}

		void RemoveComponent(int componentID)
		{
			components.erase(std::remove_if(components.begin(), components.end(), [&](const std::unique_ptr<IComponent>& c) {  return c->GetID() == componentID; }));
		}
	};
}

