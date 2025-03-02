#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float near, float far,
	float speed, float sensitivity) : View(position, direction, up, fov, aspectRatio, near, far)
{
	this->speed = speed;
	this->sensitivity = sensitivity;
	initialDirection = direction;
	pitch = 0.0f;
	yaw = 0.0f;
}

void Camera::updateFov(float offset)
{
	fov -= offset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 60.0f)
		fov = 60.0f;
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
	glm::quat qPitch = glm::angleAxis(glm::radians(pitch), getRight());
	glm::quat qYaw = glm::angleAxis(glm::radians(yaw), -getUp());
	glm::quat qResult = qPitch * qYaw;
	direction = qResult * initialDirection;
}
