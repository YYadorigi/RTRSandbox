#pragma once
#include "View/Camera.h"
#include "View/Light.h"

const unsigned int OPENGL_MAJOR_VERSION = 4;
const unsigned int OPENGL_MINOR_VERSION = 3;
const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 1200;
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;
const unsigned int POINT_LIGHT_BYTES = 2 * sizeof(glm::vec4);
const unsigned int SPOT_LIGHT_BYTES = 6 * sizeof(glm::vec4) + 2 * sizeof(glm::mat4);
const unsigned int MAX_LIGHTS_PER_TYPE = 4;

Camera camera = Camera(
	ViewCone(
		glm::vec3(0.0f, 0.0f, 5.0f),	// position
		glm::vec3(0.0f, 0.0f, -1.0f),	// look-at direction
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		Frustum(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f)
	),
	4.5f,	// speed
	0.05f	// sensitivity
);

std::vector<PointLight> pointLights = {
	PointLight(
		glm::vec3(5.0f, 5.0f, 5.0f),	// position
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		10.0f,							// intensity
		Frustum(90.0f, static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT, 0.1f, 100.0f)
	),
	PointLight(
		glm::vec3(-5.0f, -5.0f, -5.0f),	// position
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		10.0f,							// intensity
		Frustum(90.0f, static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT, 0.1f, 100.0f)
	),
	PointLight(
		glm::vec3(5.0f, 0.0f, -5.0f),	// position
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		10.0f,							// intensity
		Frustum(90.0f, static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT, 0.1f, 100.0f)
	),
	PointLight(
		glm::vec3(-5.0f, 0.0f, 5.0f),	// position
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		10.0f,							// intensity
		Frustum(90.0f, static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT, 0.1f, 100.0f)
	)
};

std::vector<SpotLight> spotLights = {
	SpotLight(
		ViewCone(
			glm::vec3(0.0f, 0.0f, 5.0f),	// position
			glm::vec3(0.0f, 0.0f, -1.0f),	// look-at direction
			glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
			Frustum(45.0f, static_cast<float>(SHADOW_WIDTH) / SHADOW_HEIGHT, 0.1f, 100.0f)
		),
		glm::vec3(1.0f, 1.0f, 1.0f),		// color
		10.0f,								// intensity
		glm::cos(glm::radians(12.5f)),		// cutoff
		glm::cos(glm::radians(17.5f))		// outer cutoff
	)
};

SpotLight& cameraLight = spotLights[0];
bool cameraLightActive = true;