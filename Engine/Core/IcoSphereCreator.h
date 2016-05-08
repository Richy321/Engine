#pragma once
#include <map>
#include "Dependencies/glm/detail/type_vec3.hpp"
#include <vector>
#include <memory>
#include "Core/Mesh.h"


using namespace glm;
class IcoSphereCreator
{
public:

	IcoSphereCreator()
	{
	}

	~IcoSphereCreator()
	{
	}

	struct TriangleIndices
	{
		int v1;
		int v2;
		int v3;

		TriangleIndices(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3)
		{
		}
	};

	static void CreateSimple(std::shared_ptr<Core::Mesh> mesh)
	{
		const int Faces[] = {
			2, 1, 0,
			3, 2, 0,
			4, 3, 0,
			5, 4, 0,
			1, 5, 0,

			11, 6,  7,
			11, 7,  8,
			11, 8,  9,
			11, 9,  10,
			11, 10, 6,

			1, 2, 6,
			2, 3, 7,
			3, 4, 8,
			4, 5, 9,
			5, 1, 10,

			2,  7, 6,
			3,  8, 7,
			4,  9, 8,
			5, 10, 9,
			1, 6, 10 };

		const float Verts[] = {
			0.000f,  0.000f,  1.000f,
			0.894f,  0.000f,  0.447f,
			0.276f,  0.851f,  0.447f,
			-0.724f,  0.526f,  0.447f,
			-0.724f, -0.526f,  0.447f,
			0.276f, -0.851f,  0.447f,
			0.724f,  0.526f, -0.447f,
			-0.276f,  0.851f, -0.447f,
			-0.894f,  0.000f, -0.447f,
			-0.276f, -0.851f, -0.447f,
			0.724f, -0.526f, -0.447f,
			0.000f,  0.000f, -1.000f };

		mesh->indices.insert(std::end(mesh->indices), std::begin(Faces), std::end(Faces));
		mesh->vertices.insert(std::end(mesh->vertices), std::begin(Verts), std::end(Verts));
	}

