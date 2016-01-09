#pragma once
#include <vector>
#include "VertexFormat.h"
#include "../Dependencies/glm/detail/type_vec3.hpp"
#include "../Dependencies/glew/glew.h"
#include "../Managers/ShaderManager.h"
#include "../Dependencies/glm/gtc/type_ptr.hpp"
#include <memory>
#include "Camera.h"
#include "IAssetManager.h"

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
		std::string materialID;
		std::vector<vec4> colours;

		GLenum mode = GL_TRIANGLES;
		IAssetManager* assetManager;

		Mesh(IAssetManager* assMan) : assetManager(assMan)
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
			//todo - investigate moving view/prj into common
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetWorldMatrix(toWorld);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetViewMatrix(mainCamera->view);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetProjectionMatrix(mainCamera->projection);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetTextureUnit(0);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->Enable();
			glBindVertexArray(vao);
			
			if(!materialID.empty())
				assetManager->BindTexture(materialID, GL_TEXTURE0);

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
	};
}
