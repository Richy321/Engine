#pragma once
#include  "BoundingSphere.h"
#include "AABB.h"
#include <memory>
#include "../Core/IGameobject.h"
#include "../Core/Utils.h"
#include "../Core/Components/Interfaces/ICollider.h"
#include "../Core/Components/SphereColliderComponent.h"
#include "../Core/Components/PolygonColliderComponent.h"
#include "../IManifold.h"

using namespace glm;

namespace Collision
{
	typedef void(*CollisionCallback)(std::shared_ptr<IManifold>& m, std::shared_ptr<ICollider>& a, std::shared_ptr<ICollider>& b);

	void CircleVsCircle(std::shared_ptr<IManifold>& m, std::shared_ptr<ICollider>& colliderA, std::shared_ptr<ICollider>& colliderB)
	{
		std::shared_ptr<SphereColliderComponent> sphereA = std::dynamic_pointer_cast<SphereColliderComponent>(colliderA);
		std::shared_ptr<SphereColliderComponent> sphereB = std::dynamic_pointer_cast<SphereColliderComponent>(colliderB);

		std::shared_ptr<IGameObject> bodyAGO = sphereA->GetParentGameObject().lock();
		std::shared_ptr<IGameObject> bodyBGO = sphereB->GetParentGameObject().lock();

		glm::vec2 posA = glm::vec2(bodyAGO->GetWorldTransform()[3].x, bodyAGO->GetWorldTransform()[3].y);
		glm::vec2 posB = glm::vec2(bodyBGO->GetWorldTransform()[3].x, bodyBGO->GetWorldTransform()[3].y);

		glm::vec2 normal = posB - posA;
		float radius = sphereA->boundingSphere.radius + sphereB->boundingSphere.radius;

		float squaredDistance = Utils::Len2Vec2(normal);
		float squaredRadius = radius * radius;


		if (squaredDistance >= squaredRadius)
		{
			//no contact
			//m->contacts.clear();
			return;
		}


		float distance = sqrtf(squaredDistance);

		if (distance == 0.0f)
		{
			//ontop each other exactly
			m->penetration = sphereA->boundingSphere.radius;
			m->normal = glm::vec2(1, 0);
			m->contacts.push_back(glm::vec2(posA));
		}
		else
		{
			m->penetration = radius - distance;
			m->normal = normal / distance;
			m->contacts.push_back(m->normal * sphereA->boundingSphere.radius + posA);
		}
	}

