#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Frustum
{
	float fov;
	float aspectRatio;
	float near;
	float far;
	glm::mat4 getProjectionMatrix() const;
};

/**
 * A class to represent an abstract viewer with a frustum in 3D space.
 *
 * It serves as a base class for more specific viewers, such as a camera or a light.
 * When it comes to light sources, the frustum is used to generate a shadow map.
 *
 */
class View
{
public:
	View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float fov, float aspectRatio, float near, float far);
	View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Frustum frustum);
	inline glm::vec3 getPosition() const { return position; }
	inline glm::vec3 getDirection() const { return glm::normalize(direction); }
	inline glm::vec3 getUp() const { return glm::normalize(up); }
	inline glm::vec3 getRight() const { return glm::normalize(glm::cross(direction, up)); }
	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
protected:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;

	Frustum frustum;
};