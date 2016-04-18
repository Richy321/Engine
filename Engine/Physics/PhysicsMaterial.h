#pragma once
class PhysicsMaterial
{
public:

	PhysicsMaterial()
	{
	}

	PhysicsMaterial(float density, float restitution)
	{
		this->density = density;
		this->restitution = restitution;
	}

	~PhysicsMaterial()
	{
	}

	float density;
	float restitution;
};
