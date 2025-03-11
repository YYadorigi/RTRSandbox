#pragma once
#include "View.h"

/*
* This class represents a first-person camera in 3D space.
*
* It has a speed and a sensitivity to control movement and the rotation.
* Pitch and yaw angles are used to calculate the viewing direction.
* Z-axis rotation is not considered in this case.
*
*/
class Camera : public View
{
public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Frustum frustum, float speed, float sensitivity);
	inline void updatePosition(glm::vec3 direction, float deltaTime) { position += speed * deltaTime * glm::normalize(direction); }
	void updateDirection(float offsetX, float offsetY);
	void updateFov(float offset);
protected:
	float speed;
	float pitch;
	float yaw;
	float sensitivity;
	glm::vec3 initialDirection;
};