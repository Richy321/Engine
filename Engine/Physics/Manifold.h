#pragma once
#include "RigidBody2DComponent.h"
#include <vector>
#include "CollisionDetection.h"
#include "../Core/Components/Interfaces/ICollider.h"
#include "../IManifold.h"

namespace Collision
{
	extern CollisionCallback Dispatch[ICollider::ColliderType::Count][ICollider::ColliderType::Count]
	{
		{
			CircleVsCircle, CircletoPolygon
		},
		{
			PolygontoCircle, PolygontoPolygon
		},
	};
}
class Manifold : public IManifold
{

	std::shared_ptr<Mesh> mesh;

public:
	Manifold(std::shared_ptr<RigidBody2DComponent> bodyA, std::shared_ptr<RigidBody2DComponent> bodyB)
	{
		this->bodyA = bodyA;
		this->bodyB = bodyB;
		mesh = std::make_shared<Mesh>(&AssetManager::GetInstance());
		mesh->mode = GL_LINES;
		mesh->renderType = Mesh::Coloured;
	}

	~Manifold()
	{
	}


	void Solve() override
	{
		std::shared_ptr<ICollider> aCollider = std::dynamic_pointer_cast<ICollider>(bodyA->GetParentGameObject().lock()->GetComponentByType(IComponent::Collider));
		std::shared_ptr<ICollider> bCollider = std::dynamic_pointer_cast<ICollider>(bodyB->GetParentGameObject().lock()->GetComponentByType(IComponent::Collider));

		Collision::Dispatch[aCollider->GetColliderType()][bCollider->GetColliderType()](shared_from_this(), aCollider, bCollider);
	}

	void Initialise(float dt, vec2 gravity) override
	{
		const float EPSILON = 0.0001f;

		e = std::min(bodyA->physicsMaterial->restitution, bodyB->physicsMaterial->restitution);

		staticFriction = std::sqrt(bodyA->staticFriction * bodyA->staticFriction);
		dynamicFriction = std::sqrt(bodyA->dynamicFriction * bodyA->dynamicFriction);

		for (size_t i = 0; i < contacts.size(); i++)
		{
			vec2 ra = contacts[i] - vec2(bodyA->GetParentGameObject().lock()->GetPosition());
			vec2 rb = contacts[i] - vec2(bodyB->GetParentGameObject().lock()->GetPosition());

			vec2 rv = bodyB->velocity + Utils::CrossVec2(bodyB->angularVelocity, rb) -
				bodyA->velocity - Utils::CrossVec2(bodyA->angularVelocity, ra);

			//if only gravity: collide without restitution
			if (Utils::Len2Vec2(rv) < Utils::Len2Vec2(dt * gravity) + EPSILON)
				e = 0.0f;
		}

		InitialiseMesh();
	}

	void ApplyImpulseSimple()
	{
		if (Utils::EqualWithEpsilon(bodyA->inverseMass + bodyB->inverseMass, 0.0f))
		{
			InfiniteMassCorrection();
			return;
		}

		for (size_t i = 0; i < contacts.size(); ++i)
		{
			vec2 ra = contacts[i] - vec2(bodyA->GetParentGameObject().lock()->GetPosition());
			vec2 rb = contacts[i] - vec2(bodyB->GetParentGameObject().lock()->GetPosition());

			vec2 relVel = bodyB->velocity - bodyA->velocity;

			float velAlongNormal = Utils::DotVec2(relVel, normal);

			if (velAlongNormal > 0)
				return;

			float e = min(bodyA->physicsMaterial->restitution, bodyB->physicsMaterial->restitution);

			// Calculate impulse scalar
			float j = -(1.0f + e) * velAlongNormal;
			j /= bodyA->inverseMass + bodyB->inverseMass;

			// Apply impulses
			vec2 impulse = j * normal;
			bodyA->ApplyImpulse(-(bodyA->inverseMass * impulse), ra);
			bodyB->ApplyImpulse(bodyB->inverseMass * impulse, rb);
		}
	}

	void ApplyImpulseWithFriction()
	{
		if (Utils::EqualWithEpsilon(bodyA->inverseMass + bodyB->inverseMass, 0.0f))
		{
			InfiniteMassCorrection();
			return;
		}

		for (size_t i = 0; i < contacts.size(); ++i)
		{
			vec2 ra = contacts[i] - vec2(bodyA->GetParentGameObject().lock()->GetPosition());
			vec2 rb = contacts[i] - vec2(bodyB->GetParentGameObject().lock()->GetPosition());

			vec2 relVel = bodyB->velocity - bodyA->velocity;

			float combinedInverseMass = bodyA->inverseMass + bodyB->inverseMass;

			float velAlongNormal = Utils::DotVec2(relVel, normal);

			if (velAlongNormal > 0)
				return;

			float e = min(bodyA->physicsMaterial->restitution, bodyB->physicsMaterial->restitution);

			// Calculate impulse scalar
			float j = -(1.0f + e) * velAlongNormal;
			j /= combinedInverseMass;

			// Apply impulses
			vec2 impulse = j * normal;
			bodyA->ApplyImpulse(-(bodyA->inverseMass * impulse), ra);
			bodyB->ApplyImpulse(bodyB->inverseMass * impulse, rb);

			//tangent
			float dotRelNorm = Utils::DotVec2(relVel, normal);

			vec2 tangent = relVel - dotRelNorm * normal;
			tangent = Utils::NormaliseVec2(tangent);

			// j tangent magnitude
			float jt = -Utils::DotVec2(relVel, tangent); //friction applies negatively along tangent
			jt /= combinedInverseMass;
			jt /= static_cast<float>(contacts.size());

			// Don't apply tiny friction impulses
			if (Utils::EqualWithEpsilon(jt, 0.0f))
				return;

			// Coulumb's law
			vec2 tangentImpulse;
			if (std::abs(jt) < j * staticFriction)
				tangentImpulse = tangent * jt;
			else
				tangentImpulse = tangent * -j * dynamicFriction;

			// Apply friction impulse
			bodyA->ApplyImpulse(-tangentImpulse, ra);
			bodyB->ApplyImpulse(tangentImpulse, rb);
		}
	}

