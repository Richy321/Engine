#pragma once
#include "Interfaces/IRenderableComponent.h"
#include <vector>
#include <iostream>
#include "../../Dependencies/glew/glew.h"
#include "../../Dependencies/freeglut/freeglut.h"
#include "../../Rendering/VertexFormat.h"

namespace Core
{
	class MeshComponent : public IRenderableComponent
	{
	protected:
		GLuint vao;
		GLuint program;
		std::vector<GLuint> vbos;

		int startIndex = 0;
		int vertexCount = 0;
		GLenum mode = GL_TRIANGLES;
	public:

		MeshComponent()
		{
		}

		~MeshComponent()
		{
			Destroy();
		}
		

		void Render() override
		{
			glUseProgram(program);
			glBindVertexArray(vao);
			glDrawArrays(mode, startIndex, vertexCount);
		}

		void Update() override
		{

		}

		void Destroy() override
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(vbos.size(), &vbos[0]);
			vbos.clear();
		}

		void SetProgram(GLuint program) { this->program = program; }

		GLuint GetVao() const {	return vao; }
		void SetVao(GLuint newVao) 
		{
			glDeleteVertexArrays(1, &vao);
			vao = newVao;
		}

		//void SetVertices(GLsizeiptr dataSize, const void* data)
		//{
		//	GLuint vao;
		//	GLuint vbo;
		//	glGenVertexArrays(1, &vao);
		//	glBindVertexArray(vao);

		//	glGenBuffers(1, &vbo);
		//	glBindBuffer(GL_ARRAY_BUFFER, vbo);            //here we have 4
		//	glBufferData(GL_ARRAY_BUFFER, dataSize, &data, GL_STATIC_DRAW);

		//	glEnableVertexAttribArray(0);
		//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		//		sizeof(VertexFormat), (void*)0);

		//	glEnableVertexAttribArray(1);
		//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		//		sizeof(VertexFormat),
		//		(void*)(offsetof(VertexFormat, VertexFormat::color)));
		//	
		//	glBindVertexArray(0);
		//	this->vao = vao;
		//	this->vbos.push_back(vbo);
		//}

		const std::vector<GLuint>& GetVbos() const { return vbos; }
		void SetVbo(GLuint newVbos)
		{
			glDeleteBuffers(vbos.size(), &vbos[0]);
			vbos.clear();

			vbos.push_back(newVbos);
		}
		void SetVbos(std::vector<GLuint> newVbos)
		{
			vbos.clear();
			for each (GLuint vbo in newVbos)
			{
				vbos.push_back(vbo);
			}
		}

