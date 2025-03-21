#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
* This struct represents a frustum in 3D space.
*/
struct Frustum
{
	float fov;
	float aspectRatio;
	float nearDist;
	float farDist;
	inline glm::mat4 getProjectionMatrix() const
	{
		return glm::perspective(glm::radians(fov), aspectRatio, nearDist, farDist);
	}
};

/**
 * This struct represents a view cone (frustum) in 3D space.
 *
 * It can serve as the core component of various forms of view, such as
 * cameras and lights. In the case of lights, it is used to generate shadow maps.
 *
 */
struct ViewCone
{
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	Frustum frustum;
	inline glm::vec3 getRight() const { return glm::normalize(glm::cross(direction, up)); }
	inline glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(position, position + direction, up);
	}
	inline glm::mat4 getRearviewMatrix() const
	{
		return glm::lookAt(position, position - direction, up);
	}
	inline glm::mat4 getProjectionMatrix() const
	{
		return frustum.getProjectionMatrix();
	}
};