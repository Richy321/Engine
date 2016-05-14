#pragma once
#include "../../Dependencies/glm/detail/type_vec3.hpp"

using namespace glm;

class ClothParticle
{
public:

	ClothParticle(vec3 position) 
	{
		startingPos = position;
		Reset();
	}

	~ClothParticle()
	{
	}

	float mass;
	vec3 startingPos;
	vec3 pos;
	vec3 oldPos;
	vec3 acceleration;
	bool movable;
	vec3 normal;
	vec4 rgba;

	void Reset()
	{
		pos = startingPos;
		oldPos = startingPos;
		mass = 1;
		acceleration = vec3(0.0f, 0.0f, 0.0f);
	}

	void AddForce(vec3 f)
	{
		acceleration += f / mass;
	}

	void OnFixedTimeStep(float deltaTime, float damping)
	{
		if(movable)
		{
			//Verlet Integration
			vec3 tmp = pos;
			pos = pos + (pos - oldPos) * (1.0f - damping) + acceleration * deltaTime;
			oldPos = tmp;
			acceleration = vec3(0.0f, 0.0f, 0.0f);
		}
	}



};

