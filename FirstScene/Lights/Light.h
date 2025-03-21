#pragma once
#include "Camera/ViewCone.h"

/*
* This class represents a light source in 3D space.
*
* Its light effect consists of two parts: color and intensity.
*
*/
class Light
{
public:
	inline glm::vec3 getColor() const { return color; }
	inline float getIntensity() const { return intensity; }
	inline void updateColor(glm::vec3 color) { this->color = color; }
	inline void updateIntensity(float intensity) { this->intensity = intensity; }
protected:
	Light(glm::vec3 color, float intensity);
protected:
	glm::vec3 color;
	float intensity;
};

/*
* This class represents a point light source in 3D space.
*
* Its shadow map is generated as a cube map, called the
* omnidirectional shadow map.
*
*/
class PointLight : public Light
{
public:
	PointLight(glm::vec3 position, glm::vec3 color, float intensity, Frustum frustum);
	inline glm::vec3 getPosition() const { return position; }
	inline void updatePosition(glm::vec3 position) { this->position = position; }
	glm::mat4 getViewMatrix(unsigned int index) const;
	inline glm::mat4 getProjectionMatrix() const { return frustum.getProjectionMatrix(); }
protected:
	glm::vec3 position;
	Frustum frustum;
};

/*
* This class represents a spot light source in 3D space.
*
* It has a cutoff angle and an outer cutoff angle for simulating light cones.
* Note that we store the cutoff and outer cutoff angles with cosine values
* to avoid calling trigonometric functions in the shader.Instead of a direction vector, we sometimes prefer to use a target coordinate
* to describe its directionality.
*
*/
class SpotLight : public Light
{
public:
	SpotLight(ViewCone viewCone, glm::vec3 color, float intensity, float cutoff, float outerCutoff);
	inline glm::vec3 getPosition() const { return viewCone.position; }
	inline glm::vec3 getDirection() const { return glm::normalize(viewCone.direction); }
	inline void updatePosition(glm::vec3 position) { viewCone.position = position; }
	inline void updateDirection(glm::vec3 direction) { viewCone.direction = direction; }
	inline void updateTarget(glm::vec3 target) { viewCone.direction = target - viewCone.position; }
	inline float getCutoff() const { return cutoff; }
	inline float getOuterCutoff() const { return outerCutoff; }
	inline glm::mat4 getViewMatrix() const { return viewCone.getViewMatrix(); }
	inline glm::mat4 getProjectionMatrix() const { return viewCone.getProjectionMatrix(); }
protected:
	ViewCone viewCone;
	float cutoff;
	float outerCutoff;
};