	void CircletoPolygon(std::shared_ptr<IManifold>& m, std::shared_ptr<ICollider>& colliderA, std::shared_ptr<ICollider>& colliderB)
	{
		std::shared_ptr<SphereColliderComponent> sphereA = std::dynamic_pointer_cast<SphereColliderComponent>(colliderA);
		std::shared_ptr<PolygonColliderComponent> polyB = std::dynamic_pointer_cast<PolygonColliderComponent>(colliderB);
		std::shared_ptr<IGameObject> bodyAGO = sphereA->GetParentGameObject().lock();
		std::shared_ptr<IGameObject> bodyBGO = polyB->GetParentGameObject().lock();

		//transform circle center to polygon model space
		glm::vec2 center = vec2(bodyAGO->GetPosition());
		glm::vec2 bPos = vec2(bodyBGO->GetPosition());
		glm::vec2 aPos = vec2(bodyAGO->GetPosition());

		glm::mat2 b_transform_mat2 = glm::mat2(bodyBGO->GetWorldTransform());

		//m->contacts.clear();

		center = transpose(b_transform_mat2) * center - vec2(bodyBGO->GetPosition());

		// Find edge with minimum penetration
		// Exact concept as using support points in Polygon vs Polygon
		float separation = -FLT_MAX;
		unsigned int faceNormal = 0;
		for (unsigned int i = 0; i < polyB->polygonCollider.vertices.size(); ++i)
		{
			float s = dot(polyB->polygonCollider.normals[i], center - polyB->polygonCollider.vertices[i]);

			if (s > sphereA->boundingSphere.radius)
				return;

			if (s > separation)
			{
				separation = s;
				faceNormal = i;
			}
		}

		// Grab face's vertices
		vec2 v1 = polyB->polygonCollider.vertices[faceNormal];
		uint32 i2 = faceNormal + 1 < polyB->polygonCollider.vertices.size() ? faceNormal + 1 : 0;
		vec2 v2 = polyB->polygonCollider.vertices[i2];

		// Check to see if center is within polygon
		if (separation < EPSILON)
		{
			m->normal = -(b_transform_mat2 * polyB->polygonCollider.normals[faceNormal]);
			m->penetration = sphereA->boundingSphere.radius;
			m->contacts.push_back(m->normal * sphereA->boundingSphere.radius + aPos);
			return;
		}

		// Determine which voronoi region of the edge center of circle lies within
		float dot1 = dot(center - v1, v2 - v1);
		float dot2 = dot(center - v2, v1 - v2);
		m->penetration = sphereA->boundingSphere.radius - separation;

		// Closest to v1
		if (dot1 <= 0.0f)
		{
			if (Utils::DistSquared(center, v1) > sphereA->boundingSphere.radius * sphereA->boundingSphere.radius)
				return;

			vec2 n = v1 - center;
			n = b_transform_mat2 * n;
			n = normalize(n);
			m->normal = n;
			v1 = b_transform_mat2 * v1 + bPos;
			m->contacts.push_back(v1);
		}

		// Closest to v2
		else if (dot2 <= 0.0f)
		{
			if (Utils::DistSquared(center, v2) > sphereA->boundingSphere.radius * sphereA->boundingSphere.radius)
				return;

			vec2 n = v2 - center;
			v2 = b_transform_mat2 * v2 + bPos;
			m->contacts.push_back(v2);
			n = b_transform_mat2 * n;
			n = normalize(n);
			m->normal = n;
		}

		// Closest to face
		else
		{
			vec2 n = polyB->polygonCollider.normals[faceNormal];
			if (dot(center - v1, n) > sphereA->boundingSphere.radius)
				return;

			n = b_transform_mat2 * n;
			m->normal = -n;
			m->contacts.push_back(m->normal * sphereA->boundingSphere.radius + aPos);
		}
	}

	void PolygontoCircle(std::shared_ptr<IManifold>& m, std::shared_ptr<ICollider>& colliderA, std::shared_ptr<ICollider>& colliderB)
	{
		CircletoPolygon(m, colliderB, colliderA);
		m->normal = -m->normal;
	}

	vec2 GetSupport(const vec2& dir, std::shared_ptr<PolygonColliderComponent>& polygonCollider)
	{
		float bestProjection = -FLT_MAX;
		vec2 bestVertex;

		for (uint32 i = 0; i < polygonCollider->polygonCollider.vertices.size(); ++i)
		{
			vec2 v = polygonCollider->polygonCollider.vertices[i];
			float projection = Utils::DotVec2(v, dir);

			if (projection > bestProjection)
			{
				bestVertex = v;
				bestProjection = projection;
			}
		}

		return bestVertex;
	}

	float FindAxisLeastPenetration(uint32 *faceIndex, std::shared_ptr<PolygonColliderComponent>& A, std::shared_ptr<PolygonColliderComponent>& B)
	{
		float bestDistance = -FLT_MAX;
		uint32 bestIndex;

		std::shared_ptr<IGameObject> bodyAGO = A->GetParentGameObject().lock();
		std::shared_ptr<IGameObject> bodyBGO = B->GetParentGameObject().lock();

		vec2 aPos = vec2(bodyAGO->GetPosition());
		vec2 bPos = vec2(bodyBGO->GetPosition());
		
		glm::mat2 ATransform = glm::mat2(bodyAGO->GetWorldTransform());
		glm::mat2 BTransform = glm::mat2(bodyBGO->GetWorldTransform());

		for (uint32 i = 0; i < A->polygonCollider.vertices.size(); ++i)
		{
			// Retrieve a face normal from A
			vec2 n = A->polygonCollider.normals[i];
			vec2 nw = ATransform * n;

			// Transform face normal into B's model space
			mat2 buT = transpose(BTransform);
			n = buT * nw;

			// Retrieve support point from B along -n
			vec2 s = GetSupport(-n, B);

			// Retrieve vertex on face from A, transform into
			// B's model space
			vec2 v = A->polygonCollider.vertices[i];
			v = ATransform * v + aPos;
			v -= bPos;
			v = buT * v;

			// Compute penetration distance (in B's model space)
			float d = Utils::DotVec2(n, s - v);

			// Store greatest distance
			if (d > bestDistance)
			{
				bestDistance = d;
				bestIndex = i;
			}
		}

		*faceIndex = bestIndex;
		return bestDistance;
	}

