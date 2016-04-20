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
#include "Colours.h"
#include <limits>
#include "../Physics/BoundingSphere.h"
#include "../Physics/AABB.h"

namespace Core
{
	class Mesh
	{
	public:

		enum RenderType
		{
			Coloured,
			LitTextured
			
		} renderType = LitTextured;

		GLuint vao;
		GLuint ebo;
		GLuint program;
		std::vector<GLuint> vbos;

		std::vector<vec3> vertices;
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
			vec3 cameraPos = vec3(mainCamera->GetWorldTransform()[3]);

			switch(renderType)
			{
			case Coloured:
				Check_GLError();
				Managers::ShaderManager::GetInstance().colouredMeshEffect->Enable();
				Managers::ShaderManager::GetInstance().colouredMeshEffect->SetWorldMatrix(toWorld);
				Managers::ShaderManager::GetInstance().colouredMeshEffect->SetViewMatrix(mainCamera->view);
				Managers::ShaderManager::GetInstance().colouredMeshEffect->SetProjectionMatrix(mainCamera->projection);
				break;
			case LitTextured: 
				Managers::ShaderManager::GetInstance().litTexturedMeshEffect->Enable();
				Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetWorldMatrix(toWorld);
				Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetViewMatrix(mainCamera->view);
				Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetProjectionMatrix(mainCamera->projection);
				Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetTextureUnit(0);

				Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetEyeWorldPos(cameraPos);


				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			default: break;
			}

			glBindVertexArray(vao);
			Check_GLError();

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
				glDrawArrays(mode, 0, vertices.size());

			glBindVertexArray(0);
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

			std::vector<VertexPositionNormalTextured> verts;
			for (size_t v = 0; v < vertices.size(); v++)
			{
				vec3 normal = vec3(0.0f, 1.0f, 0.0f);
				if (v < normals.size())
					normal = normals[v];

				vec2 uv = vec2(0.0f, 1.0f);
				if (v < uvs.size())
					uv = uvs[v];

				verts.push_back(VertexPositionNormalTextured(vertices[v], normal, uv));
			}
				

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionNormalTextured) * verts.size(), &verts[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormalTextured), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormalTextured), (void*)offsetof(VertexPositionNormalTextured, VertexPositionNormalTextured::normal));

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPositionNormalTextured), (void*)offsetof(VertexPositionNormalTextured, VertexPositionNormalTextured::uv));

			glBindVertexArray(0);

			vbos.push_back(vbo);
		}

		void BuildAndBindVertexPositionColorBuffer()
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
			std::vector<VertexPositionColour> verts;
			for (size_t i = 0; i < vertices.size(); i++)
			{
				vec4 colour = i < colours.size() ? colours[i] : Colours_RGBA::HotPink;

				verts.push_back(VertexPositionColour(vertices[i], colour));
			}

			if (indices.size() > 0)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			}

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionColour) * verts.size(), &verts[0], GL_STATIC_DRAW);
			
			
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColour), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColour), (void*)(offsetof(VertexPositionColour, VertexPositionColour::color)));
			
			glBindVertexArray(0);

			vbos.push_back(vbo);
		}

		void BuildAndBindIndexBuffer()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		void BuildAndBindVertexPositionBuffer()
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			std::vector<VertexPosition> verts;
			for (size_t i = 0; i < vertices.size(); i++)
			{
				verts.push_back(VertexPosition(vertices[i]));
			}

			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPositionColour) * verts.size(), &verts[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionColour), (void*)0);

			vbos.push_back(vbo);
		}

		AABB ComputeAABB()
		{
			float minX, minY, minZ;
			float maxX, maxY, maxZ;
			minX = minY = minZ = std::numeric_limits<float>::max();
			maxX = maxY = maxZ = std::numeric_limits<float>::min();

			for(vec3 var : vertices)
			{
				minX = min(minX, var.x);
				minY = min(minY, var.y);
				minZ = min(minZ, var.z);

				maxX = max(maxX, var.x);
				maxY = max(maxY, var.y);
				maxZ = max(maxZ, var.z);
			}

			return AABB(vec3(minX, minY, minZ), vec3(maxX, maxY, maxZ));
		}

		BoundingSphere ComputeBoundSphere()
		{
			vec3 center;
			float maxRadius = std::numeric_limits<float>::min();
			for (vec3 var : vertices)
			{
				center += var;
				maxRadius = max(maxRadius, fabs(static_cast<float>(var.length())));
			}
			center /= vertices.size();

			return BoundingSphere(maxRadius);
		}
		
	};
}
