#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float speed, float sensitivity);
	inline void updatePosition(glm::vec3 direction, float deltaTime) { position += speed * deltaTime * glm::normalize(direction); }
	void updateFov(float offset);
	void updatePitchYaw(float offsetX, float offsetY);
	void updateDirection();
	inline glm::vec3 getPosition() const { return position; }
	inline glm::vec3 getDirection() const { return glm::normalize(direction); }
	inline glm::vec3 getUp() const { return glm::normalize(up); }
	inline glm::vec3 getRight() const { return glm::normalize(glm::cross(direction, up)); }
	inline glm::mat4 getViewMatrix() const { return glm::lookAt(position, position + direction, up); }
	inline float getFov() const { return fov; }
private:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	float fov;
	float speed;

	glm::vec3 initialDirection;
	float pitch;
	float yaw;
	float sensitivity;
};