#pragma once

#include <memory>
#include "RigidBody2DComponent.h"
#include "../Core/Components/SphereColliderComponent.h"
#include "IPhysicsManager.h"

using namespace glm;

namespace Core
{

	class PhysicsManager : public IPhysicsManager
	{
	public:

		PhysicsManager()
		{
		}

		~PhysicsManager() override
		{
		}

		static void ComputeSphereMass(std::shared_ptr<RigidBody2DComponent>& body, std::shared_ptr<SphereColliderComponent>& sphere)
		{
			float density = body->physicsMaterial->density;
			body->mass = M_PI * sphere->boundingSphere.radius * sphere->boundingSphere.radius * density;
			body->inverseMass = (body->mass) ? 1.0f / body->mass : 0.0f;
			body->inertia = body->mass * sphere->boundingSphere.radius * sphere->boundingSphere.radius;
			body->inverseInertia = (body->inertia) ? 1.0f / body->inertia : 0.0f;
		}

		static void ComputePolygonMass(std::shared_ptr<RigidBody2DComponent>& body, std::vector<glm::vec3>& vertices)
		{
			float density = body->physicsMaterial->density;
			// Calculate centroid and moment of interia
			vec2 c(0.0f, 0.0f); // centroid
			float area = 0.0f;
			float I = 0.0f;
			const float k_inv3 = 1.0f / 3.0f;

			for (uint32 i1 = 0; i1 < vertices.size(); ++i1)
			{
				// Triangle vertices, third vertex implied as (0, 0)
				vec2 p1(vertices[i1]);
				uint32 i2 = i1 + 1 < vertices.size() ? i1 + 1 : 0;
				vec2 p2(vertices[i2]);

				float D = Utils::CrossVec2(p1, p2); //vec2 'cross'
				float triangleArea = 0.5f * D;

				area += triangleArea;

				// Use area to weight the centroid average, not just vertex position
				c += triangleArea * k_inv3 * (p1 + p2);

				float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
				float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
				I += (0.25f * k_inv3 * D) * (intx2 + inty2);
			}

			c *= 1.0f / area;

			// Translate vertices to centroid (make the centroid (0, 0)
			// for the polygon in model space)
			// Not really necessary, but I like doing this anyway
			for (uint32 i = 0; i < vertices.size(); ++i)
				vertices[i] -= vec3(c, 0.0f);

			body->mass = density * area;
			body->inverseMass = (body->mass) ? 1.0f / body->mass : 0.0f;
			body->inertia = I * density;
			body->inverseInertia = body->inertia ? 1.0f / body->inertia : 0.0f;
		}
	};
}

