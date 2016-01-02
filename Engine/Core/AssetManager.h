#pragma once
#include "../Dependencies/glm/glm.hpp"
#include "../Dependencies/glm/gtc/matrix_transform.hpp"
#include "Components/MeshComponent.h"

#include "../Dependencies/assimp/Importer.hpp"      // C++ importer interface
#include "../Dependencies/assimp/scene.h"           // Output data structure
#include "../Dependencies/assimp/postprocess.h"     // Post processing fla

using namespace glm;

namespace Core
{
	class AssetManager
	{
	public:
		static AssetManager GetInstance()
		{
			static AssetManager Instance;
			return Instance;
		}

		std::vector<MeshComponent> meshComponents;
		AssetManager()
		{
		}

		~AssetManager()
		{
		}


		static void recursiveImport(const struct aiScene* scene, const struct aiNode* nd, std::unique_ptr<MeshComponent>& meshComponent)
		{
			// import all node meshes
			for (size_t n = 0; n < nd->mNumMeshes; n++)
			{
				const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

				GLuint vao;
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);
				meshComponent->vertices.clear();

				GLuint elementbuffer;
				meshComponent->indices.clear();
				glGenBuffers(1, &elementbuffer);

				GLuint vbo;
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);

				vec4 colour(1, 0, 0, 1);

				for (size_t t = 0; t < mesh->mNumFaces; ++t)
				{
					const struct aiFace* face = &mesh->mFaces[t];
					GLenum face_mode;

					switch (face->mNumIndices)
					{
						case 1: meshComponent->mode = GL_POINTS; break;
						case 2: meshComponent->mode = GL_LINES; break;
						case 3: meshComponent->mode = GL_TRIANGLES; break;
						default: meshComponent->mode = GL_POLYGON; break;
					}

					for (size_t i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
					{
						int vertexIndex = face->mIndices[i];	// get group index for current index
						meshComponent->indices.push_back(vertexIndex);
						/*
						if (mesh->mColors[0] != NULL)
							Color4f(&mesh->mColors[0][vertexIndex]);
						if (mesh->mNormals != NULL)

							if (mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
							{
								glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
							}
							

						glNormal3fv(&mesh->mNormals[vertexIndex].x);
						*/
					}
				}


				for (size_t v = 0; v < mesh->mNumVertices; v++)
				{
					meshComponent->vertices.push_back(VertexFormat(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z),//pos
						colour));   //color
				}

				glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * meshComponent->vertices.size(), &meshComponent->vertices[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, VertexFormat::color));
				glBindVertexArray(0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshComponent->indices.size() * sizeof(unsigned int), &meshComponent->indices[0], GL_STATIC_DRAW);

				meshComponent->SetProgram(Managers::ShaderManager::GetShader("colorShader"));
				//meshComponent->mode = GL_TRIANGLE_STRIP;
				meshComponent->vertexCount = meshComponent->vertices.size();
				meshComponent->vao = vao;
				meshComponent->vbos.push_back(vbo);
			}

			// import all children
			for (size_t n = 0; n < nd->mNumChildren; ++n)
			{
				recursiveImport(scene, nd->mChildren[n], meshComponent);
			}
		}

		std::unique_ptr<MeshComponent> LoadMeshFromFile(const std::string& filename) const
		{
			// Create a logger instance 
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filename,
				aiProcessPreset_TargetRealtime_Quality);
			if(!scene)
			{
				printf("AssImp Failed: %s", importer.GetErrorString());
				assert(scene);
			}

			std::unique_ptr<MeshComponent> rval = std::make_unique<MeshComponent>(std::weak_ptr<IGameObject>());

			//todo - load textures from scene
			//todo - detect vertex format based on Has* functions

			recursiveImport(scene, scene->mRootNode, rval);

			return rval;
		}
	};
}