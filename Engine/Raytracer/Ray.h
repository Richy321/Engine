#pragma once
class Ray
{
public:

	vec3 origin;
	vec3 direction;
	float tMin;
	float tMax;

	enum RayType
	{
		Primary,
		Shadow,
		Reflection,
		Refraction
	} rayType;

	Ray() : origin(0.0f), direction(0.0f, 0.0f, -1.0f), tMin(0.1f), tMax(1000.0f), rayType(Primary)
	{}

	Ray(vec3 origin, vec3 dir, RayType rayType = Primary) : origin(origin), direction(dir), tMin(0.1f), tMax(1000.0f), rayType(rayType)
	{
	}

	~Ray()
	{
	}

	bool IntersectSphere(Ray* orig, Ray* direction, float t)
	{

	}

	bool 
		IntersectTriangle()
	{

	}


};