		static MeshComponent* CreateTrianglePrimitive()
		{
			MeshComponent* triangleMesh = new MeshComponent();

			GLuint vao;
			GLuint vbo;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			std::vector<VertexFormat> vertices;
			vertices.push_back(VertexFormat(vec3(0.25, -0.25, 0.0),
				vec4(1, 0, 0, 1)));
			vertices.push_back(VertexFormat(vec3(-0.25, -0.25, 0.0),
				vec4(0, 1, 0, 1)));
			vertices.push_back(VertexFormat(vec3(0.25, 0.25, 0.0),
				vec4(0, 0, 1, 1)));

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * 3, &vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
				(void*)0);
			glEnableVertexAttribArray(1);
			// you can use offsetof to get the offset of an attribute
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
				(void*)(offsetof(VertexFormat, VertexFormat::color)));
			glBindVertexArray(0);

			//here we assign the values
			triangleMesh->SetProgram(Managers::ShaderManager::GetShader("colorShader"));
			triangleMesh->vertexCount = vertices.size();
			triangleMesh->vao = vao;
			triangleMesh->vbos.push_back(vbo);

			return triangleMesh;
		}

		static MeshComponent* CreateQuadPrimitive()
		{
			MeshComponent* quadMesh = new MeshComponent();

			GLuint vao;
			GLuint vbo;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			std::vector<VertexFormat> vertices;
			vertices.push_back(VertexFormat(vec3(-0.25, 0.5, 0.0),//pos
				vec4(1, 0, 0, 1)));   //color
			vertices.push_back(VertexFormat(vec3(-0.25, 0.75, 0.0),//pos
				vec4(0, 0, 0, 1)));   //color
			vertices.push_back(VertexFormat(vec3(0.25, 0.5, 0.0),  //pos
				vec4(0, 1, 0, 1)));   //color				   //4th vertex
			vertices.push_back(VertexFormat(vec3(0.25, 0.75, 0.0),//pos
				vec4(0, 0, 1, 1)));   //color
										   //nothing different from Triangle model
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);            //here we have 4
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * 4, &vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
				sizeof(VertexFormat), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
				(void*)(offsetof(VertexFormat, VertexFormat::color)));
			glBindVertexArray(0);


			quadMesh->SetProgram(Managers::ShaderManager::GetShader("colorShader"));
			quadMesh->mode = GL_TRIANGLE_STRIP;
			quadMesh->vertexCount = vertices.size();
			quadMesh->vao = vao;
			quadMesh->vbos.push_back(vbo);

			return quadMesh;
		}

		static MeshComponent* CreateCubePrimitive()
		{
			MeshComponent* cubeMesh = new MeshComponent();

			GLuint vao;
			GLuint vbo;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			std::vector<VertexFormat> vertices;
			//vertices for the front face of the cube
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, 1.0),
				vec4(0.0, 0.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, -1.0, 1.0),
				vec4(1.0, 0.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, 1.0),
				vec4(1.0, 1.0, 1.0, 1.0)));

			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, 1.0),
				vec4(0.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, 1.0),
				vec4(1.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, 1.0),
				vec4(0.0, 0.0, 1.0, 1.0)));

			//vertices for the right face of the cube
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, 1.0),
				vec4(1.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, -1.0),
				vec4(1.0, 1.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, -1.0, -1.0),
				vec4(1.0, 0.0, 0.0, 1.0)));

			vertices.push_back(VertexFormat(vec3(1.0, 1.0, 1.0),
				vec4(1.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, -1.0, -1.0),
				vec4(1.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, -1.0, 1.0),
				vec4(1.0, 0.0, 1.0, 1.0)));

			//vertices for the back face of the cube
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, -1.0),
				vec4(0.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, -1.0, -1.0),
				vec4(1.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, -1.0),
				vec4(1.0, 1.0, 0.0, 1.0)));

			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, -1.0),
				vec4(0.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, -1.0),
				vec4(1.0, 1.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, -1.0),
				vec4(0.0, 1.0, 0.0, 1.0)));

			//vertices for the left face of the cube
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, -1.0),
				vec4(0.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, 1.0),
				vec4(0.0, 0.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, 1.0),
				vec4(0.0, 1.0, 1.0, 1.0)));

			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, -1.0),
				vec4(0.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, 1.0),
				vec4(0.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, -1.0),
				vec4(0.0, 1.0, 0.0, 1.0)));

			//vertices for the upper face of the cube
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, 1.0),
				vec4(1.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, 1.0),
				vec4(0.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, -1.0),
				vec4(1.0, 1.0, 0.0, 1.0)));

			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, 1.0),
				vec4(0.0, 1.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, 1.0, -1.0),
				vec4(1.0, 1.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, 1.0, -1.0),
				vec4(0.0, 1.0, 0.0, 1.0)));

			//vertices for the bottom face of the cube
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, -1.0),
				vec4(0.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, -1.0, -1.0),
				vec4(1.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, 1.0),
				vec4(0.0, 0.0, 1.0, 1.0)));

			vertices.push_back(VertexFormat(vec3(1.0, -1.0, -1.0),
				vec4(1.0, 0.0, 0.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(-1.0, -1.0, 1.0),
				vec4(0.0, 0.0, 1.0, 1.0)));
			vertices.push_back(VertexFormat(vec3(1.0, -1.0, 1.0),
				vec4(1.0, 0.0, 1.0, 1.0)));

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, VertexFormat::color));
			glBindVertexArray(0);

			cubeMesh->SetProgram(Managers::ShaderManager::GetShader("colorShader"));
			cubeMesh->mode = GL_TRIANGLE_STRIP;
			cubeMesh->vertexCount = vertices.size();
			cubeMesh->vao = vao;
			cubeMesh->vbos.push_back(vbo);

			return cubeMesh;
		}
	};
}
