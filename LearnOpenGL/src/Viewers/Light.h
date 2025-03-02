#pragma once
#include "View.h"
#include <glm/gtc/quaternion.hpp>

class Light : public View
{
public:
	Light(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio, float near, float far,
		glm::vec3 color, float intensity);
	inline glm::vec3 getColor() const { return color; }
	inline float getIntensity() const { return intensity; }
	inline void updatePosition(float x, float y, float z) { position = glm::vec3(x, y, z); }
	inline void updateDirection(glm::vec3 target) { direction = glm::normalize(target - position); }
protected:
	glm::vec3 color;
	float intensity;
};
