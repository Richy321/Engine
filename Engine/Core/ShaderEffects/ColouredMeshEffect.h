#pragma once
#include "BaseShaderEffect.h"
#include "../../Dependencies/glm/gtc/type_ptr.hpp"
#include "../../Core/Utils.h"
#include <memory>

using namespace glm;

namespace Core
{
	class ColouredMeshEffect : public BaseShaderEffect
	{
	public:

		ColouredMeshEffect()
		{
		}

		bool Initialise() override
		{
			if (!BaseShaderEffect::Initialise())
				return false;
			Check_GLError();
			if (!AddShader(GL_VERTEX_SHADER, "Shaders\\basicColor.vert", "basicColor"))
				return false;
			Check_GLError();
			if (!AddShader(GL_FRAGMENT_SHADER, "Shaders\\basicColor.frag", "basicColor"))
				return false;
			Check_GLError();
			Finalise();
			Check_GLError();

			//Set uniforms
			worldMatrixLocation = glGetUniformLocation(shaderProgram, "gWorld");
			viewMatrixLocation = glGetUniformLocation(shaderProgram, "gView");
			projectionMatrixLocation = glGetUniformLocation(shaderProgram, "gProjection");

			if (worldMatrixLocation == INVALID_UNIFORM_LOCATION ||
				viewMatrixLocation == INVALID_UNIFORM_LOCATION ||
				projectionMatrixLocation == INVALID_UNIFORM_LOCATION)
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

	private:
		GLuint worldMatrixLocation;
		GLuint viewMatrixLocation;
		GLuint projectionMatrixLocation;
	};
}