	void ApplyImpulseFrictionOrientation()
	{
		if (Utils::EqualWithEpsilon(bodyA->inverseMass + bodyB->inverseMass, 0.0f))
		{
			InfiniteMassCorrection();
			return;
		}

		for (size_t i = 0; i < contacts.size(); ++i)
		{
			// Calculate radii from COM to contact
			vec2 ra = contacts[i] - vec2(bodyA->GetParentGameObject().lock()->GetPosition());
			vec2 rb = contacts[i] - vec2(bodyB->GetParentGameObject().lock()->GetPosition());

			// Relative velocity
			vec2 rv = bodyB->velocity + Utils::CrossVec2(bodyB->angularVelocity, rb) -
				bodyA->velocity - Utils::CrossVec2(bodyA->angularVelocity, ra);

			// Relative velocity along the normal
			float contactVel = Utils::DotVec2(rv, normal);

			// Do not resolve if velocities are separating
			if (contactVel > 0)
				return;

			float raCrossN = Utils::CrossVec2(ra, normal);
			float rbCrossN = Utils::CrossVec2(rb, normal);
			float invMassSum = bodyA->inverseMass + bodyB->inverseMass + pow(raCrossN, 2) * bodyA->inverseInertia + pow(rbCrossN, 2) * bodyB->inverseInertia;

			// Calculate impulse scalar
			float j = -(1.0f + e) * contactVel;
			j /= invMassSum;
			j /= (float)contacts.size();

			// Apply impulse
			vec2 impulse = normal * j;
			bodyA->ApplyImpulse(-impulse, ra);
			bodyB->ApplyImpulse(impulse, rb);

			// Friction impulse
			rv = bodyB->velocity + Utils::CrossVec2(bodyB->angularVelocity, rb) -
				bodyA->velocity - Utils::CrossVec2(bodyA->angularVelocity, ra);

			vec2 t = rv - (normal * Utils::DotVec2(rv, normal));
			t = Utils::NormaliseVec2(t);

			// j tangent magnitude
			float jt = -Utils::DotVec2(rv, t);
			jt /= invMassSum;
			jt /= (float)contacts.size();

			// Don't apply tiny friction impulses
			if (Utils::EqualWithEpsilon(jt, 0.0f))
				return;

			// Coulumb's law
			vec2 tangentImpulse;
			if (std::abs(jt) < j * staticFriction)
				tangentImpulse = t * jt;
			else
				tangentImpulse = t * -j * dynamicFriction;

			// Apply friction impulse
			bodyA->ApplyImpulse(-tangentImpulse, ra);
			bodyB->ApplyImpulse(tangentImpulse, rb);
		}
	}

	void ApplyImpulse() override
	{
		//ApplyImpulseSimple();
		ApplyImpulseWithFriction();
		return;
	}

	void PositionalCorrection() override
	{
		const float k_slop = 0.05f;
		const float percent = 0.4f;

		vec2 correction = (std::max(penetration - k_slop, 0.0f) / (bodyA->inverseMass + bodyB->inverseMass)) * normal * percent;
		printf("Correction %f, %f\n", correction.x, correction.y);
		bodyA->GetParentGameObject().lock()->Translate(-vec3(correction * bodyA->inverseMass, 0.0f));
		bodyB->GetParentGameObject().lock()->Translate(vec3(correction * bodyB->inverseMass, 0.0f));
	}

	void InfiniteMassCorrection() override
	{
		bodyA->velocity.x = 0;
		bodyA->velocity.y = 0;
		bodyB->velocity.x = 0;
		bodyB->velocity.y = 0;
	}


	void InitialiseMesh()
	{
		for (auto v : contacts)
		{
			mesh->vertices.push_back(vec3(v.x, v.y, 0.0f));
			mesh->colours.push_back(vec4(1.0f, 1.0f, 0.0f, 1.0f));

			vec2 projectedAlongNorm = v + (normal * 2.0f);


			mesh->vertices.push_back(vec3(projectedAlongNorm.x, projectedAlongNorm.y, 0.0f));
			mesh->colours.push_back(vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}

		if(contacts.size() >0)
			mesh->BuildAndBindVertexPositionColorBuffer();
	}

	void Render(std::shared_ptr<Camera> mainCamera, const mat4 &toWorld)
	{
		mesh->Render(mainCamera, toWorld);
	}
};

