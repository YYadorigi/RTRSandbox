#include <algorithm>
#include <glm/gtc/quaternion.hpp>
#include "Camera.h"

Camera::Camera(ViewCone viewCone, float speed, float sensitivity) :
	viewCone(viewCone), speed(speed), sensitivity(sensitivity)
{
	initialDirection = viewCone.direction;
	pitch = 0.0f;
	yaw = 0.0f;
}

glm::vec3 Camera::getPosition() const
{
	return viewCone.position;
}

glm::vec3 Camera::getDirection() const
{
	return glm::normalize(viewCone.direction);
}

float Camera::getFov() const
{
	return viewCone.frustum.fov;
}

glm::vec3 Camera::getUp() const
{
	return viewCone.up;
}

glm::vec3 Camera::getRight() const
{
	return viewCone.getRight();
}

void Camera::updatePosition(glm::vec3 direction, float deltaTime)
{
	viewCone.position += speed * deltaTime * glm::normalize(direction);
}

void Camera::updateDirection(float offsetX, float offsetY)
{
	pitch += offsetY * sensitivity;
	yaw += offsetX * sensitivity;
	pitch = std::clamp(pitch, -85.0f, 85.0f);

	// Quaternion rotation
	glm::quat qPitch = glm::angleAxis(glm::radians(pitch), viewCone.getRight());
	glm::quat qYaw = glm::angleAxis(glm::radians(yaw), -viewCone.up);
	glm::quat qResult = qPitch * qYaw;
	viewCone.direction = qResult * initialDirection;
}

void Camera::updateFov(float offset)
{
	viewCone.frustum.fov -= offset;
	viewCone.frustum.fov = std::clamp(viewCone.frustum.fov, 1.0f, 60.0f);
}

glm::mat4 Camera::getViewMatrix() const
{
	return viewCone.getViewMatrix();
}

glm::mat4 Camera::getRearviewMatrix() const
{
	return viewCone.getRearviewMatrix();
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return viewCone.getProjectionMatrix();
}
