#pragma once
#include "Dependencies/glm/detail/type_vec4.hpp"
#include "Core/Colours.h"

class Material
{
public:

	vec3 colour;
	vec3 emissionColour;

	float transparency;
	float reflection;
	
	//phong lighting settings
	float specularExponent;
	float Kd; //diffuse scalar
	float Ks; //specular scalar

	float indexOfRefraction; //(1.0 = air    1.5 = glass)



	Material() : colour(vec3(Colours_RGBA::HotPink)), emissionColour(0.0f, 0.0f, 0.0f), transparency(0.0f), reflection(0.0f), specularExponent(25.0f), Kd(0.8f), Ks(0.2f), indexOfRefraction(1.3f)
	{
	}

	~Material()
	{
	}
};

