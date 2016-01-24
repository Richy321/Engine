#pragma once
#include "Interfaces/IComponent.h"
#include "../../Dependencies/glm/glm.hpp"
#include "../../Dependencies/glm/gtc/quaternion.hpp"

using namespace glm;
namespace Core
{
	class DirectionalMovementComponent : public Core::IComponent
	{
	private:
		vec3 forward;
		vec3 right;
		vec3 up;
		vec3 translateDelta;
		float headingAngle = 3.14f;
		float pitchAngle = 0.0f;

		void RotateSpherical(float deltaX, float deltaY)
		{
			// Compute new orientation
			headingAngle += float(deltaX);
			pitchAngle += float(deltaY);

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

		void RotateQuaternion(float deltaX, float deltaY)
		{
			const vec3 Vaxis(0.0f, 1.0f, 0.0f); //always rotate about a fixed up vector (FPS style)

			// Compute new orientation
			headingAngle += deltaX;
			pitchAngle += deltaY;

			//create rotation heading angle about the vertical axis
			vec3 tmpFwd(0.0f, 0.0f, 1.0f); //default fwd vector
			quat h = glm::angleAxis(headingAngle, Vaxis);
			tmpFwd = normalize(h * tmpFwd);

			vec3 Haxis = normalize(cross(tmpFwd, Vaxis)); //get updated horizontal axis

			//create rotation pitch angle about the horizontal axis
			quat p = glm::angleAxis(pitchAngle, Haxis);
			tmpFwd = normalize(p * tmpFwd);

			forward = normalize(tmpFwd);
			up = normalize(cross(forward, Haxis)); //get updated up axis
			right = Haxis;

			h *= p;
			mat4 rot(glm::mat4_cast(h));
			if(!parentGameObject.expired())
				parentGameObject.lock()->GetWorldTransform() *= rot;
		}

		mat4 buildViewLookAt() const
		{
			vec3 position(parentGameObject.lock()->GetWorldTransform()[3]);
			return glm::lookAt(
				position,           // Camera is here
				position + forward, // and looks here : at the same position, plus "direction"
				up                  // Head is up (set to 0,-1, 0 to look upside-down)
				);
		}
	public:
		bool useQuaternionRotation = true;


		DirectionalMovementComponent(std::weak_ptr<Core::IGameObject> gameObj) : IComponent(gameObj)
		{
			translateDelta = vec3(0.0f, 0.0f, 0.0f);
			Rotate(0.0f, 0.0f);
		}

		~DirectionalMovementComponent()
		{
		}

		void Update(float deltaTime) override
		{
			parentGameObject.lock()->Translate(translateDelta * deltaTime);
			translateDelta.x = 0.0f;
			translateDelta.y = 0.0f;
			translateDelta.z = 0.0f;

			//parentGameObject.lock()->GetWorldTransform() = buildViewLookAt();
		}


		void Rotate(float heading, float pitch)
		{
			if (useQuaternionRotation)
				RotateQuaternion(heading, pitch);
			else
				RotateSpherical(heading, pitch);
		}

		void MoveRight(float amount) { translateDelta.x += amount; }

		void MoveUp(float amount) { translateDelta.y += amount; }

		void MoveForward(float amount) { translateDelta.z += amount; }


		vec3 GetForward() const { return forward; }
		vec3 GetRight() const { return right; }
		vec3 GetUp() const { return up; }

		ComponentTypes GetComponentType() const override { return DirectionalMovement; }
	};
}
