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
		const float rotationSpeed = 0.005f;

		vec3 translateDelta;

		vec3 forward;
		vec3 right;
		vec3 up;

		void RotateSpherical(int deltaX, int deltaY)
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
			up = cross(right, forward);
		}

		void RotateQuaternion(int deltaX, int deltaY)
		{
			const vec3 Vaxis(0.0f, 1.0f, 0.0f); //always rotate about a fixed up vector (FPS style)

			// Compute new orientation
			headingAngle += rotationSpeed * float(deltaX);
			pitchAngle += rotationSpeed * float(deltaY);

			//create rotation heading angle about the vertical axis
			vec3 tmpFwd(0.0f, 0.0f, 1.0f); //default fwd vector
			quat q = glm::angleAxis(headingAngle, Vaxis);
			tmpFwd = normalize(q * tmpFwd);

			vec3 Haxis = normalize(cross(tmpFwd, Vaxis)); //get updated horizontal axis

			//create rotation pitch angle about the horizontal axis
			q = glm::angleAxis(pitchAngle, Haxis);
			tmpFwd = normalize(q * tmpFwd);

			forward = normalize(tmpFwd);
			up = normalize(cross(Haxis, forward)); //get updated up axis
			right = Haxis;
		}

	public:
		bool useQuaternionRotation = true;

		CameraFPS()
		{
			CameraFPS::OnMouseMove(0, 0);
		}

		~CameraFPS()
		{
		}

		void OnMouseMove(int deltaX, int deltaY) override 
		{

			Rotate(-deltaX, -deltaY);
		}

		void OnKey(unsigned char key, int x, int y) override
		{
			if (key == 'a')
				translateDelta -= right * cameraMovementSpeed;
			if (key == 'd')
				translateDelta += right * cameraMovementSpeed;
			if (key == 'w')
				translateDelta += forward * cameraMovementSpeed;
			if (key == 's')
				translateDelta -= forward * cameraMovementSpeed;
			if (key == 'z')
				translateDelta += up * cameraMovementSpeed;
			if (key == 'c')
				translateDelta -= up * cameraMovementSpeed;
			if (key == 'q')
				Rotate(30, 0);
			if (key == 'e')
				Rotate(-30, 0);
		}

		void Rotate(int deltaX, int deltaY)
		{
			if (useQuaternionRotation)
				RotateQuaternion(deltaX, deltaY);
			else
				RotateSpherical(deltaX, deltaY);
		}

		void Update(float deltaTime) override
		{
			Translate(translateDelta * deltaTime);
			translateDelta.x = 0.0f;
			translateDelta.y = 0.0f;
			translateDelta.z = 0.0f;
			
			view = buildViewLookAt();
				
			worldToProjection = projection * view;
			//Camera::Update(deltaTime);
		}

		mat4 buildViewManual()
		{
			mat4 newView;
			vec3 pos(world[3]);

			vec3 N = forward;
			glm::normalize(N);
			vec3 U = up;
			U = cross(normalize(U), N);
			vec3 V = cross(N, U);

			newView[0][0] = U.x;
			newView[1][0] = U.y;
			newView[2][0] = U.z;

			newView[0][1] = V.x;
			newView[1][1] = V.y;
			newView[2][1] = V.z;

			newView[0][2] = -N.x;
			newView[1][2] = -N.y;
			newView[2][2] = -N.z;
			//newView[0][2] = -N.x;
			//newView[1][2] = -N.y;
			//newView[2][2] = -N.z;
			newView[3][0] = -dot(V, pos);
			newView[3][1] = -dot(U, pos);
			newView[3][2] = dot(N, pos);
			//newView[3][0] = pos.x;
			//newView[3][1] = pos.y;
			//newView[3][2] = pos.z;
			newView[3][3] = 1.0f;


			return newView;
		}

		mat4 buildViewLookAt()
		{
			vec3 position(world[3]);
			return glm::lookAt(
				position,           // Camera is here
				position + forward, // and looks here : at the same position, plus "direction"
				up                  // Head is up (set to 0,-1,0 to look upside-down)
				);
		}
	};
}
