#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;
	float getFov() const;
	glm::vec3 getUp() const;
	glm::vec3 getRight() const;
	void updatePosition(glm::vec3 direction, float deltaTime);
	void updateDirection(float offsetX, float offsetY);
	void updateFov(float offset);

	glm::mat4 getViewMatrix() const;
	glm::mat4 getRearviewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
protected:
	ViewCone viewCone;
	float speed;
	float sensitivity;
	float pitch;
	float yaw;
	glm::vec3 initialDirection;
};