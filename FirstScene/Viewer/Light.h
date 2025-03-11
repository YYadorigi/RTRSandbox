#pragma once
#include "View.h"

enum class UpdateType
{
	DIRECTION,
	TARGET,
};

/*
* This class represents a light source in 3D space.
*
* Instead of a direction vector, we prefer to use a target coordinate to describe
* its directionality. Its light effect consists of two parts: color and intensity.
*
*/
class Light : public View
{
public:
	inline glm::vec3 getColor() const { return color; }
	inline float getIntensity() const { return intensity; }
	inline void updatePosition(glm::vec3 position) { this->position = position; }
	void updateDirection(glm::vec3 vec, UpdateType type = UpdateType::DIRECTION);
protected:
	Light(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color, float intensity);
protected:
	glm::vec3 color;
	float intensity;
};

/*
* This class represents a point light source in 3D space.
*/
class PointLight : public Light
{
public:
	PointLight(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color, float intensity);
};

/*
* This class represents a spot light source in 3D space.
*
* It has a cutoff angle and an outer cutoff angle for simulating light cones.
* Note that we store the cutoff and outer cutoff angles with cosine values
* to avoid calling trigonometric functions in the shader.
*
*/
class SpotLight : public Light
{
public:
	SpotLight(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color,
		float intensity, float cutoff, float outerCutoff);
	inline float getCutoff() const { return cutoff; }
	inline float getOuterCutoff() const { return outerCutoff; }
protected:
	float cutoff;
	float outerCutoff;
};

/*
* This struct represents an ambient lighting effect in a scene.
*/
struct AmbientLight
{
	glm::vec3 color;
	float intensity;
};

/*
* This struct represents a directional lighting effect in a scene.
*/
struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec3 color;
	float intensity;
};