	void FindIncidentFace(vec2 *v, std::shared_ptr<PolygonColliderComponent>& RefPoly, std::shared_ptr<PolygonColliderComponent>& IncPoly, uint32 referenceIndex)
	{
		vec2 referenceNormal = RefPoly->polygonCollider.normals[referenceIndex];

		std::shared_ptr<IGameObject> incGO = IncPoly->GetParentGameObject().lock();
		std::shared_ptr<IGameObject> refGO = RefPoly->GetParentGameObject().lock();

		vec2 incPos = vec2(incGO->GetPosition());
		vec2 refPos = vec2(refGO->GetPosition());

		mat2 refTransform = mat2(refGO->GetWorldTransform());
		mat2 incTransform = mat2(incGO->GetWorldTransform());

		// Calculate normal in incident's frame of reference
		referenceNormal = refTransform * referenceNormal; // To world space
		referenceNormal = transpose(incTransform) * referenceNormal; // To incident's model space

		// Find most anti-normal face on incident polygon
		int32 incidentFace = 0;
		float minDot = FLT_MAX;
		for (uint32 i = 0; i < IncPoly->polygonCollider.vertices.size(); ++i)
		{
			float dotResult = Utils::DotVec2(referenceNormal, IncPoly->polygonCollider.normals[i]);
			if (dotResult < minDot)
			{
				minDot = dotResult;
				incidentFace = i;
			}
		}

		// Assign face vertices for incidentFace
		v[0] = incTransform * IncPoly->polygonCollider.vertices[incidentFace] + incPos;
		incidentFace = incidentFace + 1 >= (int32)IncPoly->polygonCollider.vertices.size() ? 0 : incidentFace + 1;
		v[1] = incTransform * IncPoly->polygonCollider.vertices[incidentFace] + incPos;
	}

	int32 Clip(vec2 n, float c, vec2 *face)
	{
		uint32 sp = 0;
		vec2 out[2] = {
			face[0],
			face[1]
		};

		// Retrieve distances from each endpoint to the line
		// d = ax + by - c
		float d1 = dot(n, face[0]) - c;
		float d2 = dot(n, face[1]) - c;

		// If negative (behind plane) clip
		if (d1 <= 0.0f) out[sp++] = face[0];
		if (d2 <= 0.0f) out[sp++] = face[1];

		// If the points are on different sides of the plane
		if (d1 * d2 < 0.0f) // less than to ignore -0.0f
		{
			// Push interesection point
			float alpha = d1 / (d1 - d2);
			out[sp] = face[0] + alpha * (face[1] - face[0]);
			++sp;
		}

		// Assign our new converted values
		face[0] = out[0];
		face[1] = out[1];

		assert(sp != 3);

		return sp;
	}

