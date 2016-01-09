#pragma once

#include "..\Dependencies\glew\glew.h"
#include "..\Dependencies\freeglut\freeglut.h"
#include <iostream>
#include <map>
#include "../Core/ShaderEffects/LitTexturedMeshEffect.h"

namespace Managers
{
	class ShaderManager
	{
	private:
		ShaderManager();
		std::string ReadShader(const std::string& fileName);
		GLuint CreateShader(GLenum shaderType, std::string source, const std::string& shaderName);
		static std::map<std::string, GLuint> programs;
	public:
		Core::LitTexturedMeshEffect* litTexturedMeshEffect;

		bool Initialise()
		{
			bool success = true;
			litTexturedMeshEffect = new Core::LitTexturedMeshEffect();
			success &= litTexturedMeshEffect->Initialise();

			return success;
		}
		static ShaderManager &GetInstance()
		{
			static ShaderManager* instance = new ShaderManager();
			return *instance;
		}
		
		~ShaderManager();

		GLuint CreateProgram(const std::string& shaderName, 
							const std::string& vertexShaderFilename, 
						const std::string &fragmentShaderFilename);

		static const GLuint GetShader(const std::string& shaderName);
	};


}
