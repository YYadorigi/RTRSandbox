#include "Light.h"

glm::vec3 Light::getColor() const
{
	return color;
}

float Light::getIntensity() const
{
	return intensity;
}

void Light::updateColor(glm::vec3 color)
{
	this->color = color;
}

void Light::updateIntensity(float intensity)
{
	this->intensity = intensity;
}

Light::Light(glm::vec3 color, float intensity) : color(color), intensity(intensity)
{}

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity, Frustum frustum) :
	Light(color, intensity), position(position), frustum(frustum)
{}

glm::vec3 PointLight::getPosition() const
{
	return position;
}

void PointLight::updatePosition(glm::vec3 position)
{
	this->position = position;
}

glm::mat4 PointLight::getViewMatrix(unsigned int index) const
{
	static const glm::vec3 directions[] = {
		   glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
		   glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		   glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)
	};	// right, left, up, down, front, back
	static const glm::vec3 ups[] = {
		   glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		   glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
		   glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)
	};	// corresponding up directions of each face
	return glm::lookAt(position, position + directions[index], ups[index]);
}

glm::mat4 PointLight::getProjectionMatrix() const
{
	return frustum.getProjectionMatrix();
}

SpotLight::SpotLight(ViewCone viewCone, glm::vec3 color, float intensity, float cutoff, float outerCutoff) :
	Light(color, intensity), viewCone(viewCone), cutoff(cutoff), outerCutoff(outerCutoff)
{}

glm::vec3 SpotLight::getPosition() const
{
	return viewCone.position;
}

glm::vec3 SpotLight::getDirection() const
{
	return glm::normalize(viewCone.direction);
}

void SpotLight::updatePosition(glm::vec3 position)
{
	viewCone.position = position;
}

void SpotLight::updateDirection(glm::vec3 direction)
{
	viewCone.direction = direction;
}

void SpotLight::updateTarget(glm::vec3 target)
{
	viewCone.direction = target - viewCone.position;
}

float SpotLight::getCutoff() const
{
	return cutoff;
}

float SpotLight::getOuterCutoff() const
{
	return outerCutoff;
}

glm::mat4 SpotLight::getViewMatrix() const
{
	return viewCone.getViewMatrix();
}

glm::mat4 SpotLight::getProjectionMatrix() const
{
	return viewCone.getProjectionMatrix();
}
