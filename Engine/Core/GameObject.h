#pragma once
#include "IGameObject.h"
#include "IComponent.h"
#include "Components/Interfaces/IRenderableComponent.h"
#include <vector>
#include <algorithm>

class IComponent;

namespace Core
{
	class GameObject : public IGameObject, public std::enable_shared_from_this<GameObject>
	{
	public:
		glm::mat4 world;
		std::vector<std::unique_ptr<IComponent>> components;
		std::vector<std::weak_ptr<IGameObject>> children;
		std::weak_ptr<IGameObject> parent;

		std::vector<std::weak_ptr<IGameObject>>& GetChildren() override { return children; }
		std::weak_ptr<IGameObject>& GetParent() override { return parent; }

		glm::mat4& GetWorldTransform() override { return world; }

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
			
		#pragma region Component Management
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
		#pragma endregion

		#pragma region Transforms
 		void Translate(glm::vec3 trans)
		{
			world = glm::translate(world, trans);
		}

		void Translate(float x, float y, float z)
		{
			world = glm::translate(world, glm::vec3(x, y, z));
		}

		void Rotate(float angle, glm::vec3 axis)
		{
			world = glm::rotate(world, angle, axis);
		}

		void RotateX(float angle)
		{
			world = glm::rotate(world, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		void RotateY(float angle)
		{
			world = glm::rotate(world, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		void RotateZ(float angle)
		{
			world = glm::rotate(world, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		void Scale(float s)
		{
			world = glm::scale(world, glm::vec3(s));
		}

		void Scale(float x, float y, float z)
		{
			world = glm::scale(world, glm::vec3(x,y,z));
		}

		void Scale(glm::vec3 scale)
		{
			world = glm::scale(world, scale);
		}
		#pragma endregion 
	};
}

