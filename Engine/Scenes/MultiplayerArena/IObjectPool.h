#pragma once
#include "../../Core/GameObject.h"
namespace MultiplayerArena
{
	class IObjectFactoryPool
	{
	public:

		enum FactoryObjectType
		{
			Player,
			Bullet
		};

		virtual ~IObjectFactoryPool() {}

		virtual std::shared_ptr<Core::GameObject> GetFactoryObject(FactoryObjectType objectType) = 0;

		virtual void CreateFactoryObjects(FactoryObjectType objectType, unsigned int count) = 0;

	};
}