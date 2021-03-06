#pragma once
#include "../Dependencies/glm/glm.hpp"
#include "../Dependencies/assimp/Importer.hpp"      // C++ importer interface
#include "../Dependencies/assimp/scene.h"           // Output data structure
#include "../Dependencies/assimp/postprocess.h"     // Post processing fla
#include "../Dependencies/freeImage/FreeImage.h"
#include "IAssetManager.h"
#include "Components/MeshComponent.h"
#include "IcoSphereCreator.h"
#include "../Dependencies/glm/gtc/random.hpp"

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

		static void CopyaiMat(const aiMatrix4x4 *from, glm::mat4 &to) 
		{
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
					subMesh->vertices.push_back(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));

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
				//subMesh->SetProgram(Managers::ShaderManager::GetShader("basicLighting"));

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
			//todo - Check for embedded textures in model if possible?
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
							//Todo - Handle missing textures from models
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

		bool UnloadTexture(const std::string texID) override
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

		bool BindTexture(const std::string texID, GLenum TextureUnit) override
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

		void UnloadAllTextures() override
		{
			//start at the begginning of the texture map
			std::map<std::string, GLuint>::iterator i = textureIDs.begin();

			//Unload the textures untill the end of the texture map is found
			while (i != textureIDs.end())
				UnloadTexture(i->first);

			//clear the texture map
			textureIDs.clear();
		}

		GLuint GetOGLTextureID(const std::string texID) override
		{
			if (textureIDs.find(texID) != textureIDs.end())
				return textureIDs[texID];
			return NULL;
		}

		std::shared_ptr<Mesh> CreateTrianglePrimitive() override
		{
			std::shared_ptr<Mesh> triangleMesh = std::make_shared<Mesh>(&GetInstance());

			triangleMesh->vertices.push_back(vec3(0.25, -0.25, 0.0));
			triangleMesh->colours.push_back(vec4(1, 0, 0, 1));
			triangleMesh->vertices.push_back(vec3(-0.25, -0.25, 0.0));
			triangleMesh->colours.push_back(vec4(0, 1, 0, 1));
			triangleMesh->vertices.push_back(vec3(0.25, 0.25, 0.0));
			triangleMesh->colours.push_back(vec4(0, 0, 1, 1));

			triangleMesh->BuildAndBindVertexPositionColorBuffer();
			triangleMesh->renderType = Mesh::Coloured;

			return triangleMesh;
		}

		std::shared_ptr<Mesh> CreateQuadPrimitive(float width, float height) const override
		{
			std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>(&GetInstance());

			float halfWidth = width / 2;
			float halfHeight = height / 2;

			quadMesh->vertices.push_back(vec3(-halfWidth, -halfHeight, 0.0));
			quadMesh->colours.push_back(vec4(1, 0, 0, 1));
			quadMesh->normals.push_back(vec3(0.0f, -1.0f, 0.0f));
			
			quadMesh->vertices.push_back(vec3(-halfWidth, halfHeight, 0.0));
			quadMesh->colours.push_back(vec4(0, 0, 0, 1));
			quadMesh->normals.push_back(vec3(-1.0f, 0.0f, 0.0f));

			quadMesh->vertices.push_back(vec3(halfWidth, -halfHeight, 0.0));
			quadMesh->colours.push_back(vec4(0, 1, 0, 1));
			quadMesh->normals.push_back(vec3(1.0f, 0.0f, 0.0f));

			quadMesh->vertices.push_back(vec3(halfWidth, halfHeight, 0.0));
			quadMesh->colours.push_back(vec4(0, 0, 1, 1));
			quadMesh->normals.push_back(vec3(0.0f, 1.0f, 0.0f));

			quadMesh->BuildAndBindVertexPositionColorBuffer();
			quadMesh->SetProgram(Managers::ShaderManager::GetShader("basicColor"));
			quadMesh->mode = GL_TRIANGLE_STRIP;
			quadMesh->renderType = Mesh::Coloured;

			return quadMesh;
		}

		std::shared_ptr<Mesh> CreateCirclePrimitive(float radius, int fragments) const
		{
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(&GetInstance());
			const float PI = 3.1415926f;

			float increment = 2.0f * PI / fragments;

			mesh->vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
			mesh->colours.push_back(Colours_RGBA::Green);

			for (float currAngle = 0.0f; currAngle < 2.0f * PI; currAngle += increment)
			{
				mesh->vertices.push_back(vec3(radius * cos(currAngle), radius * sin(currAngle), 0));
				if (currAngle == 0.0f || currAngle == 2.0f * PI)
					mesh->colours.push_back(Colours_RGBA::HotPink);
				else
					mesh->colours.push_back(Colours_RGBA::Green);
			}
			float lastAngle = 2.0f * PI + increment;
			mesh->vertices.push_back(vec3(radius * cos(lastAngle), radius * sin(lastAngle), 0));
			mesh->colours.push_back(Colours_RGBA::HotPink);

			mesh->vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
			mesh->colours.push_back(Colours_RGBA::HotPink);

			mesh->BuildAndBindVertexPositionColorBuffer();
			mesh->SetProgram(Managers::ShaderManager::GetShader("basicColor"));
			mesh->mode = GL_TRIANGLE_FAN;
			mesh->renderType = Mesh::Coloured;

			return mesh;
		}

		std::shared_ptr<Mesh> CreateRandomPolygonPrimitive(int minVertexCount, int maxVertexCount, float sizeRange) const
		{
			const int MaxPolyVertexCount = 64;
			const float EPSILON = 0.0001f;

			assert(minVertexCount > 2 && maxVertexCount > 2);

			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(&GetInstance());

			std::vector<vec2> verts;

			uint vertexCount = linearRand(minVertexCount, maxVertexCount);
			for (size_t i = 0; i < vertexCount; i++)
			{
				verts.push_back(vec2(linearRand(-sizeRange, sizeRange), linearRand(-sizeRange, sizeRange)));
			}

			// Find the right most point on the hull
			int32 rightMost = 0;
			float highestXCoord = verts[0].x;
			for (size_t i = 1; i < vertexCount; ++i)
			{
				float x = verts[i].x;
				if (x > highestXCoord)
				{
					highestXCoord = x;
					rightMost = i;
				}

				// If matching x then take farthest negative y
				else if (x == highestXCoord)
					if (verts[i].y < verts[rightMost].y)
						rightMost = i;
			}


			int32 hull[MaxPolyVertexCount];
			int32 outCount = 0;
			int32 indexHull = rightMost;

			for (;;)
			{
				hull[outCount] = indexHull;

				// Search for next index that wraps around the hull
				// by computing cross products to find the most counter-clockwise
				// vertex in the set, given the previous hull index
				int32 nextHullIndex = 0;
				for (int32 i = 1; i < (int32)vertexCount; ++i)
				{
					// Skip if same coordinate as we need three unique
					// points in the set to perform a cross product
					if (nextHullIndex == indexHull)
					{
						nextHullIndex = i;
						continue;
					}

					// Cross every set of three unique vertices
					// Record each counter clockwise third vertex and add
					// to the output hull
					// See : http://www.oocities.org/pcgpe/math2d.html
					vec2 e1 = verts[nextHullIndex] - verts[hull[outCount]];
					vec2 e2 = verts[i] - verts[hull[outCount]];
					float c = Utils::CrossVec2(e1, e2);
					if (c < 0.0f)
						nextHullIndex = i;

					// Cross product is zero then e vectors are on same line
					// therefor want to record vertex farthest along that line
					if (c == 0.0f && Utils::Len2Vec2(e2) > Utils::Len2Vec2(e1))
						nextHullIndex = i;
				}

				++outCount;
				indexHull = nextHullIndex;

				// Conclude algorithm upon wrap-around
				if (nextHullIndex == rightMost)
				{
					break;
				}
			}

			for (size_t i = 0; i < verts.size(); i++)
				mesh->vertices.push_back(vec3(verts[hull[i]], 0.0f));

			// Compute face normals
			for (uint32 i1 = 0; i1 < mesh->vertices.size(); ++i1)
			{
				uint32 i2 = i1 + 1 < mesh->vertices.size() ? i1 + 1 : 0;
				vec3 face = mesh->vertices[i2] - mesh->vertices[i1];

				// Ensure no zero-length edges, because that's bad
				assert(Utils::Len2Vec2(vec2(face.x, face.y)) > EPSILON * EPSILON);

				// Calculate normal with 2D cross product between vector and scalar
				mesh->normals.push_back(vec3(face.y, -face.x, 0.0f));
				mesh->normals[i1] = normalize(mesh->normals[i1]);
			}

			mesh->BuildAndBindVertexPositionColorBuffer();
			mesh->SetProgram(Managers::ShaderManager::GetShader("basicColor"));
			mesh->mode  = GL_LINE_LOOP;
			mesh->renderType = Mesh::Coloured;

			return mesh;
		}

		std::shared_ptr<Mesh> CreateQuadPrimitiveAdv(float width, float depth) const
		{
			const vec3 upNormal = vec3(0.0, 1.0f, 0.0f);

			std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>(&GetInstance());
			
			quadMesh->vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
			quadMesh->normals.push_back(upNormal);
			quadMesh->uvs.push_back(vec2(0.0f, 0.0f));

			quadMesh->vertices.push_back(vec3(0.0f, 0.0f, depth));
			quadMesh->normals.push_back(upNormal);
			quadMesh->uvs.push_back(vec2(0.0f, 1.0f));

			quadMesh->vertices.push_back(vec3(width, 0.0f, depth));
			quadMesh->normals.push_back(upNormal);
			quadMesh->uvs.push_back(vec2(1.0f, 1.0f));

			quadMesh->vertices.push_back(vec3(width, 0.0f, 0.0f));
			quadMesh->normals.push_back(upNormal);
			quadMesh->uvs.push_back(vec2(1.0f, 0.0f));

			quadMesh->indices.push_back(0);
			quadMesh->indices.push_back(1);
			quadMesh->indices.push_back(3);

			quadMesh->indices.push_back(3);
			quadMesh->indices.push_back(1);
			quadMesh->indices.push_back(2);

			quadMesh->BuildAndBindVertexPositionNormalTexturedBuffer();
			quadMesh->mode = GL_TRIANGLES;
			quadMesh->renderType = Mesh::LitTextured;

			return quadMesh;
		}

		std::shared_ptr<Mesh> CreateCubePrimitive(float size = 1.0f) override
		{
			return CreateBoxPrimitive(size, size, size);
		}

		std::shared_ptr<Mesh> CreateBoxPrimitive(float width, float height, float depth) const
		{
			std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(&GetInstance());

			//front face of the cube
			cubeMesh->vertices.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, -height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));

			cubeMesh->vertices.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));

			//right face of the cube
			cubeMesh->vertices.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));

			cubeMesh->vertices.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, -height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 1.0, 1.0));

			//back face of the cube
			cubeMesh->vertices.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));

			cubeMesh->vertices.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 0.0, 1.0));

			//left face of the cube
			cubeMesh->vertices.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));

			cubeMesh->vertices.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 0.0, 1.0));

			//upper face of the cube
			cubeMesh->vertices.push_back(vec3(width, height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));

			cubeMesh->vertices.push_back(vec3(-width, height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 1.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 1.0, 0.0, 1.0));

			//bottom face of the cube
			cubeMesh->vertices.push_back(vec3(-width, -height, -depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));

			cubeMesh->vertices.push_back(vec3(width, -height, -depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 0.0, 1.0));
			cubeMesh->vertices.push_back(vec3(-width, -height, depth));
			cubeMesh->colours.push_back(vec4(0.0, 0.0, 1.0, 1.0));
			cubeMesh->vertices.push_back(vec3(width, -height, depth));
			cubeMesh->colours.push_back(vec4(1.0, 0.0, 1.0, 1.0));

			cubeMesh->BuildAndBindVertexPositionColorBuffer();
			cubeMesh->mode = GL_TRIANGLE_STRIP;
			cubeMesh->renderType = Mesh::Coloured;

			return cubeMesh;
		}

		std::shared_ptr<Mesh> CreateIcospherePrimitive(int recursionLevel, float radius)
		{
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(&GetInstance());

			IcoSphereCreator::Create(mesh, recursionLevel, radius);

			mesh->BuildAndBindVertexPositionNormalTexturedBuffer();
			mesh->mode = GL_TRIANGLES;
			mesh->renderType = Mesh::LitTextured;

			return mesh;
		}

		std::shared_ptr<Mesh> CreateSphereCustom(float radius, unsigned int rings, unsigned int sectors)
		{
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(&GetInstance());
			float const R = 1.0f / (float)(rings);
			float const S = 1.0f / (float)(sectors);

			for (size_t r = 0; r <= rings; ++r)
			{
				for (size_t s = 0; s <= sectors; ++s)
				{
					float const y = sin(-M_PI_2 + M_PI * r * R);
					float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
					float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

					vec3 position(x, y, z);

					mesh->uvs.push_back(vec2(s*S, r*R));
					mesh->vertices.push_back(position * radius);
					mesh->normals.push_back(normalize(position));

					//generate indices
					int curRow = r * sectors;
					int nextRow = (r + 1) * sectors;

					mesh->indices.push_back(curRow + s);
					mesh->indices.push_back(nextRow + s);
					mesh->indices.push_back(nextRow + (s + 1));

					mesh->indices.push_back(curRow + s);
					mesh->indices.push_back(nextRow + (s + 1));
					mesh->indices.push_back(curRow + (s + 1));
				}
			}
			mesh->BuildAndBindVertexPositionNormalTexturedBuffer();
			mesh->mode = GL_TRIANGLES;
			mesh->renderType = Mesh::LitTextured;
			return mesh;

		}

		std::unique_ptr<MeshComponent> CreateCirclePrimitiveMeshComponent(float radius, int fragments)
		{
			std::unique_ptr<MeshComponent> mesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			mesh->AddRootMesh(CreateCirclePrimitive(radius, fragments));
			return mesh;
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

		std::unique_ptr<MeshComponent> CreateBoxPrimitiveMeshComponent(float width, float height, float depth) const
		{
			std::unique_ptr<MeshComponent> boxMesh = std::make_unique<MeshComponent>(std::weak_ptr<GameObject>());
			boxMesh->AddRootMesh(CreateBoxPrimitive(width, height, depth));
			return boxMesh;
		}

		std::shared_ptr<MeshComponent> CreateQuadPrimitiveMeshComponent(float width, float depth, std::string texture = "") const
		{
			std::shared_ptr<MeshComponent> quadMesh = std::make_shared<MeshComponent>(std::weak_ptr<GameObject>());
			quadMesh->AddRootMesh(CreateQuadPrimitiveAdv(width, depth));
			quadMesh->rootMeshNode->meshes[0]->materialID = texture;
			return quadMesh;
		}

		//std::shared_ptr<MeshComponent> CreateGridPrimitiveMeshComponent(float width, float depth, int widthSegments, int depthSegments, std::string texture = "") const
		//{
		//	std::shared_ptr<MeshComponent> quadMesh = std::make_shared<MeshComponent>(std::weak_ptr<GameObject>());
		//	quadMesh->AddRootMesh(CreateQuadPrimitiveAdv(width, depth));
		//	quadMesh->rootMeshNode->meshes[0]->materialID = texture;
		//	return quadMesh;
		//}

		std::shared_ptr<MeshComponent> CreateSimpleQuadPrimitiveMeshComponent(float width, float depth) const
		{
			std::shared_ptr<MeshComponent> quadMesh = std::make_shared<MeshComponent>(std::weak_ptr<GameObject>());
			quadMesh->AddRootMesh(CreateQuadPrimitive(width, depth));
			return quadMesh;
		}

		std::shared_ptr<MeshComponent> CreateRandomPolygonPrimitiveMeshComponent(int polyCountMin, int polyCountMax, float size) const
		{
			std::shared_ptr<MeshComponent> mesh = std::make_shared<MeshComponent>(std::weak_ptr<GameObject>());
			mesh->AddRootMesh(CreateRandomPolygonPrimitive(polyCountMin, polyCountMax, size));
			return mesh;
		}

		std::shared_ptr<MeshComponent> CreateIcospherePrimitiveMeshComponent(int recursionLevel = 1, float radius = 1.0f)
		{
			std::shared_ptr<MeshComponent> mesh = std::make_shared<MeshComponent>(std::weak_ptr<GameObject>());
			mesh->AddRootMesh(CreateIcospherePrimitive(recursionLevel, radius));
			return mesh;
		}

		std::shared_ptr<MeshComponent> CreateSpherePrimitiveMeshComponent(float radius, unsigned int rings, unsigned int sectors)
		{
			std::shared_ptr<MeshComponent> mesh = std::make_shared<MeshComponent>(std::weak_ptr<GameObject>());
			mesh->AddRootMesh(CreateSphereCustom(radius, rings, sectors));
			return mesh;
		}

	protected:

		AssetManager()
		{
		}

		~AssetManager()
		{
		}
	};
}