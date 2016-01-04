#pragma once
#include <vector>
#include "VertexFormat.h"
#include "../Dependencies/glm/detail/type_vec3.hpp"
#include "../Dependencies/glew/glew.h"
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

		std::vector<vec3> positions;
		std::vector<unsigned int> indices;
		std::vector<vec3> normals;
		std::vector<vec2> uvs;
		std::vector<unsigned int> materialIndex;
		std::vector<vec4> colours;

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

		void Render(std::shared_ptr<Camera> mainCamera, const mat4 &toWorld) const
		{
			//todo - convert to shared view and proj uniforms function
			//Set uniforms
			static GLuint gWorldLocation = glGetUniformLocation(program, "gWorld");
			static GLuint gViewUniform = glGetUniformLocation(program, "gView");
			static GLuint gProjectionUniform = glGetUniformLocation(program, "gProjection");

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
				glDrawArrays(mode, 0, positions.size());
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

		void BuildAndBindVertexPositionNormalTexturedBuffer()
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &ebo);
			
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			std::vector<VertexPositionNormalTextured> vertices;
			for (size_t v = 0; v < positions.size(); v++)
				vertices.push_back(VertexPositionNormalTextured(positions[v], normals[v], uvs[v]));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionNormalTextured) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormalTextured), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormalTextured), (void*)offsetof(VertexPositionNormalTextured, VertexPositionNormalTextured::normal));

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormalTextured), (void*)offsetof(VertexPositionNormalTextured, VertexPositionNormalTextured::uv));

			glBindVertexArray(0);

			this->vbos.push_back(vbo);
		}

		void BuildAndBindVertexPositionColorBuffer()
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
			std::vector<VertexPositionColour> vertices;
			for (size_t i = 0; i < positions.size(); i++)
			{
				vertices.push_back(VertexPositionColour(positions[i], colours[i]));
			}

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionColour) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColour), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColour), (void*)(offsetof(VertexPositionColour, VertexPositionColour::color)));
			glBindVertexArray(0);

			vbos.push_back(vbo);
		}


		static std::shared_ptr<Mesh> CreateTrianglePrimitive()
		{
			std::shared_ptr<Mesh> triangleMesh = std::make_shared<Mesh>();

			triangleMesh->positions.push_back(vec3(0.25, -0.25, 0.0));
			triangleMesh->colours.push_back(vec4(1, 0, 0, 1));
			triangleMesh->positions.push_back(vec3(-0.25, -0.25, 0.0));
			triangleMesh->colours.push_back(vec4(0, 1, 0, 1));
			triangleMesh->positions.push_back(vec3(0.25, 0.25, 0.0));
			triangleMesh->colours.push_back(vec4(0, 0, 1, 1));

			triangleMesh->BuildAndBindVertexPositionColorBuffer();
			triangleMesh->SetProgram(Managers::ShaderManager::GetShader("basicColor"));

			return triangleMesh;
		}

		static std::shared_ptr<Mesh> CreateQuadPrimitive()
		{
			std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>();

			quadMesh->positions.push_back(vec3(-0.25, 0.5, 0.0));
			quadMesh->colours.push_back(vec4(1, 0, 0, 1));
			quadMesh->positions.push_back(vec3(-0.25, 0.75, 0.0));
			quadMesh->colours.push_back(vec4(0, 0, 0, 1));
			quadMesh->positions.push_back(vec3(0.25, 0.5, 0.0));
			quadMesh->colours.push_back(vec4(0, 1, 0, 1));
			quadMesh->positions.push_back(vec3(0.25, 0.75, 0.0));
			quadMesh->colours.push_back(vec4(0, 0, 1, 1));
									  
			quadMesh->BuildAndBindVertexPositionColorBuffer();
			quadMesh->SetProgram(Managers::ShaderManager::GetShader("basicColor"));
			quadMesh->mode = GL_TRIANGLE_STRIP;

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
			
			//front face of the cube
			cubeMesh->positions.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, -height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));

			cubeMesh->positions.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));

			//right face of the cube
			cubeMesh->positions.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));

			cubeMesh->positions.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, -height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 1.0, 1.0));

			//back face of the cube
			cubeMesh->positions.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));

			cubeMesh->positions.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 0.0, 1.0));

			//left face of the cube
			cubeMesh->positions.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));

			cubeMesh->positions.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 0.0, 1.0));

			//upper face of the cube
			cubeMesh->positions.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));

			cubeMesh->positions.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 0.0, 1.0));

			//bottom face of the cube
			cubeMesh->positions.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));

			cubeMesh->positions.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->positions.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			cubeMesh->positions.push_back(vec3(width, -height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 1.0, 1.0));


			cubeMesh->BuildAndBindVertexPositionColorBuffer();
			cubeMesh->SetProgram(Managers::ShaderManager::GetShader("basicColor"));
			cubeMesh->mode = GL_TRIANGLE_STRIP;
			cubeMesh->vbos.push_back(vbo);

			return cubeMesh;
		}
	};
}
