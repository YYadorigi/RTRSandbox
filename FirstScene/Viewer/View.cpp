#include "View.h"

glm::mat4 Frustum::getProjectionMatrix() const
{
	return glm::perspective(glm::radians(fov), aspectRatio, nearDist, farDist);
}

View::View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float nearDist, float farDist)
{
	this->position = position;
	this->direction = direction;
	this->up = up;
	this->frustum = Frustum{ fov, aspectRatio, nearDist, farDist };
}

View::View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Frustum frustum)
{
	this->position = position;
	this->direction = direction;
	this->up = up;
	this->frustum = frustum;
}

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
