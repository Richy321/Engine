#pragma once
#include "BaseShaderEffect.h"
#include "../../Dependencies/glm/gtc/type_ptr.hpp"
#include "../../Utils.h"
#include "../Lights.h"
#include <memory>

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
			Check_GLError();
			if (!AddShader(GL_VERTEX_SHADER, "Shaders\\basicLighting.vert", "basicLightingVert"))
				return false;
			Check_GLError();
			if (!AddShader(GL_FRAGMENT_SHADER, "Shaders\\basicLighting.frag", "basicLightingFrag"))
				return false;
			Check_GLError();
			Finalise();
			Check_GLError();

			//Set uniforms
			worldMatrixLocation = glGetUniformLocation(shaderProgram, "gWorld");
			viewMatrixLocation= glGetUniformLocation(shaderProgram, "gView");
			projectionMatrixLocation = glGetUniformLocation(shaderProgram, "gProjection");
			colourMapLocation = glGetUniformLocation(shaderProgram, "gColorMap");

			if (worldMatrixLocation == INVALID_UNIFORM_LOCATION ||
				viewMatrixLocation == INVALID_UNIFORM_LOCATION ||
				projectionMatrixLocation == INVALID_UNIFORM_LOCATION ||
				colourMapLocation== INVALID_UNIFORM_LOCATION)
				return false;
			
			dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Color");
			dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.AmbientIntensity");
			dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
			dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.DiffuseIntensity");

			if (dirLightLocation.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.Color == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.Direction == INVALID_UNIFORM_LOCATION)
				return false;

			return true;
		}

		void SetWorldMatrix(const mat4& worldMatrix) const
		{
			glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, value_ptr(worldMatrix));
		}

		void SetViewMatrix(const mat4& viewMatrix) const
		{
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
		}

		void SetProjectionMatrix(const mat4& projectionMatrix) const
		{
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));
		}

		void SetTextureUnit(unsigned int TextureUnit) const
		{
			glUniform1i(colourMapLocation, TextureUnit);
		}

		
		void SetDirectionalLight(std::shared_ptr<DirectionalLight> Light) const
		{
			glUniform3f(dirLightLocation.Color, Light->Color.x, Light->Color.y, Light->Color.z);
			glUniform1f(dirLightLocation.AmbientIntensity, Light->AmbientIntensity);
			vec3 Direction = normalize(Light->Direction);
			glUniform3f(dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
			glUniform1f(dirLightLocation.DiffuseIntensity, Light->DiffuseIntensity);
		}

	private:
		GLuint worldMatrixLocation;
		GLuint viewMatrixLocation;
		GLuint projectionMatrixLocation;
		GLuint colourMapLocation;
		
		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
		} dirLightLocation;
	};
}
