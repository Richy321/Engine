#pragma once
#include <memory>

namespace Core
{
	class IGameObject;
	enum ComponentFlags
	{
		None = 1,
		Renderable = 2,
		NetworkSyncable = 4,
		Physics = 8
	};

	template<class T> T operator~ (T a) { return (T)~(int)a; }
	template<class T> T operator| (T a, T b) { return (T)((int)a | (int)b); }
	template<class T> T operator& (T a, T b) { return (T)((int)a & (int)b); }
	template<class T> T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
	template<class T> T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
	template<class T> T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
	template<class T> T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

	class IComponent
	{
	protected:
		unsigned int id;
		ComponentFlags componentFlags;
		std::weak_ptr<IGameObject> parentGameObject;
	public:
		enum ComponentTypes
		{
			Mesh,
			NetworkView,
			DirectionalMovement,
			ParticleSystem
		};

		int GetID() const { return id; }
		IComponent(std::weak_ptr<IGameObject> gameObj) : componentFlags(None), parentGameObject(gameObj)
		{
			static unsigned int idCounter = 0;  id = idCounter++;
		}
		virtual ~IComponent() {}
		ComponentFlags GetComponentFlags() const { return componentFlags; }
		
		std::weak_ptr<IGameObject> GetParentGameObject() const { return parentGameObject; }
		void SetParentGameObject(std::weak_ptr<IGameObject> parent) { parentGameObject = parent; }

		virtual void Update(float deltaTime) = 0;
		virtual void Destroy() {}

		virtual ComponentTypes GetComponentType() const = 0;
	};
}
