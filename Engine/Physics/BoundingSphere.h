#pragma once
class BoundingSphere
{
public:

	BoundingSphere()
	{
	}

	BoundingSphere(float radius, vec3 position)
	{
		this->radius = radius;
		this->position = position;
	}

	~BoundingSphere()
	{
	}

	float radius;
	vec3 position;
};

