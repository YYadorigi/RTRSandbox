#include "Light.h"

Light::Light(glm::vec3 color, float intensity) : color(color), intensity(intensity)
{}

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity, Frustum frustum) :
	Light(color, intensity), position(position), frustum(frustum)
{}

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

SpotLight::SpotLight(ViewCone viewCone, glm::vec3 color, float intensity, float cutoff, float outerCutoff) :
	Light(color, intensity), viewCone(viewCone), cutoff(cutoff), outerCutoff(outerCutoff)
{}
