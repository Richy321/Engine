#pragma once

#include "..\Dependencies\glew\glew.h"
#include "..\Dependencies\freeglut\freeglut.h"
#include <iostream>
#include <map>
#include "../Core/ShaderEffects/LitTexturedMeshEffect.h"
#include "../Core/ShaderEffects/ColouredMeshEffect.h"

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
		std::shared_ptr<Core::LitTexturedMeshEffect> litTexturedMeshEffect;
		std::shared_ptr<Core::ColouredMeshEffect> colouredMeshEffect;


		bool Initialise()
		{
			bool success = true;
			litTexturedMeshEffect = std::make_shared<Core::LitTexturedMeshEffect>();
			success &= litTexturedMeshEffect->Initialise();

			colouredMeshEffect = std::make_shared<Core::ColouredMeshEffect>();
			success &= colouredMeshEffect->Initialise();

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
