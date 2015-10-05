#pragma once

#include "..\Dependancies\glew\glew.h"
#include "..\Dependancies\freeglut\freeglut.h"
#include <iostream>
#include <fstream>
#include <vector>

namespace Core
{
	class ShaderManager
	{
	private:
		std::string ReadShader(char *filename);
		GLuint CreateShader(GLenum shaderType, std::string source, char *shaderName);
	public:
		ShaderManager();
		~ShaderManager();

		GLuint CreateProgram(char *vertexShaderFilename, char *fragmentShaderFilename);

	};


}
