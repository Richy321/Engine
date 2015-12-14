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
		mat4 worldToCamera;
		mat4 worldToProjection;

		void SetPerspectiveProjection(float fov, float width, float height, float zNear, float zFar)
		{
			projectionType = Perspective;
			//projection = BuildPerspectiveProjection(fov, width, height, zNear, zFar);
			projection = BuildPerspectiveProjection(-zNear, zNear, -zNear, zNear, zNear, zFar);
			//projection = BuildPerspectiveProjection2(fov, width, height, zNear, zFar);

		}

		void SetOrthographicProjection(float xMag, float yMag, float zNear, float zFar)
		{
			projectionType = Orthographic;
			//projection = BuildOrthographicProjection(width, height, zNear, zFar);
			projection = BuildOrthographicProjection(-xMag*0.5f, xMag*0.5f, -yMag*0.5f, yMag*0.5f, zNear, zFar);
		}

		void Update(float deltaTime) override
		{
			//view = LookAt(vec3(world[3][0], world[3][1], world[3][2]), target, vec3(0, 1, 0));
			worldToCamera = GetWorldTransform().InvertQuick();
			worldToProjection = worldToCamera * projection;
		}

		void LookAt(const vec3 &target)
		{
			this->target = target;
			vec3 pos = GetWorldTransform().GetRow(3).xyz();
			this->GetWorldTransform() = BuildLookAt(pos, target, vec3(0.0f, 1.0f, 0.0f));
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

		static mat4 BuildPerspectiveProjection2(float fov, float width, float height, float zNear, float zFar)
		{
			const float ar = width / height;
			const float zRange = zNear - zFar;
			const float tanHalfFOV = tanf(Utils::DegToRad(fov / 2.0f));
			mat4 m;

			m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;            m[0][2] = 0.0f;            m[0][3] = 0.0;
			m[1][0] = 0.0f;                   m[1][1] = 1.0f / tanHalfFOV; m[1][2] = 0.0f;            m[1][3] = 0.0;
			m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-zNear - zFar) / zRange; m[2][3] = 2.0f*zFar*zNear / zRange;
			m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;            m[3][3] = 0.0;

			return m.Transpose();
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

		static mat4 BuildLookAt(const vec3 &position, const vec3 &target, const vec3 &up)
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

		/// As in glFrustum - make a perspective matrix used for conventional cameras
		/// this matrix makes a 4x4 vector which is then divided to get perspective
		static mat4 BuildPerspectiveProjection(float left, float right, float bottom, float top, float n, float f)
		{
			float X = 2 * n / (right - left);
			float Y = 2 * n / (top - bottom);
			float A = (right + left) / (right - left);
			float B = (top + bottom) / (top - bottom);
			float C = -(f + n) / (f - n);
			float D = -2 * f*n / (f - n);

			// before the divide:
			// xp = ( 2*n*x + (right+left) * z ) / (right-left);
			// yp = ( 2*n*y + (top+bottom) * z ) / (top-bottom);
			// zp = (-2*n*f -        (f+n) * z ) / (f-n)
			// wp = -z

			// after the divide:
			// xd = ( -2*n*x/z - (right+left) ) / (right-left);
			// yd = ( -2*n*y/z - (top+bottom) ) / (top-bottom);
			// zd = (  2*n*f/z        + (f+n) ) / (f-n)

			// special values of z:
			// if z == -n: zd = ( (f+n) - 2*f ) / (f-n) = -1   (zp=n)
			// if z == -f: zd = ( (f+n) - 2*n ) / (f-n) = 1    (zp=-f)
			// if z == -infinity: zd = (f+n) / (f-n)

			// important: we get more z values closer to the camera,
			// so choose f and especially n with care!

			mat4 mul(
				vec4(X, 0.0f, 0.0f, 0.0f),
				vec4(0.0f, Y, 0.0f, 0.0f),
				vec4(A, B, C, -1.0f),
				vec4(0.0f, 0.0f, D, 0.0f)
				);
			return mul;
		}

		static mat4 BuildOrthographicProjection(float left, float right, float bottom, float top, float nearVal, float farVal)
		{
			float X = 2.0f / (right - left);
			float Y = 2.0f / (top - bottom);
			float Z = 2.0f / (farVal - nearVal);
			float tx = -(right + left) / (right - left);
			float ty = -(top + bottom) / (top - bottom);
			float tz = -(farVal + nearVal) / (farVal - nearVal);
			mat4 mul(
				vec4(X, 0.0f, 0.0f, 0.0f),
				vec4(0.0f, Y, 0.0f, 0.0f),
				vec4(0.0f, 0.0f, Z, 0.0f),
				vec4(tx, ty, tz, 1.0f)
				);
			return mul;
		}
	};
}

