#pragma once
#include <vector>
#include "VertexFormat.h"
#include "../Dependencies/glm/detail/type_vec3.hpp"
#include "../Dependencies/glew/glew.h"
#include "../Dependencies/glm/mat4x2.hpp"
#include "../Managers/ShaderManager.h"
#include "../Dependencies/glm/gtc/type_ptr.hpp"
#include <memory>
#include "Camera.h"

namespace Core
{
	class Mesh
	{
	public:
		GLuint vao;
		GLuint ebo;
		GLuint program;
		std::vector<GLuint> vbos;

		std::vector<VertexFormat> vertices;
		std::vector<unsigned int> indices;
		std::vector<vec3> normals;
		std::vector<vec2> uvs;
		std::vector<unsigned int> materialIndex;

		GLenum mode = GL_TRIANGLES;

		Mesh()
		{
		}

		~Mesh()
		{
			Destroy();
		}


		void Update()
		{
			
		}

		void Destroy()
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(vbos.size(), &vbos[0]);
			vbos.clear();
		}

		void Render(std::shared_ptr<Camera> mainCamera, const mat4 &toWorld)
		{
			//todo - convert to shared view and proj uniforms
			static GLuint gWorldLocation = glGetUniformLocation(Managers::ShaderManager::GetInstance().GetShader("colorShader"), "gWorld");
			static GLuint gViewUniform = glGetUniformLocation(Managers::ShaderManager::GetInstance().GetShader("colorShader"), "gView");
			static GLuint gProjectionUniform = glGetUniformLocation(Managers::ShaderManager::GetInstance().GetShader("colorShader"), "gProjection");

			glm::mat4 wp = mainCamera->worldToProjection * toWorld;

			glUniformMatrix4fv(gWorldLocation, 1, GL_FALSE, glm::value_ptr(toWorld));
			glUniformMatrix4fv(gViewUniform, 1, GL_FALSE, glm::value_ptr(mainCamera->view));
			glUniformMatrix4fv(gProjectionUniform, 1, GL_FALSE, glm::value_ptr(mainCamera->projection));

			glUseProgram(program);
			glBindVertexArray(vao);
			if (indices.size() > 0)
				glDrawElements(
					mode,      // mode
					indices.size(),    // count
					GL_UNSIGNED_INT,   // type
					(void*)0           // element array buffer offset
					);
			else
				glDrawArrays(mode, 0, vertices.size());
		}

		void SetProgram(GLuint program) { this->program = program; }

		GLuint GetVao() const { return vao; }
		void SetVao(GLuint newVao)
		{
			glDeleteVertexArrays(1, &vao);
			vao = newVao;
		}

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

		static std::shared_ptr<Mesh> CreateTrianglePrimitive()
		{
			std::shared_ptr<Mesh> triangleMesh = std::make_shared<Mesh>();

			GLuint vao;
			GLuint vbo;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			triangleMesh->vertices.push_back(VertexFormat(vec3(0.25, -0.25, 0.0),
				vec4(1, 0, 0, 1)));
			triangleMesh->vertices.push_back(VertexFormat(vec3(-0.25, -0.25, 0.0),
				vec4(0, 1, 0, 1)));
			triangleMesh->vertices.push_back(VertexFormat(vec3(0.25, 0.25, 0.0),
				vec4(0, 0, 1, 1)));

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * triangleMesh->vertices.size(), &triangleMesh->vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(offsetof(VertexFormat, VertexFormat::color)));
			glBindVertexArray(0);

