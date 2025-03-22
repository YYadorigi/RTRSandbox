#include "utils.h"

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform)
{
	shader.use();
	shader.setTransform("model", glm::value_ptr(transform));
	shader.setTransform("invModel", glm::value_ptr(glm::inverse(transform)));
	model.draw(shader);
}

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform, std::vector<glm::vec3>& translations)
{
	shader.use();
	shader.setTransform("model", glm::value_ptr(transform));
	shader.setTransform("invModel", glm::value_ptr(glm::inverse(transform)));
	model.draw(shader, translations);
}

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform, std::vector<glm::vec3>&& translations)
{
	shader.use();
	shader.setTransform("model", glm::value_ptr(transform));
	shader.setTransform("invModel", glm::value_ptr(glm::inverse(transform)));
	model.draw(shader, translations);
}

unsigned int uniformOffset(unsigned int increase, bool reset)
{
	static unsigned int offset = 0;
	if (reset) {
		offset = 0;
	}
	unsigned int temp = offset;
	offset += increase;
	return temp;
}

void loadPointLightUniforms(UniformBuffer& lightsUBO, const PointLight& pointLight, bool active)
{
	lightsUBO.setData(
		glm::value_ptr(pointLight.getPosition()),
		sizeof(glm::vec4),
		uniformOffset(sizeof(glm::vec4))
	);
	lightsUBO.setData(
		glm::value_ptr(pointLight.getColor()),
		sizeof(glm::vec3),
		uniformOffset(sizeof(glm::vec3))
	);
	lightsUBO.setData(
		&static_cast<const float&>(pointLight.getIntensity() * active),
		sizeof(float),
		uniformOffset(sizeof(float))
	);
}

void loadSpotLightUniforms(UniformBuffer& lightsUBO, const SpotLight& spotLight, bool active)
{
	lightsUBO.setData(
		glm::value_ptr(spotLight.getPosition()),
		sizeof(glm::vec4),
		uniformOffset(sizeof(glm::vec4))
	);
	lightsUBO.setData(
		glm::value_ptr(spotLight.getDirection()),
		sizeof(glm::vec4),
		uniformOffset(sizeof(glm::vec4))
	);
	lightsUBO.setData(
		glm::value_ptr(spotLight.getColor()),
		sizeof(glm::vec3),
		uniformOffset(sizeof(glm::vec3))
	);
	lightsUBO.setData(
		&static_cast<const float&>(spotLight.getIntensity() * active),
		sizeof(float),
		uniformOffset(sizeof(float))
	);
	lightsUBO.setData(
		&static_cast<const float&>(spotLight.getCutoff()),
		sizeof(float),
		uniformOffset(sizeof(float))
	);
	lightsUBO.setData(
		&static_cast<const float&>(spotLight.getOuterCutoff()),
		sizeof(float),
		uniformOffset(sizeof(glm::vec4) - sizeof(float))
	);
	lightsUBO.setData(
		glm::value_ptr(spotLight.getViewMatrix()),
		sizeof(glm::mat4),
		uniformOffset(sizeof(glm::mat4))
	);
	lightsUBO.setData(
		glm::value_ptr(spotLight.getProjectionMatrix()),
		sizeof(glm::mat4),
		uniformOffset(sizeof(glm::mat4))
	);
}
