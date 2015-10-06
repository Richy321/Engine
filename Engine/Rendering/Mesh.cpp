#include "Mesh.h"
using namespace Rendering;

Mesh::Mesh()
{
}


Mesh::~Mesh()
{
	Destroy();
}

void Mesh::Draw()
{
	//this will be again overridden
}

void Mesh::Update()
{
	//this will be again overridden
}

void Mesh::SetProgram(GLuint program)
{
	this->program = program;
}

GLuint Mesh::GetVao() const
{
	return vao;
}

const std::vector<GLuint>& Mesh::GetVbos() const
{
	return vbos;
}

void Mesh::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(vbos.size(), &vbos[0]);
	vbos.clear();
}