#pragma once
#include "../../Dependencies/glm/detail/type_vec3.hpp"

using namespace glm;

class ClothParticle2
{
public:

	ClothParticle2(vec3 position, vec3 acceleration)
		:position(position), velocity(0.0f, 0.0f, 0.0f), acceleration(acceleration), forceAccumulated(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), damping(0.4f), inverseMass(1.0f), movable(true)
	{
	}

	ClothParticle2()
		: position(0.0f, 0.0f, 0.0f), velocity(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f), forceAccumulated(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), damping(0.7f), inverseMass(1.0f), movable(true)
	{
	}

	ClothParticle2(const ClothParticle2& particle)
		: position(particle.position), velocity(particle.velocity), acceleration(particle.acceleration), forceAccumulated(particle.forceAccumulated),
		normal(particle.normal), damping(particle.damping), inverseMass(particle.inverseMass), movable(particle.movable)
	{
	}

	ClothParticle2& ClothParticle2::operator= (const ClothParticle2& particle) {
		position = particle.position;
		velocity = particle.velocity;
		acceleration = particle.acceleration;
		forceAccumulated = particle.forceAccumulated;
		normal = particle.normal;
		damping = particle.damping;
		inverseMass = particle.inverseMass;
		movable = particle.movable;
		return *this;
	}

	~ClothParticle2()
	{
	}

	vec3 position;
	vec3 velocity;
	vec3 acceleration;
	vec3 forceAccumulated;
	vec3 normal;
	float damping;
	float inverseMass;
	bool movable;

	vec4 rgba;

	void addNormal(const vec3& n) { normal += n; }
	void addForceAccumulated(const vec3& f) { forceAccumulated += f; }
	void multiplyVelocity(const float scalar) { velocity *= scalar; }
	void multiplyForceAccumulated(const float scalar) { forceAccumulated *= scalar; }

	void Integrate(float deltaTime)
	{
		if (deltaTime > 0.0f && movable)
		{
			position += velocity * deltaTime;
			velocity += (forceAccumulated * inverseMass) * deltaTime;
			forceAccumulated = vec3(0.0f, 0.0f, 0.0f);

			velocity *= pow(damping, deltaTime);
		}
	}

};