	static void Create(std::shared_ptr<Core::Mesh> mesh, int recursionLevel, float radius)
	{
		middlePointIndexCache.clear();
		index = 0;

		auto t = (1.0f + sqrt(5.0f)) / 2.0f;

		AddVertex(mesh, normalize(vec3(-1, t, 0)) * radius);
		AddVertex(mesh, normalize(vec3(1, t, 0)) * radius);
		AddVertex(mesh, normalize(vec3(-1, -t, 0)) * radius);
		AddVertex(mesh, normalize(vec3(1, -t, 0)) * radius);

		AddVertex(mesh, normalize(vec3(0, -1, t)) * radius);
		AddVertex(mesh, normalize(vec3(0, 1, t)) * radius);
		AddVertex(mesh, normalize(vec3(0, -1, -t)) * radius);
		AddVertex(mesh, normalize(vec3(0, 1, -t)) * radius);

		AddVertex(mesh, normalize(vec3(t, 0, -1)) * radius);
		AddVertex(mesh, normalize(vec3(t, 0, 1)) * radius);
		AddVertex(mesh, normalize(vec3(-t, 0, -1)) * radius);
		AddVertex(mesh, normalize(vec3(-t, 0, 1)) * radius);

		auto faces = std::vector<std::shared_ptr<TriangleIndices>>();
		faces.push_back(std::make_shared<TriangleIndices>(0, 11, 5));
		faces.push_back(std::make_shared<TriangleIndices>(0, 5, 1));
		faces.push_back(std::make_shared<TriangleIndices>(0, 1, 7));
		faces.push_back(std::make_shared<TriangleIndices>(0, 7, 10));
		faces.push_back(std::make_shared<TriangleIndices>(0, 10, 11));

		faces.push_back(std::make_shared<TriangleIndices>(1, 5, 9));
		faces.push_back(std::make_shared<TriangleIndices>(5, 11, 4));
		faces.push_back(std::make_shared<TriangleIndices>(11, 10, 2));
		faces.push_back(std::make_shared<TriangleIndices>(10, 7, 6));
		faces.push_back(std::make_shared<TriangleIndices>(7, 1, 8));

		faces.push_back(std::make_shared<TriangleIndices>(3, 9, 4));
		faces.push_back(std::make_shared<TriangleIndices>(3, 4, 2));
		faces.push_back(std::make_shared<TriangleIndices>(3, 2, 6));
		faces.push_back(std::make_shared<TriangleIndices>(3, 6, 8));
		faces.push_back(std::make_shared<TriangleIndices>(3, 8, 9));

		faces.push_back(std::make_shared<TriangleIndices>(4, 9, 5));
		faces.push_back(std::make_shared<TriangleIndices>(2, 4, 11));
		faces.push_back(std::make_shared<TriangleIndices>(6, 2, 10));
		faces.push_back(std::make_shared<TriangleIndices>(8, 6, 7));
		faces.push_back(std::make_shared<TriangleIndices>(9, 8, 1));

		for (auto i = 0; i < recursionLevel; ++i)
		{
			auto faces2 = std::vector<std::shared_ptr<TriangleIndices>>();
			for (auto tri : faces)
			{
				int a = GetMiddlePoint(tri->v1, tri->v2, mesh, radius);
				int b = GetMiddlePoint(tri->v2, tri->v3, mesh, radius);
				int c = GetMiddlePoint(tri->v3, tri->v1, mesh, radius);

				faces2.push_back(std::make_shared<TriangleIndices>(tri->v1, a, c));
				faces2.push_back(std::make_shared<TriangleIndices>(tri->v2, b, a));
				faces2.push_back(std::make_shared<TriangleIndices>(tri->v3, c, b));
				faces2.push_back(std::make_shared<TriangleIndices>(a, b, c));
			}

			faces.clear();
			for (unsigned int j = 0; j < faces2.size(); ++j)
			{
				faces.push_back(faces2[j]);
			}
		}

		for (auto tri : faces)
		{
			mesh->indices.push_back(tri->v1);
			mesh->indices.push_back(tri->v2);
			mesh->indices.push_back(tri->v3);
		}
	}

private:
	static int index;
	static std::map<int64_t, int> middlePointIndexCache;

	static int AddVertex(std::shared_ptr<Core::Mesh> mesh, vec3 position)
	{
		mesh->vertices.push_back(position);
		mesh->colours.push_back(Core::Colours_RGBA::HotPink);
		mesh->normals.push_back(normalize(position));
		float tu = asin(position.x) / Core::M_PI + 0.5;
		float tv = asin(position.y) / Core::M_PI + 0.5;
		mesh->uvs.push_back(vec2(tu, tv));
		return index++;
	}

	static int GetMiddlePoint(int p1, int p2, std::shared_ptr<Core::Mesh> mesh, float radius)
	{
		bool firstPointIsSmaller = p1 < p2;
		int64_t smallerIndex = firstPointIsSmaller ? p1 : p2;
		int64_t greaterIndex = firstPointIsSmaller ? p2 : p1;
		int64_t key = (smallerIndex << 32) + greaterIndex;

		auto foundValueIterator = middlePointIndexCache.find(key);
		if (foundValueIterator != middlePointIndexCache.end())
		{
			return foundValueIterator->second;
		}

		vec3 point1 = mesh->vertices[p1];
		vec3 point2 = mesh->vertices[p2];
		vec3 middle = vec3((point1.x + point2.x) / 2.0,
								(point1.y + point2.y) / 2.0,
								(point1.z + point2.z) / 2.0);

		int i = AddVertex(mesh, normalize(middle) * radius);

		middlePointIndexCache.insert(std::make_pair(key, i));
		return i;
	}
};

int IcoSphereCreator::index = 0;

std::map<int64_t, int> IcoSphereCreator::middlePointIndexCache;
