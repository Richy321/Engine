#pragma once
class AABB
{
public:
	AABB()
	{
	}

	AABB(vec3 min, vec3 max)
	{
		this->min = min;
		this->max = max;
	}

	~AABB()
	{
	}

	vec3 min;
	vec3 max;


};

