#include "View.h"

View::View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float near, float far)
{
	this->position = position;
	this->direction = direction;
	this->up = up;
	this->fov = fov;
	this->aspectRatio = aspectRatio;
	this->near = near;
	this->far = far;
}

glm::mat4 View::getViewMatrix() const
{
	return glm::lookAt(position, position + direction, up);
}

glm::mat4 View::getProjectionMatrix() const
{
	return glm::perspective(fov, aspectRatio, near, far);
}
