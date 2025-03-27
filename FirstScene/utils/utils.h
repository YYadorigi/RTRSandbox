#pragma once
#include "Model/Model.h"
#include "Shader/Shader.h"
#include "Shader/UniformBuffer.h"
#include "View/Light.h"
#include "View/Camera.h"

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform);

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform, std::vector<glm::vec3>& translations);

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform, std::vector<glm::vec3>&& translations);

unsigned int uniformOffset(unsigned int increase, bool reset = false);

void loadPointLightUniforms(UniformBuffer& lightsUBO, const PointLight& pointLight, bool active = true);

void loadSpotLightUniforms(UniformBuffer& lightsUBO, const SpotLight& spotLight, bool active = true);

template<typename T>
concept Viewer = requires(T viewer)
{
	{ viewer.getViewMatrix() } -> std::same_as<glm::mat4>;
	{ viewer.getProjectionMatrix() } -> std::same_as<glm::mat4>;
};

template<Viewer T>
void loadViewProjUniforms(UniformBuffer& viewProjUBO, const T& viewer)
{
	viewProjUBO.setData(
		glm::value_ptr(viewer.getViewMatrix()),
		sizeof(glm::mat4),
		uniformOffset(sizeof(glm::mat4))
	);
	viewProjUBO.setData(
		glm::value_ptr(viewer.getProjectionMatrix()),
		sizeof(glm::mat4),
		uniformOffset(sizeof(glm::mat4))
	);
}