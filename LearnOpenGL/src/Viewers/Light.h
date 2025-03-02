#pragma once
#include "View.h"
#include <glm/gtc/quaternion.hpp>

/*
* A class to represent a point light source in 3D space.
*
* Instead of a direction, it has a target to point to. The lighting effect
* is composed of a color and an intensity.
*
*/
class Light : public View
{
public:
	Light(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio, float near, float far,
		glm::vec3 color, float intensity);
	Light(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color, float intensity);
	inline glm::vec3 getColor() const { return color; }
	inline float getIntensity() const { return intensity; }
	inline void updatePosition(float x, float y, float z) { position = glm::vec3(x, y, z); }
	inline void updateDirection(glm::vec3 target) { direction = glm::normalize(target - position); }
protected:
	glm::vec3 color;
	float intensity;
};

/*
* A subclass of Light to represent a spot light source in 3D space.
*
* It has a cutoff angle and an outer cutoff angle to simulate a cone of light.
* Note that we store the cutoff and outerCutoff in cosine values to avoid
* calling trigonometric functions in the shader.
*
*/
class SpotLight : public Light
{
public:
	SpotLight(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio, float near, float far,
		glm::vec3 color, float intensity, float cutoff, float outerCutoff);
	SpotLight(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color, float intensity,
		float cutoff, float outerCutoff);
	inline float getCutoff() const { return cutoff; }
	inline float getOuterCutoff() const { return outerCutoff; }
protected:
	float cutoff;
	float outerCutoff;
};