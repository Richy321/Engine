#pragma once
#include "../Dependencies/glm/glm.hpp"
using namespace glm;
namespace Core
{
	struct BaseLight
	{
		vec3 Color;
		float AmbientIntensity;
		float DiffuseIntensity;

		BaseLight()
		{
			Color = vec3(0.0f, 0.0f, 0.0f);
			AmbientIntensity = 0.0f;
			DiffuseIntensity = 0.0f;
		}
	};

	struct DirectionalLight : BaseLight
	{
		vec3 Direction;

		DirectionalLight()
		{
			Direction = vec3(0.0f, 0.0f, 0.0f);
		}
	};

	struct PointLight : BaseLight
	{
		vec3 Position;

		struct
		{
			float Constant;
			float Linear;
			float Exp;
		} Attenuation;

		PointLight()
		{
			Position = vec3(0.0f, 0.0f, 0.0f);
			Attenuation.Constant = 1.0f;
			Attenuation.Linear = 0.0f;
			Attenuation.Exp = 0.0f;
		}
	};

	struct SpotLight : PointLight
	{
		vec3 Direction;
		float Cutoff;

		SpotLight()
		{
			Direction = vec3(0.0f, 0.0f, 0.0f);
			Cutoff = 0.0f;
		}
	};
}