			//here we assign the values
			triangleMesh->SetProgram(Managers::ShaderManager::GetShader("colorShader"));
			triangleMesh->vao = vao;
			triangleMesh->vbos.push_back(vbo);
			return triangleMesh;
		}

		static std::shared_ptr<Mesh> CreateQuadPrimitive()
		{
			std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>();

			GLuint vao;
			GLuint vbo;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			quadMesh->vertices.push_back(VertexFormat(vec3(-0.25, 0.5, 0.0),//pos
				vec4(1, 0, 0, 1)));   //color
			quadMesh->vertices.push_back(VertexFormat(vec3(-0.25, 0.75, 0.0),//pos
				vec4(0, 0, 0, 1)));   //color
			quadMesh->vertices.push_back(VertexFormat(vec3(0.25, 0.5, 0.0),  //pos
				vec4(0, 1, 0, 1)));   //color				   //4th vertex
			quadMesh->vertices.push_back(VertexFormat(vec3(0.25, 0.75, 0.0),//pos
				vec4(0, 0, 1, 1)));   //color
									  //nothing different from Triangle model
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);            //here we have 4
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * 4, &quadMesh->vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
				sizeof(VertexFormat), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
				(void*)(offsetof(VertexFormat, VertexFormat::color)));
			glBindVertexArray(0);


			quadMesh->SetProgram(Managers::ShaderManager::GetShader("colorShader"));
			quadMesh->mode = GL_TRIANGLE_STRIP;
			quadMesh->vao = vao;
			quadMesh->vbos.push_back(vbo);

			return quadMesh;
		}

		static std::shared_ptr<Mesh> CreateCubePrimitive(float size = 1.0f)
		{
			return CreateBoxPrimitive(size, size, size);
		}

		static std::shared_ptr<Mesh> CreateBoxPrimitive(float width, float height, float depth)
		{
			std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>();

			GLuint vao;
			GLuint vbo;

			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			//vertices for the front face of the cube
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, depth),
				vec4(0.0, 0.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, depth),
				vec4(1.0, 0.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, depth),
				vec4(1.0, 1.0, 1.0, 1.0)));

			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, depth),
				vec4(0.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, depth),
				vec4(1.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, depth),
				vec4(0.0, 0.0, 1.0, 1.0)));

			//vertices for the right face of the cube
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, depth),
				vec4(1.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, -depth),
				vec4(1.0, 1.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, -depth),
				vec4(1.0, 0.0, 0.0, 1.0)));

			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, depth),
				vec4(1.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, -depth),
				vec4(1.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, depth),
				vec4(1.0, 0.0, 1.0, 1.0)));

			//vertices for the back face of the cube
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, -depth),
				vec4(0.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, -depth),
				vec4(1.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, -depth),
				vec4(1.0, 1.0, 0.0, 1.0)));

			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, -depth),
				vec4(0.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, -depth),
				vec4(1.0, 1.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, -depth),
				vec4(0.0, 1.0, 0.0, 1.0)));

			//vertices for the left face of the cube
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, -depth),
				vec4(0.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, depth),
				vec4(0.0, 0.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, depth),
				vec4(0.0, 1.0, 1.0, 1.0)));

			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, -depth),
				vec4(0.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, depth),
				vec4(0.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, -depth),
				vec4(0.0, 1.0, 0.0, 1.0)));

			//vertices for the upper face of the cube
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, depth),
				vec4(1.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, depth),
				vec4(0.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, -depth),
				vec4(1.0, 1.0, 0.0, 1.0)));

			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, depth),
				vec4(0.0, 1.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, height, -depth),
				vec4(1.0, 1.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, height, -depth),
				vec4(0.0, 1.0, 0.0, 1.0)));

			//vertices for the bottom face of the cube
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, -depth),
				vec4(0.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, -depth),
				vec4(1.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, depth),
				vec4(0.0, 0.0, 1.0, 1.0)));

			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, -depth),
				vec4(1.0, 0.0, 0.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(-width, -height, depth),
				vec4(0.0, 0.0, 1.0, 1.0)));
			cubeMesh->vertices.push_back(VertexFormat(vec3(width, -height, depth),
				vec4(1.0, 0.0, 1.0, 1.0)));

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * cubeMesh->vertices.size(), &cubeMesh->vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, VertexFormat::color));
			glBindVertexArray(0);

			cubeMesh->SetProgram(Managers::ShaderManager::GetShader("colorShader"));
			cubeMesh->mode = GL_TRIANGLE_STRIP;
			cubeMesh->vao = vao;
			cubeMesh->vbos.push_back(vbo);

			return cubeMesh;
		}
	};
}
