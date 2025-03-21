#include "Camera.h"

Camera::Camera(ViewCone viewCone, float speed, float sensitivity) :
	viewCone(viewCone), speed(speed), sensitivity(sensitivity)
{
	initialDirection = viewCone.direction;
	pitch = 0.0f;
	yaw = 0.0f;
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
