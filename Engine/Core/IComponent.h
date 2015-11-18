#pragma once
#include <memory>
#include "IGameObject.h"

namespace Core
{
	enum ComponentFlags
	{
		None = 1,
		Renderable = 2,

	};

	template<class T> inline T operator~ (T a) { return (T)~(int)a; }
	template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
	template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
	template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
	template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
	template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
	template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

	class IComponent
	{
	protected:
		unsigned int id;
		ComponentFlags componentFlags;
		std::weak_ptr<IGameObject> parentGameObject;
	public:
		int GetID() const { return id; }
		IComponent(std::weak_ptr<IGameObject> gameObj) : componentFlags(None), parentGameObject(gameObj)
		{
			
			static unsigned int idCounter = 0;  id = idCounter++;
		}
		virtual ~IComponent() {}
		ComponentFlags GetComponentFlags() const { return componentFlags; }
		
		std::weak_ptr<IGameObject> GetParentGameObject() const { return parentGameObject; }
		void SetParentGameObject(std::weak_ptr<IGameObject> parent) { parentGameObject = parent; }

		virtual void Update() = 0;
		virtual void Destroy() = 0;
	};
}
