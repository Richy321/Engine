#pragma once
#include "IGameObject.h"
#include "Components/Interfaces/IRenderableComponent.h"
#include <vector>
#include <algorithm>
#include "Components/Interfaces/IComponent.h"
#include "Components/NetworkViewComponent.h"

class IComponent;

namespace Core
{
	class GameObject : public IGameObject, public std::enable_shared_from_this<GameObject>
	{
	private:
		unsigned int ID;
	public:
		glm::mat4 world;
		std::vector<std::shared_ptr<IComponent>> components;
		std::vector<std::weak_ptr<IGameObject>> children;
		std::weak_ptr<IGameObject> parent;

		std::vector<std::weak_ptr<IGameObject>>& GetChildren() override { return children; }
		std::weak_ptr<IGameObject>& GetParent() override { return parent; }

		bool isEnabled;

		mat4& GetWorldTransform() override { return world; }
		
		vec3 GetPosition() const { return vec3(world[3].x, world[3].y, world[3].z); }

		unsigned int GetID() override { return ID; }
		
		GameObject()
		{
			static unsigned int IDCounter = 0;
			ID = IDCounter++;
			isEnabled = true;
		}

		virtual ~GameObject()
		{
		}

		virtual void Initialise()
		{
		}

		virtual void Update(float deltaTime)
		{
			if (!isEnabled)
				return;
			for (auto& comp : components)
			{
				comp->Update(deltaTime);
			}
		}

		virtual void UpdatePhysics()
		{
			if (!isEnabled)
				return;
		}

		virtual void UpdateNetworkComms(float deltaTime)
		{
			if (!isEnabled)
				return;

			for (auto& comp : components)
			{
				if (comp->GetComponentFlags() & NetworkSyncable)
					std::dynamic_pointer_cast<INetworkViewComponent>(comp)->UpdateComms(deltaTime);
			}
		}

		virtual void Render(std::shared_ptr<Camera> mainCamera)
		{
			if (!isEnabled)
				return;

			for(auto& comp : components)
			{
				if (comp->GetComponentFlags() & Renderable)
				{
					static_cast<IRenderableComponent&>(*comp.get()).Render(mainCamera);
				}
			}
		}
			
		#pragma region Component Management
		virtual void AddComponent(std::shared_ptr<IComponent> component)
		{
			component->SetParentGameObject(shared_from_this());
			components.push_back(component);
		}

		void RemoveComponent(std::shared_ptr<IComponent> &component)
		{
			components.erase(std::remove(components.begin(), components.end(), component), components.end());
		}

		void RemoveComponent(int componentID)
		{
			components.erase(std::remove_if(components.begin(), components.end(), [&](const std::shared_ptr<IComponent>& c) {  return c->GetID() == componentID; }));
		}

		std::shared_ptr<IComponent> GetComponentByType(IComponent::ComponentTypes type) override
		{
			for (auto const& value : components)
			{
				if (value->GetComponentType() == type)
					return value;
			}
			return nullptr;
		}

		std::shared_ptr<IComponent> GetComponentByID(int id) override
		{
			for (auto const& value : components)
			{
				if (value->GetID() == id)
					return value;
			}
			return nullptr;
		}

		std::vector<std::shared_ptr<IComponent>> GetComponentsOfType(IComponent::ComponentTypes type) override
		{
			std::vector<std::shared_ptr<IComponent>> componentsOfType;
			for (auto const& value : components)
			{
				if (value->GetComponentType() == type)
					componentsOfType.push_back(value);
			}
			return componentsOfType;
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

		void Scale(vec3 scale)
		{
			world = glm::scale(world, scale);
		}

		#pragma endregion 
	};
}

