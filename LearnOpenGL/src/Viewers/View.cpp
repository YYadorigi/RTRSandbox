#include "View.h"

glm::mat4 Frustum::getProjectionMatrix() const
{
	return glm::perspective(fov, aspectRatio, near, far);
}

View::View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float near, float far)
{
	this->position = position;
	this->direction = direction;
	this->up = up;
	this->frustum = Frustum{ fov, aspectRatio, near, far };
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

glm::mat4 View::getProjectionMatrix() const
{
	return frustum.getProjectionMatrix();
}
