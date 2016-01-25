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
		float defaultHeadingAngle = 3.14f;
		float headingAngle = defaultHeadingAngle;
		float defaultPitchAngle = 0.0f;
		float pitchAngle = defaultPitchAngle;

		void RotateSpherical(float headingAngleDelta, float pitchAngleDelta)
		{
			// Compute new orientation
			headingAngle += headingAngleDelta;
			pitchAngle += pitchAngleDelta;

			if (!parentGameObject.expired())
			{
				mat4& world = parentGameObject.lock()->GetWorldTransform();

				world = rotate(world, headingAngleDelta, up);
				world = rotate(world, pitchAngleDelta, right);
			}

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

		void RotateQuaternion(float headingAngleDelta, float pitchAngleDelta)
		{
			const vec3 Vaxis(0.0f, 1.0f, 0.0f); //always rotate about a fixed up vector (FPS style)

			// Compute new orientation
			headingAngle += headingAngleDelta;
			pitchAngle += pitchAngleDelta;

			//create rotation heading angle about the vertical axis
			vec3 tmpFwd(0.0f, 0.0f, 1.0f); //default fwd vector
			quat h = glm::angleAxis(headingAngleDelta, Vaxis);
			tmpFwd = normalize(h * tmpFwd);

			vec3 Haxis = normalize(cross(tmpFwd, Vaxis)); //get updated horizontal axis

			//create rotation pitch angle about the horizontal axis
			quat p = glm::angleAxis(pitchAngleDelta, Haxis);
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
		}


		void Rotate(float heading, float pitch)
		{
			heading = radians(heading);
			pitch = radians(pitch);

			if (useQuaternionRotation)
				RotateQuaternion(heading, pitch);
			else
				RotateSpherical(heading, pitch);
		}

		void SetOrientation(float hAngle, float pAngle)
		{
			this->headingAngle = defaultHeadingAngle + hAngle;
			this->pitchAngle = defaultPitchAngle + pAngle;

			// Compute new orientation
			if (!parentGameObject.expired())
			{
				mat4 currentWorld = parentGameObject.lock()->GetWorldTransform();
				//extract scaling factor (assumes positive scale)
				float sx = length(currentWorld[0]);
				float sy = length(currentWorld[1]);
				float sz = length(currentWorld[2]);

				mat4 world;
				world = scale(world, vec3(sx, sy, sz));
				world = rotate(world, headingAngle, up);
				world = rotate(world, pitchAngle, right);
				world[3] = parentGameObject.lock()->GetWorldTransform()[3]; //copy over position;
				parentGameObject.lock()->GetWorldTransform() = world;
			}

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

		void MoveRight(float amount) { translateDelta.x += amount; }

		void MoveUp(float amount) { translateDelta.y += amount; }

		void MoveForward(float amount) { translateDelta.z += amount; }

		vec3& GetForward() { return forward; }
		vec3& GetRight() { return right; }
		vec3& GetUp() { return up; }

		float& GetHeadingAngle() { return headingAngle; }
		float& GetPitchAngle() { return pitchAngle; }

		ComponentTypes GetComponentType() const override { return DirectionalMovement; }


	};
}
