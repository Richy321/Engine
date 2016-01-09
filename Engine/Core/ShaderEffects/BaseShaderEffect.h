#pragma once
#include <cstdio>
#include <string>
#include <list>
#include <fstream>
#include <vector>

#define GLCheckError() (glGetError() == GL_NO_ERROR)
#define INVALID_UNIFORM_LOCATION 0xffffffff

namespace Core
{
	class BaseShaderEffect
	{
	public:

		BaseShaderEffect()
		{
			shaderProgram = 0;
		}

		virtual ~BaseShaderEffect()
		{
			for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); ++it)
			{
				glDeleteShader(*it);
			}

			if (shaderProgram != 0)
			{
				glDeleteProgram(shaderProgram);
				shaderProgram = 0;
			}
		}

		virtual bool Initialise()
		{
			shaderProgram = glCreateProgram();

			if (shaderProgram == 0) {
				fprintf(stderr, "Error creating shader program\n");
				return false;
			}

			return true;
		}

		bool AddShader(GLenum shaderType, std::string shaderFileName, const std::string& shaderName)
		{
			std::string source = ReadShader(shaderFileName);

			GLuint shader = glCreateShader(shaderType);
			if (shader == 0) {
				fprintf(stderr, "Error creating shader type %d\n", shaderType);
				return false;
			}

			// Save the shader object - will be deleted in the destructor
			m_shaderObjList.push_back(shader);

			const char *shaderCodePtr = source.c_str();
			const int shaderCodeSize = source.size();
			int compileResult = 0;

			glShaderSource(shader, 1, &shaderCodePtr, &shaderCodeSize);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);
			
			if (compileResult == GL_FALSE)
			{
				int infoLogLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<char> shaderLog(infoLogLength);
				glGetShaderInfoLog(shader, infoLogLength, nullptr, &shaderLog[0]);
				std::cout << "ERROR compiling shader: " << shaderName.c_str() << std::endl << &shaderLog[0] << std::endl;
				return false;
			}

			glAttachShader(shaderProgram, shader);
			return true;

		}

		bool Finalise()
		{
			int linkResult = 0;
			glLinkProgram(shaderProgram);
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkResult);

			if (linkResult == GL_FALSE)
			{
				int infoLogLength = 0;
				glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<char> program_log(infoLogLength);
				glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, &program_log[0]);
				std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
				return 0;
			}

			glValidateProgram(shaderProgram);
			glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &linkResult);
			if (linkResult == GL_FALSE) 
			{
				int infoLogLength = 0;
				glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
				std::vector<char> program_log(infoLogLength);
				glGetProgramInfoLog(shaderProgram, infoLogLength, nullptr, &program_log[0]);
				std::cout << "Shader Loader : VALIDATION ERROR" << std::endl << &program_log[0] << std::endl;
				//   return false;
			}


			// Delete the intermediate shader objects that have been added to the program
			for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); ++it)
				glDeleteShader(*it);

			m_shaderObjList.clear();

			return true;
		}

		void Enable() const
		{
			glUseProgram(shaderProgram);
		}

		GLint GetUniformLocation(const char* pUniformName)
		{
			GLuint Location = glGetUniformLocation(shaderProgram, pUniformName);

			if (Location == INVALID_UNIFORM_LOCATION) {
				fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
			}

			return Location;
		}

		GLint GetProgramParam(GLint param)
		{
			GLint ret;
			glGetProgramiv(shaderProgram, param, &ret);
			return ret;
		}

	protected:
		typedef std::list<GLuint> ShaderObjList;
		ShaderObjList m_shaderObjList;
		GLuint shaderProgram;
		std::string ReadShader(const std::string& filename)
		{
			std::string shaderCode;
			std::ifstream file(filename, std::ios::in);

			if (!file.good())
			{
				std::cout << "Can't read file" << filename.c_str() << std::endl;
				std::terminate();
			}

			file.seekg(0, std::ios::end);
			shaderCode.resize((unsigned int)file.tellg());

			file.seekg(0, std::ios::beg);
			file.read(&shaderCode[0], shaderCode.size());
			file.close();

			return shaderCode;
		}
	};
}