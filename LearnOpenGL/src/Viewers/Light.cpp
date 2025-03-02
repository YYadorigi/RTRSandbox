#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio, float near, float far,
	glm::vec3 color, float intensity) : View(position, target - position, up, fov, aspectRatio, near, far)
{
	this->color = color;
	this->intensity = intensity;
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 target, glm::vec3 up, float fov, float aspectRatio, float near, float far,
	glm::vec3 color, float intensity, float cutoff, float outerCutoff) : Light(position, target, up, fov, aspectRatio, near, far, color, intensity)
{
	this->cutoff = cutoff;
	this->outerCutoff = outerCutoff;
}
