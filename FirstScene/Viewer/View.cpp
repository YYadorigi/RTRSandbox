#include "View.h"

glm::mat4 Frustum::getProjectionMatrix() const
{
	return glm::perspective(glm::radians(fov), aspectRatio, nearDist, farDist);
}

View::View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Frustum frustum) :
	position(position), direction(direction), up(up), frustum(frustum)
{}

glm::mat4 View::getViewMatrix() const
{
	return glm::lookAt(position, position + direction, up);
}

glm::mat4 View::getRearviewMatrix() const
{

	return glm::lookAt(position, position - direction, up);
}

glm::mat4 View::getProjectionMatrix() const
{
	return frustum.getProjectionMatrix();
}
