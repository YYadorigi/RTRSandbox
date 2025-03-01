#include <camera.h>

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float speed, float sensitivity)
{
	this->position = position;
	this->direction = direction;
	this->initialDirection = direction;
	this->up = up;
	this->speed = speed;
	this->sensitivity = sensitivity;
	this->pitch = 0.0f;
	this->yaw = 0.0f;
}

void Camera::updatePitchYaw(float offsetX, float offsetY)
{
	pitch += offsetY * sensitivity;
	yaw += offsetX * sensitivity;
	if (pitch > 89.0f) {
		pitch = 89.0f;
	} else if (pitch < -89.0f) {
		pitch = -89.0f;
	}
}

void Camera::updateDirection()
{
	// Quaternion rotation
	glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, -1.0f, 0.0f));
	glm::quat qResult = qPitch * qYaw;
	direction = qResult * initialDirection;
}
