#include <camera.h>

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float speed)
{
	this->position = position;
	this->direction = direction;
	this->up = up;
	this->speed = speed;
}