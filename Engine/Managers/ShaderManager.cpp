#include "ShaderManager.h"
#include <assert.h> 

using namespace Managers;

std::map<std::string, GLuint> ShaderManager::programs;

std::string ShaderManager::ReadShader(const std::string& filename)
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

GLuint ShaderManager::CreateShader(GLenum shaderType, std::string source, const std::string& shaderName)
{
	int compileResult = 0;

	GLuint shader = glCreateShader(shaderType);

	const char *shaderCodePtr = source.c_str();
	const int shaderCodeSize = source.size();

	glShaderSource(shader, 1, &shaderCodePtr, &shaderCodeSize);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> shaderLog(infoLogLength);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &shaderLog[0]);
		std::cout << "ERROR compiling shader: " << shaderName.c_str() << std::endl << &shaderLog[0] << std::endl;
		return 0;
	}
	return shader;
}

ShaderManager::ShaderManager()
{
}


ShaderManager::~ShaderManager()
{
	std::map<std::string, GLuint>::iterator i;
	for (i = programs.begin(); i != programs.end(); ++i)
	{
		GLuint pr = i->second;
		glDeleteProgram(pr);
	}
	programs.clear();
}

GLuint ShaderManager::CreateProgram(const std::string& shaderName, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
{
	std::string vertexShaderCode = ReadShader(vertexShaderFilename);
	std::string fragmentShaderCode = ReadShader(fragmentShaderFilename);

	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderCode, "vertex shader");
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderCode, "fragment shader");

	int linkResult = 0;
	//create the program handle, attach the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);

	if (linkResult == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> program_log(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, NULL, &program_log[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
		return 0;
	}

	assert(programs.find(shaderName) == programs.end());
	programs[shaderName] = program;

	return program;
}

const GLuint ShaderManager::GetShader(const std::string& shaderName)
{
	return programs.at(shaderName);
}