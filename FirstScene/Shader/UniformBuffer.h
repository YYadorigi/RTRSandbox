#pragma once
#include "View/Light.h"

template<typename T>
concept Viewer = requires(T viewer)
{
	{ viewer.getViewMatrix() } -> std::same_as<glm::mat4>;
	{ viewer.getProjectionMatrix() } -> std::same_as<glm::mat4>;
};

template <typename T>
concept Lighting = std::is_base_of_v<Light, std::decay_t<T>> && !std::is_same_v<std::decay_t<T>, Light>;

class UniformBuffer
{
public:
	UniformBuffer(unsigned int size);
	~UniformBuffer();
	UniformBuffer(const UniformBuffer& other) = delete;
	UniformBuffer& operator=(const UniformBuffer& other) = delete;
	UniformBuffer(UniformBuffer&& other) noexcept;
	UniformBuffer& operator=(UniformBuffer&& other) noexcept;

	void loadData(const void* data, unsigned int size);
	template<Viewer T>
	void loadViewProjUniforms(T&& viewer);
	template <Lighting T>
	void loadLightUniforms(T&& light, bool active = true);

	void resetOffset();
	void alignOffset(unsigned int alignment);

	void bind(unsigned int bindingPoint) const;
private:
	unsigned int UBO;
	unsigned int offset = 0;
};

template<Viewer T>
void UniformBuffer::loadViewProjUniforms(T&& viewer)
{
	loadData(glm::value_ptr(viewer.getViewMatrix()), sizeof(glm::mat4));
	loadData(glm::value_ptr(viewer.getProjectionMatrix()), sizeof(glm::mat4));
}

template<Lighting T>
void UniformBuffer::loadLightUniforms(T&& light, bool active)
{
	if constexpr (std::is_same_v<std::decay_t<T>, PointLight>) {
		loadData(glm::value_ptr(light.getPosition()), sizeof(glm::vec4));
		loadData(glm::value_ptr(light.getColor()), sizeof(glm::vec3));
		float intensity = light.getIntensity() * active;
		loadData(&intensity, sizeof(float));
	}
	else if constexpr (std::is_same_v<std::decay_t<T>, SpotLight>) {
		loadData(glm::value_ptr(light.getPosition()), sizeof(glm::vec4));
		loadData(glm::value_ptr(light.getDirection()), sizeof(glm::vec4));
		loadData(glm::value_ptr(light.getColor()), sizeof(glm::vec3));
		float intensity = light.getIntensity() * active;
		loadData(&intensity, sizeof(float));
		float cutoff = light.getCutoff();
		loadData(&cutoff, sizeof(float));
		float outerCutoff = light.getOuterCutoff();
		loadData(&outerCutoff, sizeof(float));
		alignOffset(sizeof(glm::vec4));
		loadData(glm::value_ptr(light.getViewMatrix()), sizeof(glm::mat4));
		loadData(glm::value_ptr(light.getProjectionMatrix()), sizeof(glm::mat4));
	}
	else {
		static_assert(sizeof(T) == 0, "Unsupported light type");
	}
}
