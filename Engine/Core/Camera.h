#pragma once
#include "GameObject.h"

using namespace glm;

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

		virtual ~Camera()
		{
		}

		mat4 projection;
		mat4 view; //world to camera
		mat4 worldToProjection;

		void SetPerspectiveProjection(float fov, float width, float height, float zNear, float zFar)
		{
			projectionType = Perspective;
			projection = BuildPerspectiveProjection(fov, width, height, zNear, zFar);
		}

		void SetOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar)
		{
			projectionType = Orthographic;
			projection = BuildOrthographicProjection(left, right, bottom, top, zNear, zFar);
		}

		virtual void Update(float deltaTime) override
		{
			view = glm::inverse(world);
			worldToProjection = projection * view;
		}

		static mat4 BuildPerspectiveProjection(float fov, float width, float height, float zNear, float zFar)
		{
			const float ar = width / height;
			return glm::perspective(fov, ar, zNear, zFar);
		}

		static mat4 BuildOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar)
		{
			return glm::ortho(left, right, bottom, top, zNear, zFar);
		}

		virtual void OnMouseMove(int x, int y) = 0;

		virtual void OnKey(unsigned char key, int x, int y) = 0;
	};
}

