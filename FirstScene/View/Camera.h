#pragma once
#include <algorithm>
#include <glm/gtc/quaternion.hpp>
#include "ViewCone.h"

/*
* This class represents a first-person camera in 3D space.
*
* It has a speed and a sensitivity to control its movement and rotation.
* Pitch and yaw angles are used to calculate the viewing direction.
* Z-axis rotation is not considered in this case.
*
*/
class Camera
{
public:
	Camera(ViewCone viewCone, float speed, float sensitivity);
	inline glm::vec3 getPosition() const { return viewCone.position; }
	inline glm::vec3 getDirection() const { return glm::normalize(viewCone.direction); }
	inline float getFov() const { return viewCone.frustum.fov; }
	inline glm::vec3 getUp() const { return viewCone.up; }
	inline glm::vec3 getRight() const { return viewCone.getRight(); }
	inline void updatePosition(glm::vec3 direction, float deltaTime)
	{
		viewCone.position += speed * deltaTime * glm::normalize(direction);
	}
	void updateDirection(float offsetX, float offsetY);
	void updateFov(float offset);
	inline glm::mat4 getViewMatrix() const { return viewCone.getViewMatrix(); }
	inline glm::mat4 getRearviewMatrix() const { return viewCone.getRearviewMatrix(); }
	inline glm::mat4 getProjectionMatrix() const { return viewCone.getProjectionMatrix(); }
protected:
	ViewCone viewCone;
	float speed;
	float sensitivity;
	float pitch;
	float yaw;
	glm::vec3 initialDirection;
};