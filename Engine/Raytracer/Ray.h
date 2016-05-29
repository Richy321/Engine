#pragma once
class Ray
{
public:

	vec3 origin;
	vec3 direction;

	Ray(vec3 origin, vec3 dir) : origin(origin), direction(dir)
	{
	}

	~Ray()
	{
	}

	void IntersectSphere()
	{

	}

	void IntersectTriangle()
	{

	}
};

