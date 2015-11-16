#pragma once
#include <math.h>
#include "../Math/Utils.h"
#include "../Math/mat4.h"
#include "GameObject.h"

using namespace Math;

namespace Core
{
	class Camera : public GameObject
	{
	public:
		enum ProjectionType
		{
			Perspective,
			Orthographic
		};

		float nearPlane;
		float farPlane;

		float fovX;
		float fovY;
		float aspectRatio;
		ProjectionType projectionType = Perspective;
		vec3 target;

		Camera() :target(0.0f, 0.0f, 100.0f)
		{
			nearPlane = 0.1f;
			farPlane = 1000.0f;

			fovX = 0.0f;
			fovY = 90.0f;
			aspectRatio = 1.0f;
		}

		~Camera()
		{
		}


		mat4 view;
		mat4 projection;


		void SetPerspectiveProjection(float fov, float width, float height, float zNear, float zFar)
		{
			projectionType = Perspective;
			projection = BuildPerspectiveProjection(fov, width, height, zNear, zFar);
		}

		void SetOrthographicProjection(float width, float height, float zNear, float zFar)
		{
			projectionType = Orthographic;
			projection = BuildOrthographicProjection(width, height, zNear, zFar);
		}

		void Update(float deltaTime) override
		{
			//view = gluLookAt()
			view = LookAt(vec3(world[3][0], world[3][1], world[3][2]), target, vec3(0, 1, 0));
		}

		static mat4 BuildPerspectiveProjection(float fov, float width, float height, float zNear, float zFar)
		{
			mat4 m(0); //initialised to all zero
			const float ar = width / height;
			const float zRange = zNear - zFar;
			const float tanHalfFOV = tanf(Utils::DegToRad(fov / 2.0f));

			m[0][0] = 1.0f / (tanHalfFOV * ar);
            m[1][1] = 1.0f / tanHalfFOV; 
			m[2][2] = (-zNear - zFar) / zRange; 
			m[2][3] = 2.0f * zFar * zNear / zRange;
			m[3][2] = 1.0f;
			
			return m;
		}

		static mat4 BuildOrthographicProjection(float width, float height, float zNear, float zFar)
		{
			const float zRange = zNear - zFar;
			mat4 m; //initialised to identity;
			m[0][0] = 2.0f / width;
			m[1][1] = 2.0f / height;
			m[2][2] = 1.0f / zRange;
			m[2][3] = -zNear / zRange;

			return m;
		}

		static mat4 LookAt(const vec3 &position, const vec3 &target, const vec3 &up)
		{
			//get forwards
			vec3 f = position - target;
			f.normalize();

			//get sidewards
			vec3 s = f.cross(up);
			s.normalize();

			//get correct upwards
			vec3 v = s.cross(f);

			//negate the forward direction so its pointing down negative z
			f = -f;

			mat4 V
			{
				vec4(s.x, v.x, f.x, 0.0),
				vec4(s.y, v.y, f.y, 0.0),
				vec4(s.z, v.z, f.z, 0.0),
				vec4(-s.dot(position), -v.dot(position), f.dot(position), 1.0)
			};

			return V;
		}
	};
}

