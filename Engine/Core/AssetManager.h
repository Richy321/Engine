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

		static void CopyaiMat(const aiMatrix4x4 *from, glm::mat4 &to) {
			to[0][0] = from->a1; to[1][0] = from->a2;
			to[2][0] = from->a3; to[3][0] = from->a4;
			to[0][1] = from->b1; to[1][1] = from->b2;
			to[2][1] = from->b3; to[3][1] = from->b4;
			to[0][2] = from->c1; to[1][2] = from->c2;
			to[2][2] = from->c3; to[3][2] = from->c4;
			to[0][3] = from->d1; to[1][3] = from->d2;
			to[2][3] = from->d3; to[3][3] = from->d4;
		}

		static void recursiveImport(const struct aiScene* scene, const struct aiNode* nd, std::shared_ptr<MeshNode> meshNode)
		{
			CopyaiMat(&nd->mTransformation, meshNode->toParent);

			// import all node meshes
			for (size_t n = 0; n < nd->mNumMeshes; n++)
			{
				std::shared_ptr<Mesh> subMesh = std::make_shared<Mesh>();

				const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

				GLuint vao;
				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);
				subMesh->vertices.clear();

				GLuint elementbuffer;
				glGenBuffers(1, &elementbuffer);
				subMesh->indices.clear();

				GLuint vbo;
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);

				vec4 colour(1, 0, 0, 1);

				for (size_t t = 0; t < mesh->mNumFaces; ++t)
				{
					const struct aiFace* face = &mesh->mFaces[t];
					switch (face->mNumIndices)
					{
						case 1: subMesh->mode = GL_POINTS; break;
						case 2: subMesh->mode = GL_LINES; break;
						case 3: subMesh->mode = GL_TRIANGLES; break;
						default: subMesh->mode = GL_POLYGON; break;
					}

					for (size_t i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
					{
						int vertexIndex = face->mIndices[i];	// get group index for current index
						subMesh->indices.push_back(vertexIndex);
						
						if (mesh->mNormals != NULL)
							subMesh->normals.push_back(vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));

						if (mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
						{
							subMesh->uvs.push_back(vec2(mesh->mTextureCoords[0][vertexIndex].x, /*1 - */mesh->mTextureCoords[0][vertexIndex].y)); //mTextureCoords[channel][vertex]
						}
					}
				}


				for (size_t v = 0; v < mesh->mNumVertices; v++)
				{
					subMesh->vertices.push_back(VertexFormat(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z),//pos
						colour));   //color
				}

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh->indices.size() * sizeof(unsigned int), &subMesh->indices[0], GL_STATIC_DRAW);

				glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * subMesh->vertices.size(), &subMesh->vertices[0], GL_STATIC_DRAW);
				
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
				
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)offsetof(VertexFormat, VertexFormat::color));
				
				
				glBindVertexArray(0);



				subMesh->SetProgram(Managers::ShaderManager::GetShader("basicColor"));
				subMesh->vao = vao;
				subMesh->ebo = elementbuffer;
				subMesh->vbos.push_back(vbo);

				meshNode->AddMesh(subMesh);
			}

			// import all children
 			for (size_t n = 0; n < nd->mNumChildren; ++n)
			{
				std::shared_ptr<MeshNode> childNode = std::make_shared<MeshNode>();
				meshNode->AddChild(childNode);
				recursiveImport(scene, nd->mChildren[n], childNode);
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
			//todo - proper asset caching
			//todo - extend asset manager to textures (devIL ? )
			recursiveImport(scene, scene->mRootNode, rval->rootMeshNode);

			return rval;
		}
	};
}