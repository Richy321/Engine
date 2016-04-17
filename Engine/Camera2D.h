#pragma once
#include "Core/Camera.h"

using namespace glm;

namespace Core
{
	class Camera2D : public Camera
	{
		const float cameraMovementSpeed = 10.1f;
		const float zoomSpeed = 0.01;
		vec3 translateDelta;
		float zoom = 1.0f;
		float zoomDelta = 0.0f;

		vec3 forwardDir = vec3(0.0f, 0.0f, 1.0f);
		vec3 rightDir = vec3(1.0f, 0.0f, 0.0f);
		vec3 upDir = vec3(0.0f, 1.0f, 0.0f);

	public:

		Camera2D()
		{
		}

		~Camera2D()
		{
		}


		void OnMouseMove(int deltaX, int deltaY) override
		{
		}

		void OnKey(unsigned char key, int x, int y) override
		{
			if (key == 'j')
				translateDelta -= rightDir * cameraMovementSpeed;
			if (key == 'l')
				translateDelta += rightDir * cameraMovementSpeed;
			if (key == 'i')
				translateDelta += forwardDir * cameraMovementSpeed;
			if (key == 'k')
				translateDelta -= forwardDir * cameraMovementSpeed;
			if (key == 'm')
				translateDelta += upDir * cameraMovementSpeed;
			if (key == '.')
				translateDelta -= upDir * cameraMovementSpeed;

			if (key == 'u')
				zoomDelta += zoomSpeed;
			if (key == 'o')
				zoomDelta -= zoomSpeed;

		}

		void Update(float deltaTime) override
		{
			Translate(translateDelta * deltaTime);
			translateDelta.x = 0.0f;
			translateDelta.y = 0.0f;
			translateDelta.z = 0.0f;
			
			if (zoomDelta != 0.0f)
			{
				SetZoom(zoom + zoomDelta);
				zoomDelta = 0.0f;
			}

			Camera::Update(deltaTime);
		}


		void SetZoom(float newZoom)
		{
			projection = BuildOrthographicProjection(left* newZoom, right * newZoom, bottom * newZoom, top * newZoom, nearPlane, farPlane);
			zoom = newZoom;
		}
	};
}

