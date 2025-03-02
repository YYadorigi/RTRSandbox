#pragma once
#include "View.h"
#include <glm/gtc/quaternion.hpp>

class Camera : public View
{
public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float near, float far,
		float speed, float sensitivity);
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Frustum frustum, float speed, float sensitivity);
	inline void updatePosition(glm::vec3 direction, float deltaTime) { position += speed * deltaTime * glm::normalize(direction); }
	void updateFov(float offset);
	void updatePitchYaw(float offsetX, float offsetY);
	void updateDirection();
protected:
	float speed;

	glm::vec3 initialDirection;
	float pitch;
	float yaw;
	float sensitivity;
};