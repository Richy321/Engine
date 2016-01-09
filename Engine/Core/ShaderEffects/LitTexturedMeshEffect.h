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

		static const unsigned int MAX_POINT_LIGHTS = 2;

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


			matSpecularIntensityLocation = GetUniformLocation("gMatSpecularIntensity");
			matSpecularPowerLocation = GetUniformLocation("gSpecularPower");
			eyeWorldPosLocation = GetUniformLocation("gEyeWorldPos");

			if (matSpecularIntensityLocation == INVALID_UNIFORM_LOCATION ||
				matSpecularPowerLocation == INVALID_UNIFORM_LOCATION ||
				eyeWorldPosLocation == INVALID_UNIFORM_LOCATION)
				return false;

			dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Base.Color");
			dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.Base.AmbientIntensity");
			dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.Base.DiffuseIntensity");
			dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");

			if (dirLightLocation.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.Color == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
				dirLightLocation.Direction == INVALID_UNIFORM_LOCATION)
				return false;

			numPointLightsLocation = GetUniformLocation("gNumPointLights");

			for (unsigned int i = 0; i < MAX_POINT_LIGHTS; i++)
			{
				char Name[128];
				memset(Name, 0, sizeof(Name));
				_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Base.Color", i);
				m_pointLightsLocation[i].Color = GetUniformLocation(Name);

				_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Base.AmbientIntensity", i);
				m_pointLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

				_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Position", i);
				m_pointLightsLocation[i].Position = GetUniformLocation(Name);

				_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Base.DiffuseIntensity", i);
				m_pointLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

				_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Atten.Constant", i);
				m_pointLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

				_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Atten.Linear", i);
				m_pointLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

				_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Atten.Exp", i);
				m_pointLightsLocation[i].Atten.Exp = GetUniformLocation(Name);

				if (m_pointLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
					m_pointLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
					m_pointLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
					m_pointLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
					m_pointLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
					m_pointLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
					m_pointLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
					return false;
				}
			}

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

		void SetEyeWorldPos(const vec3& EyeWorldPos) const
		{
			glUniform3f(eyeWorldPosLocation, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
		}

		void SetMatSpecularIntensity(float Intensity) const
		{
			glUniform1f(matSpecularIntensityLocation, Intensity);
		}

		void SetMatSpecularPower(float Power) const
		{
			glUniform1f(matSpecularPowerLocation, Power);
		}
		
		void SetDirectionalLight(std::shared_ptr<DirectionalLight> Light) const
		{
			glUniform3f(dirLightLocation.Color, Light->Color.x, Light->Color.y, Light->Color.z);
			glUniform1f(dirLightLocation.AmbientIntensity, Light->AmbientIntensity);
			vec3 Direction = normalize(Light->Direction);
			glUniform3f(dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
			glUniform1f(dirLightLocation.DiffuseIntensity, Light->DiffuseIntensity);
		}

		void SetPointLights(std::vector<std::shared_ptr<PointLight>> pointLights) const
		{
			glUniform1i(numPointLightsLocation, pointLights.size());

			for (unsigned int i = 0; i < pointLights.size(); i++) {
				glUniform3f(m_pointLightsLocation[i].Color, pointLights[i]->Color.x, pointLights[i]->Color.y, pointLights[i]->Color.z);
				glUniform1f(m_pointLightsLocation[i].AmbientIntensity, pointLights[i]->AmbientIntensity);
				glUniform1f(m_pointLightsLocation[i].DiffuseIntensity, pointLights[i]->DiffuseIntensity);
				glUniform3f(m_pointLightsLocation[i].Position, pointLights[i]->Position.x, pointLights[i]->Position.y, pointLights[i]->Position.z);
				glUniform1f(m_pointLightsLocation[i].Atten.Constant, pointLights[i]->Attenuation.Constant);
				glUniform1f(m_pointLightsLocation[i].Atten.Linear, pointLights[i]->Attenuation.Linear);
				glUniform1f(m_pointLightsLocation[i].Atten.Exp, pointLights[i]->Attenuation.Exp);
			}
		}


	private:
		GLuint worldMatrixLocation;
		GLuint viewMatrixLocation;
		GLuint projectionMatrixLocation;
		GLuint colourMapLocation;
		
		GLuint eyeWorldPosLocation;
		GLuint matSpecularIntensityLocation;
		GLuint matSpecularPowerLocation;

		GLuint numPointLightsLocation;

		struct 
		{
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
		} dirLightLocation;

		struct 
		{
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint DiffuseIntensity;
			GLuint Position;
			struct
			{
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		} m_pointLightsLocation[MAX_POINT_LIGHTS];
	};
}
