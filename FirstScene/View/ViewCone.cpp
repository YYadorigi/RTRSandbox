#include "ViewCone.h"

glm::mat4 Frustum::getProjectionMatrix() const
{
	return glm::perspective(glm::radians(fov), aspectRatio, nearDist, farDist);
}

glm::vec3 ViewCone::getRight() const
{
	return glm::normalize(glm::cross(direction, up));
}

glm::mat4 ViewCone::getViewMatrix() const
{
	return glm::lookAt(position, position + direction, up);
}

glm::mat4 ViewCone::getRearviewMatrix() const
{
	return glm::lookAt(position, position - direction, up);
}

glm::mat4 ViewCone::getProjectionMatrix() const
{
	return frustum.getProjectionMatrix();
}