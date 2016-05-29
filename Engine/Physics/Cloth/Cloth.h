#pragma once

#include <ppl.h>

#include "ClothParticle.h"
#include "ClothConstraint.h"
#include "../../Core/Camera.h"

#include "../../Dependencies/glm/gtx/norm.hpp"

namespace Physics
{
	namespace Cloth
	{
		class Cloth
		{
		private:
			std::shared_ptr<ClothParticle>& GetParticle(int x, int y)
			{
				return particles[y * particleColRowCount.x + x];
			}

			void AddWindForcesForTriangle(std::shared_ptr<ClothParticle> p1, std::shared_ptr<ClothParticle> p2, std::shared_ptr<ClothParticle> p3, const vec3 direction)
			{
				vec3 normalMag = Utils::CalculateNormalMagnitude(p1->pos, p2->pos, p3->pos);
				vec3 d = normalize(normalMag);
				vec3 force = normalMag * (dot(d, direction));

				p1->AddForce(force);
				p2->AddForce(force);
				p3->AddForce(force);
			}

		public:

			std::vector<std::shared_ptr<ClothParticle>> particles;
			std::vector<std::shared_ptr<ClothConstraint>> constraints;
			std::unique_ptr<Mesh> mesh;

			float damping = 0.01f;
			uint constraintIterations = 15;
			vec2 size;
			ivec2 particleColRowCount;

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

				GenerateConstraints();
				printf("Cloth Constructor done");
			}

			~Cloth()
			{
			}

			void GenerateVertices()
			{
				mesh->vertices.clear();
				for(auto& i : particles )
					mesh->vertices.push_back(i->pos);
			}

			void GenerateIndicies()
			{
				//indices
				for (int x = 0; x < particleColRowCount.x - 1; x++)
				{
					for (int y = 0; y < particleColRowCount.y - 1; y++)
					{
						int index1= y * particleColRowCount.x + x;
						int index2 = (y + 1) * particleColRowCount.x + x;
						int index3 = y * particleColRowCount.x + x + 1;
						int index4 = (y+1) * particleColRowCount.x + x + 1;

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
				mesh->normals.clear();
				for (size_t i = 0; i < mesh->indices.size(); i+=3)
				{
					mesh->normals.push_back(Utils::CalculateNormal(
							mesh->vertices[mesh->indices[i]],
							mesh->vertices[mesh->indices[i+1]],
							mesh->vertices[mesh->indices[i + 2]]));
				}
			}

			void GenerateConstraints()
			{
				for (int x = 0; x < particleColRowCount.x; x++)
				{
					for (int y = 0; y < particleColRowCount.y; y++)
					{
						//immediate 'structural' and 'shear' neighbour constraints
						if (x < particleColRowCount.x - 1)
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x, y), GetParticle(x + 1, y)));
						if(y < particleColRowCount.y - 1)
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x, y), GetParticle(x, y + 1)));

						if(x < particleColRowCount.x - 1 && y < particleColRowCount.y - 1) 
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x, y), GetParticle(x + 1, y + 1)));
						if (x < particleColRowCount.x - 1 && y < particleColRowCount.y - 1)
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x + 1, y), GetParticle(x, y + 1)));

						//secondary 'bending' constraints
						if (x < particleColRowCount.x - 2)
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x, y), GetParticle(x + 2, y)));
						if (y < particleColRowCount.y - 2)
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x, y), GetParticle(x, y + 2)));
						if (x < particleColRowCount.x - 2 && y < particleColRowCount.y - 2)
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x, y), GetParticle(x + 2, y + 2)));
						if (x < particleColRowCount.x - 2 && y < particleColRowCount.y - 2)
							constraints.push_back(std::make_shared<ClothConstraint>(GetParticle(x + 2, y), GetParticle(x, y + 2)));
					}
				}
			}

			void Render(std::shared_ptr<Core::Camera> mainCamera, const mat4 &toWorld)
			{
				mesh->Render(mainCamera, toWorld);
			}

			void OnFixedTimeStep(float deltaTime)
			{
				for(auto& i : particles)
					i->OnFixedTimeStep(deltaTime, damping);

				//update mesh positions and normals and rebind
				GenerateVertices();
				GenerateNormals();
				mesh->BuildAndBindVertexPositionNormalTexturedBuffer();
			}

			void ApplyConstraints()
			{
				for (size_t i = 0; i < constraintIterations; i++)
				{
					Concurrency::parallel_for(size_t(0), constraints.size(), [&](size_t j)
					{
						constraints[j]->ApplyConstraint();
					});
				}
			}
			
			void AddForce(const vec3 force)
			{
				for(auto p : particles)
				{
					p->AddForce(force);
				}
			}

			void AddWindForce(const vec3 direction)
			{
				for (size_t x = 0; x < static_cast<uint>(particleColRowCount.x)-1; x++)
				{
					for (size_t y = 0; y < static_cast<uint>(particleColRowCount.y)-1; y++)
					{
						AddWindForcesForTriangle(GetParticle(x + 1, y), GetParticle(x, y), GetParticle(x, y + 1), direction);
						AddWindForcesForTriangle(GetParticle(x + 1, y + 1), GetParticle(x + 1, y), GetParticle(x, y + 1), direction);
					}
				}
			}

			void HandleCollisions(std::vector<std::shared_ptr<ICollider>> colliders, const mat4 &worldTransform)
			{
				for (size_t i = 0; i < colliders.size(); i++)
				{
					if (colliders[i]->GetColliderType() == ICollider::SphereCollider)
					{
						std::shared_ptr<SphereColliderComponent> sphereCollider = std::dynamic_pointer_cast<SphereColliderComponent>(colliders[i]);

						//for (size_t j = 0; j < particles.size(); j++)
						Concurrency::parallel_for(size_t(0), particles.size(), [&](size_t j)
						{
							vec3 sphereCenter = sphereCollider->Position();
							
							//transform particles into world space to do collision detection
							vec4 tmp = vec4(particles[j]->pos, 1.0f);
							vec4 particlePos = worldTransform * tmp;

							vec3 v = vec3(particlePos) - sphereCenter;
							
							float lSquared = length2(v);
							float rad = sphereCollider->GetRadius();

							if(lSquared < rad * rad)
							{
								float l = length(v);
								//vec3 forceV = v / l * sphereCollider->Radius();
								//particles[j]->OffsetPosition(forceV);
								particles[j]->OffsetPosition(normalize(v) * (sphereCollider->GetRadius() - l));
								//particles[j]->pos = normalize(v) * (sphereCollider->Radius() - l);//particles[j]->oldPos;
							}
						});
					}
				}
			}
		};
	}
}