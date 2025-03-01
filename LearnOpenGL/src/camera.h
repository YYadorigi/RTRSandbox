#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float speed);
	inline void updatePosition(glm::vec3 direction) { position += speed * glm::normalize(direction); }
	inline glm::vec3 getPosition() const { return position; }
	inline glm::vec3 getDirection() const { return direction; }
	inline glm::vec3 getUp() const { return up; }
	inline glm::mat4 getViewMatrix() const { return glm::lookAt(position, position + direction, up); }
private:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	float speed;
};