#pragma once

#include <ppl.h>
#include "ClothParticle2.h"
#include "../../Core/Camera.h"
#include "../../Dependencies/glm/gtx/norm.hpp"

class Cloth2
{
public:


	std::vector<ClothParticle2> particles;	
	std::unique_ptr<Mesh> mesh;
	vec2 size; // cloth dimensions
	ivec2 particleColRowCount;
	float distanceX; //horizontal distance between particles 			
	float distanceY; //vertical distance between particles
	
	const float gravity = -0.06f;
	
	Cloth2(vec2 size, ivec2 particleColRowCount)
		: size(size), particleColRowCount(particleColRowCount)
	{
		distanceX = size.x / particleColRowCount.x;
		distanceY = size.y / particleColRowCount.y;

		particles.resize(particleColRowCount.x * particleColRowCount.y);

		for (int x = 0; x < particleColRowCount.x; ++x) 
		{
			for (int y = 0; y < particleColRowCount.y; ++y) 
			{
				//build grid
				//lift edges to highlight wind effects
				vec3 position = vec3(size.x * (x / static_cast<float>(particleColRowCount.x)),
					(x == 0 || x == particleColRowCount.x - 1 
					|| y == 0 || y == particleColRowCount.y - 1) ? distanceY / 2.0f : 0,
					size.y * (y / static_cast<float>(particleColRowCount.y)));
				
				particles[y * particleColRowCount.x + x] = ClothParticle2(position, vec3(0, gravity, 0));
			}
		}

		CalculateClothNormals();

		//Pass onto mesh
		mesh = std::make_unique<Mesh>(&AssetManager::GetInstance());
		GenerateVertices();
		GenerateIndicies();
		GenerateUVs();
		GenerateMeshNormals();
	}

	void CalculateClothNormals() 
	{
		for (int x = 0; x < particleColRowCount.x - 1; x++) 
		{
			for (int y = 0; y < particleColRowCount.y - 1; y++)
			{
				//calc normals the two triangles in this quad and add the resulting normal to the particles normal value 
				//(summed vertex norm across all attached triangles for smooth lighting)

				vec3 normal = CalculateTriangleCross(*GetParticle(x + 1, y), *GetParticle(x, y), *GetParticle(x, y + 1));
				GetParticle(x + 1, y)->addNormal(normal);
				GetParticle(x, y)->addNormal(normal);
				GetParticle(x, y + 1)->addNormal(normal);

				normal = CalculateTriangleCross(*GetParticle(x + 1, y + 1), *GetParticle(x + 1, y), *GetParticle(x, y + 1));
				GetParticle(x + 1, y + 1)->addNormal(normal);
				GetParticle(x + 1, y)->addNormal(normal);
				GetParticle(x, y + 1)->addNormal(normal);
			}
		}

		size_t num = static_cast<int>(particles.size());
		Concurrency::parallel_for(size_t(0), num, [&](size_t i)
		{
			particles[i].normal = normalize(particles[i].normal);
		});
	}


	vec3 CalculateTriangleCross(const ClothParticle2& p1, const ClothParticle2& p2, const ClothParticle2& p3)
	{
		return normalize(cross(vec3(p2.position - p1.position), vec3(p3.position - p1.position)));
	}

	ClothParticle2* GetParticle(int x, int y) 
	{
		return &particles[y * particleColRowCount.x + x];
	}

	//used for rendering to avoid changes from integration
	const ClothParticle2* GetParticleConst(int x, int y) const {
		return &particles[y * particleColRowCount.x + x];
	}

	void Update(float deltaTime)
	{
		size_t num = particles.size();
		Concurrency::parallel_for(size_t(0), num, [&](size_t i)
		{
			particles[i].Integrate(deltaTime);
		});

		//update mesh positions and normals and rebind
		GenerateVertices();
		GenerateMeshNormals();
		mesh->BuildAndBindVertexPositionNormalTexturedBuffer(true);
	}

