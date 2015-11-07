#pragma once

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
	public:
		int GetID() const { return id; }
		IComponent() : componentFlags(None)
		{
			static unsigned int idCounter = 0;  id = idCounter++;
		}
		virtual ~IComponent() {}
		ComponentFlags GetComponentFlags() const { return componentFlags; }
		
		virtual void Update() = 0;
		virtual void Destroy() = 0;
	};
}