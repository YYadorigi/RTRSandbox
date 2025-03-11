#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

struct Frustum
{
	float fov;
	float aspectRatio;
	float nearDist;
	float farDist;
	glm::mat4 getProjectionMatrix() const;
};

/**
 * This class represents an abstract observer with a view cone (frustum) in 3D space.
 *
 * Various concrete forms of observation can be derived from it, such as cameras and lights.
 * View cones can be used to generate shadow maps of light sources.
 *
 */
class View
{
public:
	virtual ~View() = default;
	inline glm::vec3 getPosition() const { return position; }
	inline glm::vec3 getDirection() const { return glm::normalize(direction); }
	inline glm::vec3 getUp() const { return glm::normalize(up); }
	inline glm::vec3 getRight() const { return glm::normalize(glm::cross(direction, up)); }
	glm::mat4 getViewMatrix() const;
	glm::mat4 getRearviewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
protected:
	View(glm::vec3 position, glm::vec3 direction, glm::vec3 up, Frustum frustum);
	View(const View& other) = default;
	View& operator=(const View& other) = default;
	View(View&& other) noexcept = default;
	View& operator=(View&& other) noexcept = default;
protected:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;
	Frustum frustum;
};