	void PolygontoPolygon(std::shared_ptr<IManifold>& m, std::shared_ptr<ICollider>& colliderA, std::shared_ptr<ICollider>& colliderB)
	{
		std::shared_ptr<PolygonColliderComponent> polyA = std::dynamic_pointer_cast<PolygonColliderComponent>(colliderA);
		std::shared_ptr<PolygonColliderComponent> polyB = std::dynamic_pointer_cast<PolygonColliderComponent>(colliderB);

		m->contacts.clear();

		// Check for a separating axis with A's face planes
		uint32 faceA;
		float penetrationA = FindAxisLeastPenetration(&faceA, polyA, polyB);
		if (penetrationA >= 0.0f)
			return;

		// Check for a separating axis with B's face planes
		uint32 faceB;
		float penetrationB = FindAxisLeastPenetration(&faceB, polyB, polyA);
		if (penetrationB >= 0.0f)
			return;

		uint32 referenceIndex;
		bool flip; // Always point from a to b

		std::shared_ptr<PolygonColliderComponent> RefPoly; // Reference
		std::shared_ptr<PolygonColliderComponent> IncPoly; // Incident

		// Determine which shape contains reference face
		if (Utils::BiasGreaterThan(penetrationA, penetrationB))
		{
			RefPoly = polyA;
			IncPoly = polyB;
			referenceIndex = faceA;
			flip = false;
		}
		else
		{
			RefPoly = polyB;
			IncPoly = polyA;
			referenceIndex = faceB;
			flip = true;
		}

		// World space incident face
		vec2 incidentFace[2];
		FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);

		//        y
		//        ^  ->n       ^
		//      +---c ------posPlane--
		//  x < | i |\
		//      +---+ c-----negPlane--
		//             \       v
		//              r
		//
		//  r : reference face
		//  i : incident poly
		//  c : clipped point
		//  n : incident normal

		// Setup reference face vertices
		std::shared_ptr<IGameObject> incGO = IncPoly->GetParentGameObject().lock();
		std::shared_ptr<IGameObject> refGO = RefPoly->GetParentGameObject().lock();

		vec2 refPos = vec2(refGO->GetPosition());
		mat2 refTransform = mat2(refGO->GetWorldTransform());

		vec2 v1 = RefPoly->polygonCollider.vertices[referenceIndex];
		referenceIndex = referenceIndex + 1 == RefPoly->polygonCollider.vertices.size() ? 0 : referenceIndex + 1;
		vec2 v2 = RefPoly->polygonCollider.vertices[referenceIndex];

		// Transform vertices to world space
		v1 = refTransform * v1 + refPos;
		v2 = refTransform * v2 + refPos;

		// Calculate reference face side normal in world space
		vec2 sidePlaneNormal = v2 - v1;
		sidePlaneNormal = normalize(sidePlaneNormal);

		// Orthogonalize
		vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

		// ax + by = c
		// c is distance from origin
		float refC = Utils::DotVec2(refFaceNormal, v1);
		float negSide = -Utils::DotVec2(sidePlaneNormal, v1);
		float posSide = Utils::DotVec2(sidePlaneNormal, v2);

		// Clip incident face to reference face side planes
		if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
			return; // Due to floating point error, possible to not have required points

		if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
			return; // Due to floating point error, possible to not have required points

		// Flip
		m->normal = flip ? -refFaceNormal : refFaceNormal;

		// Keep points behind reference face
		uint32 cp = 0; // clipped points behind reference face
		float separation = Utils::DotVec2(refFaceNormal, incidentFace[0]) - refC;
		if (separation <= 0.0f)
		{
			m->contacts.push_back(incidentFace[0]);
			m->penetration = -separation;
			++cp;
		}
		else
			m->penetration = 0;

		separation = Utils::DotVec2(refFaceNormal, incidentFace[1]) - refC;
		if (separation <= 0.0f)
		{
			m->contacts.push_back(incidentFace[1]);
			m->penetration += -separation;
			++cp;

			// Average penetration
			m->penetration /= (float)cp;
		}

		//m->contact_count = cp;
	}

	bool AABBvsAABB(std::shared_ptr<AABB> a, std::shared_ptr<AABB> b)
	{
		if (a->max.x < b->min.x || a->min.x > b->max.x) return false;
		if (a->max.y < b->min.y || a->min.y > b->max.y) return false;

		//no seperating axis, at least one overlapping axis
		return true;
	}

	//bool AABBvsAABB(std::shared_ptr<IManifold>& m, std::shared_ptr<AABB> a, std::shared_ptr<AABB> b)
	//{
	//	if (a->max.x < b->min.x || a->min.x > b->max.x) return false;
	//	if (a->max.y < b->min.y || a->min.y > b->max.y) return false;

	//	//no seperating axis, at least one overlapping axis
	//	return true;
	//}


}