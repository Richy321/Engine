#pragma once
#include "../Dependencies/glm/glm.hpp"
#include "../Dependencies/glm/gtc/matrix_transform.hpp"
#include "Components/MeshComponent.h"

#include "../Dependencies/assimp/Importer.hpp"      // C++ importer interface
#include "../Dependencies/assimp/scene.h"           // Output data structure
#include "../Dependencies/assimp/postprocess.h"     // Post processing fla
#include "../Texture.h"
#include "../Dependencies/freeImage/FreeImage.h"
#include "../IAssetManager.h"

using namespace glm;

namespace Core
{
	class AssetManager : public IAssetManager
	{
	public:
		static AssetManager &GetInstance()
		{
			static AssetManager Instance;
			return Instance;
		}
		std::vector<MeshComponent> meshComponents;
		std::map<std::string, GLuint> textureIDs;

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

		static void recursiveImport(const struct aiScene* scene, const struct aiNode* nd, std::shared_ptr<MeshNode> meshNode, std::vector<std::string> materialIds)
		{
			CopyaiMat(&nd->mTransformation, meshNode->toParent);
			
			// import all node meshes
			for (size_t n = 0; n < nd->mNumMeshes; n++)
			{
				std::shared_ptr<Mesh> subMesh = std::make_shared<Mesh>(&GetInstance());

				const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
				
				uint materialIndex = mesh->mMaterialIndex;
				if (materialIndex < materialIds.size() && materialIndex >= 0)
					subMesh->materialID = materialIds[materialIndex];

				vec4 colour(1, 0, 0, 1);

				for (size_t t = 0; t < mesh->mNumFaces; ++t)
				{
					const struct aiFace* face = &mesh->mFaces[t];

					//detect mode from number of points in first face
					if (t == 0)
					{
						switch (face->mNumIndices)
						{
						case 1: subMesh->mode = GL_POINTS; break;
						case 2: subMesh->mode = GL_LINES; break;
						case 3: subMesh->mode = GL_TRIANGLES; break;
						default: subMesh->mode = GL_POLYGON; break;
						}
					}

					for (size_t i = 0; i < face->mNumIndices; i++) // go through all vertices in face
					{
						int vertexIndex = face->mIndices[i];	// get group index for current index
						subMesh->indices.push_back(vertexIndex);
					}
				}

				for (size_t v = 0; v < mesh->mNumVertices; v++)
				{
					subMesh->positions.push_back(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));

					if (mesh->mNormals != nullptr)
						subMesh->normals.push_back(vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z));
					else
						subMesh->normals.push_back(vec3(0.0f, 0.0f, 1.0f));
					if (mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
						subMesh->uvs.push_back(vec2(mesh->mTextureCoords[0][v].x, /*1 - */mesh->mTextureCoords[0][v].y)); //mTextureCoords[channel][vertex]
					else
						subMesh->uvs.push_back(vec2(0.0f, 0.0f));
				}

				subMesh->BuildAndBindVertexPositionNormalTexturedBuffer();
				subMesh->SetProgram(Managers::ShaderManager::GetShader("basicLighting"));


				meshNode->AddMesh(subMesh);
			}

			// import all children
 			for (size_t n = 0; n < nd->mNumChildren; ++n)
			{
				std::shared_ptr<MeshNode> childNode = std::make_shared<MeshNode>();
				meshNode->AddChild(childNode);
				recursiveImport(scene, nd->mChildren[n], childNode, materialIds);
			}
		}

		std::unique_ptr<MeshComponent> LoadMeshFromFile(const std::string& filename)
		{
			// Create a logger instance 
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality);
			std::unique_ptr<MeshComponent> rval = std::make_unique<MeshComponent>(std::weak_ptr<IGameObject>());
			
			if(!scene)
			{
				printf("AssImp Failed: %s", importer.GetErrorString());
				assert(scene);
			}
			std::vector<std::string> materialIds = ImportTexturesFromModel(scene, filename, rval);

			//todo - load textures from scene
			//todo - detect vertex format based on Has* functions
			//todo - proper asset caching
			//todo - extend asset manager to textures (devIL ? )
			recursiveImport(scene, scene->mRootNode, rval->rootMeshNode, materialIds);