	void CalculateForces(const vec3 &wind_dir, const vec3 &ball_pos, float ball_radius) 
	{
		size_t num = static_cast<int>(particles.size());
		Concurrency::parallel_for(size_t(0), num, [&](size_t i)
		{
			ClothParticle2* p = &particles[i];
			// Alter the application of gravity so it is not effected by mass
			p->addForceAccumulated(p->acceleration * (1.0f / p->inverseMass));

			// Apply the wind using the last calculated normals
			vec3 force = p->normal * (dot(p->normal, wind_dir));
			p->addForceAccumulated(force);
		});

		BallCollision(ball_pos, ball_radius);
		CalculateClothNormals();	

		// Calculate new forces
		for (int x = 0; x < particleColRowCount.x; ++x) 
		{
			for (int y = 0; y < particleColRowCount.y; ++y)
			{
				ClothParticle2* p = GetParticle(x, y);
				float adjParticleCount = 0;
				vec3 springForce = vec3(0, 0, 0);		//Will contain the forces of all adjacent particles are applying
				float largestStretch = 0;				//Used to cap the length the cloth can stretch

				//use closest 24 positions (two out from current particle in all available directions)
				for (int a = -2; a <= 2; ++a) 
				{
					for (int b = -2; b <= 2; ++b)
					{
						if ((a != 0 || b != 0) && //not current
							//not off the edges
							x + a >= 0 && 
							x + a < particleColRowCount.x && 
							y + b >= 0 && 
							y + b < particleColRowCount.y)
						{
							// Get the direction this spring is pulling
							vec3 springVector = GetParticleConst(x + a, y + b)->position - p->position;

							// Find how much force is exerted by this string
							float len = length(springVector);
							float normalLength = sqrt((a * distanceX) * (a * distanceX) + (b * distanceY) * (b * distanceY));
							float forceScalar = (len - normalLength) / normalLength;

							// Add the force this particle is applying to the other particle forces
							springForce += springVector / len  * forceScalar;
							adjParticleCount++;

							// Keep track of which spring is exerting the most force
							if (forceScalar > largestStretch) 
							{
								largestStretch = forceScalar;
							}
						}
					}
				}

				// If a spring is stretched beyond 25% we will begin to minimize the effects of other forces to maintain the cloth shape
				if (largestStretch >= 0.25) {
					p->multiplyForceAccumulated((largestStretch >= 0.75f) ? 0.0f : (0.75f - largestStretch));
				}

				// Apply the force of the cloth on this particle
				p->addForceAccumulated(springForce / adjParticleCount);
			}
		}
	}

	void BallCollision(const vec3 &center, const float radius) 
	{
		size_t num = particles.size();
		Concurrency::parallel_for(size_t(0), num, [&](size_t i)
		{
			vec3 v = particles[i].position - center;		
			float l = length2(v);

			if (l < radius*radius)
			{
				particles[i].position = (normalize(v)* radius + center); // position on edge of sphere radius
				particles[i].multiplyVelocity(0.5f); //fake some friction
			}
		});
	}

	void Render(std::shared_ptr<Core::Camera> mainCamera, const mat4 &toWorld)
	{
		glDepthFunc(GL_LEQUAL);
		mesh->Render(mainCamera, toWorld);
	}

	void GenerateVertices()
	{
		mesh->vertices.clear();
		for (auto& i : particles)
			mesh->vertices.push_back(i.position);
	}

	void GenerateIndicies()
	{
		//indices
		for (int x = 0; x < particleColRowCount.x - 1; x++)
		{
			for (int y = 0; y < particleColRowCount.y - 1; y++)
			{
				int index1 = y * particleColRowCount.x + x;
				int index2 = (y + 1) * particleColRowCount.x + x;
				int index3 = y * particleColRowCount.x + x + 1;
				int index4 = (y + 1) * particleColRowCount.x + x + 1;

				mesh->indices.push_back(index1);
				mesh->indices.push_back(index2);
				mesh->indices.push_back(index3);

				mesh->indices.push_back(index3);
				mesh->indices.push_back(index2);
				mesh->indices.push_back(index4);
			}
		}
	}

	void GenerateMeshNormals()
	{
		mesh->normals.clear();
		for(auto v : particles)
		{
			mesh->normals.push_back(v.normal);
		}
	}

	void GenerateUVs()
	{
		for (int x = 0; x < particleColRowCount.x; x++)
		{
			for (int y = 0; y < particleColRowCount.y; y++)
			{
				mesh->uvs.push_back(vec2(static_cast<float>(x) / (particleColRowCount.x - 1), static_cast<float>(y) / (particleColRowCount.y - 1)));
			}
		}
	}

	~Cloth2()
	{
	}
};

