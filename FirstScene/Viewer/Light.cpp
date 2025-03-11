#include "Light.h"

void Light::updateDirection(glm::vec3 vec, UpdateType type)
{
	if (type == UpdateType::DIRECTION) {
		direction = glm::normalize(vec);
	} else if (type == UpdateType::TARGET) {
		direction = glm::normalize(vec - position);
	}
}

Light::Light(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color, float intensity) :
	View(position, target - position, up, frustum), color(color), intensity(intensity)
{}

PointLight::PointLight(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color, float intensity) :
	Light(position, target, up, frustum, color, intensity)
{}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 target, glm::vec3 up, Frustum frustum, glm::vec3 color,
	float intensity, float cutoff, float outerCutoff) :
	Light(position, target, up, frustum, color, intensity), cutoff(cutoff), outerCutoff(outerCutoff)
{}