			return rval;
		}

		std::vector<std::string> ImportTexturesFromModel(const aiScene *scene, const std::string& filename, std::unique_ptr<MeshComponent> &meshComponent)
		{
			std::vector<std::string> materialIds;

			// Extract the directory part from the file name
			std::string::size_type SlashIndex = filename.find_last_of("/");
			std::string Dir;

			if (SlashIndex == std::string::npos) {
				Dir = ".";
			}
			else if (SlashIndex == 0) {
				Dir = "/";
			}
			else {
				Dir = filename.substr(0, SlashIndex);
			}

			for (size_t i = 0; i < scene->mNumMaterials; i++)
			{
				const aiMaterial* pMaterial = scene->mMaterials[i];
				if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
				{
					aiString Path;
					if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
					{
						std::string fullPath = Dir + "/" + Path.data;

						std::string texID = fullPath;

						if (!LoadTextureFromFile(fullPath, texID, GL_BGRA, GL_RGBA, 0, 0))
						{
							printf("Error loading texture '%s'\n", fullPath.c_str());
							UnloadTexture(texID);
						}
						else 
						{
							printf("Loaded texture '%s'\n", fullPath.c_str());
						}
						materialIds.push_back(texID);
					}
				}
			}
			return materialIds;
		}

		bool LoadTextureFromFile(const std::string &filename, const std::string &texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
		{
			//image format
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
			//pointer to the image, once loaded
			FIBITMAP *dib(nullptr);
			//pointer to the image data
			BYTE* bits(nullptr);
			//image width and height
			unsigned int width(0), height(0);
			//OpenGL's image ID to map to
			GLuint gl_texID;

			//check the file signature and deduce its format
			fif = FreeImage_GetFileType(filename.c_str(), 0);
			//if still unknown, try to guess the file format from the file extension
			if (fif == FIF_UNKNOWN)
				fif = FreeImage_GetFIFFromFilename(filename.c_str());
			//if still unkown, return failure
			if (fif == FIF_UNKNOWN)
				return false;

			//check that the plugin has reading capabilities and load the file
			if (FreeImage_FIFSupportsReading(fif))
				dib = FreeImage_Load(fif, filename.c_str());
			//if the image failed to load, return failure
			if (!dib)
				return false;

			dib = FreeImage_ConvertTo32Bits(dib);

			//retrieve the image data
			bits = FreeImage_GetBits(dib);
			//get the image width and height
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);
			//if this somehow one of these failed (they shouldn't), return failure
			if ((bits == nullptr) || (width == 0) || (height == 0))
				return false;

			//if this texture ID is in use, unload the current texture
			if (textureIDs.find(texID) != textureIDs.end())
				glDeleteTextures(1, &(textureIDs[texID]));

			//generate an OpenGL texture ID for this texture
			glGenTextures(1, &gl_texID);

			//store the texture ID mapping
			textureIDs[texID] = gl_texID;

			//bind to the new texture ID
			glBindTexture(GL_TEXTURE_2D, gl_texID);

			//store the texture data for OpenGL use
			glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height,
				border, image_format, GL_UNSIGNED_BYTE, bits);

			//Free FreeImage's copy of the data
			FreeImage_Unload(dib);

			return true;
		}

		bool UnloadTexture(const std::string texID)
		{
			//if this texture ID mapped, unload it's texture, and remove it from the map
			if (textureIDs.find(texID) != textureIDs.end())
			{
				glDeleteTextures(1, &(textureIDs[texID]));
				textureIDs.erase(texID);
			}
			//otherwise, unload failed
			else
				return false;

			return true;
		}

		bool BindTexture(const std::string texID, GLenum TextureUnit)
		{
			//if this texture ID mapped, bind it's texture as current
			if (textureIDs.find(texID) != textureIDs.end())
			{
				glActiveTexture(TextureUnit);
				glBindTexture(GL_TEXTURE_2D, textureIDs[texID]);
				return true;
			}
			//otherwise, binding failed
			return false;
		}

		void UnloadAllTextures()
		{
			//start at the begginning of the texture map
			std::map<std::string, GLuint>::iterator i = textureIDs.begin();

			//Unload the textures untill the end of the texture map is found
			while (i != textureIDs.end())
				UnloadTexture(i->first);

			//clear the texture map
			textureIDs.clear();
		}

		GLuint GetOGLTextureID(const std::string texID)
		{
			if (textureIDs.find(texID) != textureIDs.end())
				return textureIDs[texID];
			return NULL;
		}

		std::shared_ptr<Mesh> CreateTrianglePrimitive()
		{
			std::shared_ptr<Mesh> triangleMesh = std::make_shared<Mesh>(&GetInstance());

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

		std::shared_ptr<Mesh> CreateQuadPrimitive()
		{
			std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>(&GetInstance());

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

		std::shared_ptr<Mesh> CreateCubePrimitive(float size = 1.0f)
		{
			return CreateBoxPrimitive(size, size, size);
		}

		std::shared_ptr<Mesh> CreateBoxPrimitive(float width, float height, float depth)
		{
			std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(&GetInstance());

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

		std::unique_ptr<MeshComponent> CreateTrianglePrimitiveMeshComponent()
		{
			std::unique_ptr<MeshComponent> triangleMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			triangleMesh->AddRootMesh(CreateTrianglePrimitive());
			return triangleMesh;
		}

		std::unique_ptr<MeshComponent> CreateCubePrimitiveMeshComponent(float size = 1.0f)
		{
			std::unique_ptr<MeshComponent> cubeMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			cubeMesh->AddRootMesh(CreateCubePrimitive(size));
			return cubeMesh;
		}

		std::unique_ptr<MeshComponent> CreateBoxPrimitiveMeshComponent(float width, float height, float depth)
		{
			std::unique_ptr<MeshComponent> boxMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			boxMesh->AddRootMesh(CreateBoxPrimitive(width, height, depth));
			return boxMesh;
		}
	};
}