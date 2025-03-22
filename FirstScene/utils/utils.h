#pragma once
#include "Model/Model.h"
#include "Shader/Shader.h"
#include "Shader/UniformBuffer.h"
#include "View/Light.h"

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform);

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform, std::vector<glm::vec3>& translations);

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform, std::vector<glm::vec3>&& translations);

unsigned int uniformOffset(unsigned int increase, bool reset = false);

void loadPointLightUniforms(UniformBuffer& lightsUBO, const PointLight& pointLight, bool active = true);

void loadSpotLightUniforms(UniformBuffer& lightsUBO, const SpotLight& spotLight, bool active = true);

template<typename T>
void loadViewProjUniforms(UniformBuffer& viewProjUBO, const T& camera)
{
	viewProjUBO.setData(
		glm::value_ptr(camera.getViewMatrix()),
		sizeof(glm::mat4),
		uniformOffset(sizeof(glm::mat4))
	);
	viewProjUBO.setData(
		glm::value_ptr(camera.getProjectionMatrix()),
		sizeof(glm::mat4),
		uniformOffset(sizeof(glm::mat4))
	);
}