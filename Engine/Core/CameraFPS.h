#pragma once
#include "Core/Camera.h"

using namespace glm;

namespace Core
{
	class CameraFPS : public Camera
	{
	private:
		float headingAngle = 3.14f;
		float pitchAngle = 0.0f;

		const float cameraMovementSpeed = 10.1f;
		const float rotationSpeed = 0.001f;

		vec3 translateDelta;

		vec3 forward;
		vec3 right;
		vec3 up;

	public:

		CameraFPS()
		{
			OnMouseMove(0, 0);
		}

		~CameraFPS()
		{
		}

		void OnMouseMove(int deltaX, int deltaY) override 
		{
			// Compute new orientation
			headingAngle += rotationSpeed * float(deltaX);
			pitchAngle += rotationSpeed * float(deltaY);

			// Forward Spherical coordinates to Cartesian coordinates conversion
			forward.x = cos(pitchAngle) * sin(headingAngle);
			forward.y = sin(pitchAngle);
			forward.z = cos(pitchAngle) * cos(headingAngle);

			// Right vector -90deg from forward
			right.x = sin(headingAngle - 3.14f / 2.0f);
			right.y = 0.0f;//always zero due to right being horizontal (on xz plane)
			right.z = cos(headingAngle - 3.14f / 2.0f);

			// Up vector
			up = glm::cross(right, forward);
		}

		void OnKey(unsigned char key, int x, int y) override
		{
			if (key == 'a')
				translateDelta.x += cameraMovementSpeed;
			if (key == 'd')
				translateDelta.x -= cameraMovementSpeed;
			if (key == 'w')
				translateDelta.y -= cameraMovementSpeed;
			if (key == 's')
				translateDelta.y += cameraMovementSpeed;
			if (key == 'q')
				translateDelta.z += cameraMovementSpeed;
			if (key == 'e')
				translateDelta.z -= cameraMovementSpeed;
		}

		void Update(float deltaTime) override
		{
			Translate(translateDelta * deltaTime);
			translateDelta.x = 0.0f;
			translateDelta.y = 0.0f;
			translateDelta.z = 0.0f;
			
			vec3 position(world[3]);
			
			view = glm::lookAt(
				position,           // Camera is here
				position + forward, // and looks here : at the same position, plus "direction"
				up                  // Head is up (set to 0,-1,0 to look upside-down)
				);
				
			worldToProjection = projection * view;
			//Camera::Update(deltaTime);
		}
	};
}
