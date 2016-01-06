#pragma once
#include "BaseShaderEffect.h"
#include "Dependencies/glm/glm.hpp"
#include "Core/Lights.h"
#include "Dependencies/glm/gtc/type_ptr.inl"

using namespace glm;

namespace Core
{
	class LitTexturedMeshEffect : public BaseShaderEffect
	{
	public:

		LitTexturedMeshEffect()
		{
		}

		bool Initialise() override
		{
			if (!BaseShaderEffect::Initialise())
				return false;

			if (!AddShader(GL_VERTEX_SHADER, "Shaders\\basicLighting.vert", "basicLightingVert"))
				return false;

			if (!AddShader(GL_FRAGMENT_SHADER, "Shaders\\basicLighting.frag", "basicLightingFrag"))
				return false;

			if (!Finalise())
				return false;

			WVPLocation = GetUniformLocation("gWVP");
			WorldMatrixLocation = GetUniformLocation("gWorld");
			samplerLocation = GetUniformLocation("gSampler");
			dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Color");
			dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.AmbientIntensity");
			dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
			dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.DiffuseIntensity");

			if (dirLightLocation.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
				WVPLocation == INVALID_UNIFORM_LOCATION ||
				WorldMatrixLocation == INVALID_UNIFORM_LOCATION ||
				samplerLocation == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.Color == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.Direction == INVALID_UNIFORM_LOCATION)
				return false;

			return true;
		}
		void SetWVP(const mat4& WVP) const
		{
			glUniformMatrix4fv(WVPLocation, 1, GL_TRUE, value_ptr(WVP));
		}


		void SetWorldMatrix(const mat4& WorldInverse) const
		{
			glUniformMatrix4fv(WorldMatrixLocation, 1, GL_TRUE, value_ptr(WorldInverse));
		}


		void SetTextureUnit(unsigned int TextureUnit) const
		{
			glUniform1i(samplerLocation, TextureUnit);
		}


		void SetDirectionalLight(const DirectionalLight& Light) const
		{
			glUniform3f(dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
			glUniform1f(dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
			vec3 Direction = normalize(Light.Direction);
			glUniform3f(dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
			glUniform1f(dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
		}

	private:
		GLuint WVPLocation;
		GLuint WorldMatrixLocation;
		GLuint samplerLocation;

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
		} dirLightLocation;
	};
}
