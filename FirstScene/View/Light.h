#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ViewCone.h"

/*
* This class represents a light source in 3D space.
*
* Its light effect consists of two parts: color and intensity.
*
*/
class Light
{
public:
	glm::vec3 getColor() const;
	float getIntensity() const;
	void updateColor(glm::vec3 color);
	void updateIntensity(float intensity);
protected:
	Light(glm::vec3 color, float intensity);
protected:
	glm::vec3 color;
	float intensity;
};

/*
* This class represents a point light source in 3D space.
*
* Its shadow map is generated as a cube map, called the omnidirectional shadow map.
*
*/
class PointLight : public Light
{
public:
	PointLight(glm::vec3 position, glm::vec3 color, float intensity, Frustum frustum);
	glm::vec3 getPosition() const;
	void updatePosition(glm::vec3 position);
	glm::mat4 getViewMatrix(unsigned int index) const;
	glm::mat4 getProjectionMatrix() const;
protected:
	glm::vec3 position;
	Frustum frustum;
};

/*
* This class represents a spot light source in 3D space.
*
* It has a cutoff angle and an outer cutoff angle for simulating light cones.
* Cutoff and outer cutoff angles are stored as cosine values to avoid trigonometry in shader.
* Instead of a direction vector, we prefer to use a target to describe its directionality.
*
*/
class SpotLight : public Light
{
public:
	SpotLight(ViewCone viewCone, glm::vec3 color, float intensity, float cutoff, float outerCutoff);

	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;
	void updatePosition(glm::vec3 position);
	void updateDirection(glm::vec3 direction);
	void updateTarget(glm::vec3 target);

	float getCutoff() const;
	float getOuterCutoff() const;

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;
protected:
	ViewCone viewCone;
	float cutoff;
	float outerCutoff;
};