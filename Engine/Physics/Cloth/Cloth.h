#pragma once
#include "ClothParticle.h"
#include "../../Core/Camera.h"
namespace Physics
{
	namespace Cloth
	{
		class Cloth
		{
		public:

			std::vector<std::shared_ptr<ClothParticle>> particles;
			std::unique_ptr<Mesh> mesh;

			float damping = 0.01f;
			int constraintRestrictions = 15;
			vec2 size;
			vec2 particleColRowCount;

			Cloth(vec2 size, ivec2 particleColRowCount)
			{
				this->size = size;
				this->particleColRowCount = particleColRowCount;

				mesh = std::make_unique<Mesh>(&AssetManager::GetInstance());

				//initialise particles
				particles.resize(particleColRowCount.x * particleColRowCount.y);
				for (int x = 0; x < particleColRowCount.x; x++)
				{
					for (int y = 0; y < particleColRowCount.y; y++)
					{
						vec3 pos = vec3(size.x * (x / (float)particleColRowCount.x),
							0.0f,
							size.y * (y / (float)particleColRowCount.y));

						particles[y * static_cast<int>(particleColRowCount.x) + x] = std::make_shared<ClothParticle>(pos);
					}
				}

				GenerateVertices();
				GenerateIndicies();
				GenerateUVs();
				GenerateNormals();

				mesh->BuildAndBindVertexPositionNormalTexturedBuffer();
				mesh->mode = GL_TRIANGLES;
				mesh->renderType = Mesh::LitTextured;
			}

			~Cloth()
			{
			}


			void GenerateVertices()
			{
				mesh->vertices.clear();
				for(auto i : particles )
					mesh->vertices.push_back(i->pos);
			}

			void GenerateIndicies()
			{
				//indices
				for (int x = 0; x < particleColRowCount.x - 1; x++)
				{
					for (int y = 0; y < particleColRowCount.y - 1; y++)
					{
						int index1= y * static_cast<int>(particleColRowCount.x) + x;
						int index2 = (y + 1) * static_cast<int>(particleColRowCount.x) + x;
						int index3 = y * static_cast<int>(particleColRowCount.x) + x + 1;
						int index4 = (y+1) * static_cast<int>(particleColRowCount.x) + x + 1;

						mesh->indices.push_back(index1);
						mesh->indices.push_back(index2);
						mesh->indices.push_back(index3);

						mesh->indices.push_back(index3);
						mesh->indices.push_back(index2);
						mesh->indices.push_back(index4);
					}
				}
			}

			void GenerateUVs()
			{
				//uvs
				for (int x = 0; x < particleColRowCount.x; x++)
				{
					for (int y = 0; y < particleColRowCount.y; y++)
					{
						mesh->uvs.push_back(vec2(static_cast<float>(x) / (particleColRowCount.x - 1), static_cast<float>(y) / (particleColRowCount.y - 1)));
					}
				}
			}
				
			void GenerateNormals()
			{
				for (size_t i = 0; i < mesh->indices.size(); i+=3)
				{
					mesh->normals.push_back(Utils::CalculateNormal(
							mesh->vertices[mesh->indices[i]],
							mesh->vertices[mesh->indices[i+1]],
							mesh->vertices[mesh->indices[i + 2]]));
				}
			}

			void Render(std::shared_ptr<Core::Camera> mainCamera, const mat4 &toWorld)
			{
				mesh->Render(mainCamera, toWorld);
			}

			void OnFixedTimeStep(float deltaTime)
			{
				for(auto i : particles)
					i->OnFixedTimeStep(deltaTime, damping);

				//update mesh positions and normals and rebind
				GenerateVertices();
				GenerateNormals();
				mesh->BuildAndBindVertexPositionNormalTexturedBuffer();
			}
		};
	}
}