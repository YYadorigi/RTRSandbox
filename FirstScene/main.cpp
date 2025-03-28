#include <iostream>
#include <vector>
#include <ranges>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "macro.h"
#include "Skybox/Skybox.h"
#include "Framebuffer/Framebuffer.h"
#include "Framebuffer/ScreenQuad.h"
#include "Shader/Shader.h"
#include "Shader/UniformBuffer.h"

static float deltaTime = 0.0f;	// Time between current frame and last frame
static float lastFrame = 0.0f;	// Time of last frame
static float lastX = static_cast<float>(SCREEN_WIDTH / 2);	// Cursor X position
static float lastY = static_cast<float>(SCREEN_HEIGHT / 2);	// Cursor Y position

static glm::mat4 model{};
static std::vector<glm::vec3> translations{};

int initOpenGL(unsigned int majorVer, unsigned int minorVer, unsigned int profile);

GLFWwindow* createWindow(unsigned int width, unsigned int height, const char* title);

void processInput(GLFWwindow* window);

int main()
{
	// Initialize GLFW
	initOpenGL(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FirstScene");
	if (!window) {
		return -1;
	}

	// Create alpha blending buffers
	Framebuffer opaqueFBO(SCREEN_WIDTH, SCREEN_HEIGHT, true);
	opaqueFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);		// opaque color texture

	Framebuffer transparentFBO(SCREEN_WIDTH, SCREEN_HEIGHT, true);
	transparentFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// accumulation texture
	transparentFBO.attachColorTexture(GL_R32F, GL_RED, GL_FLOAT);			// revealge texture
	transparentFBO.attachDepthTexture(opaqueFBO.attachDepthTexture(DepthStencilType::DEPTH));

	Framebuffer intermediateFBO(SCREEN_WIDTH, SCREEN_HEIGHT);
	intermediateFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// opaque color texture
	intermediateFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// accumulation texture
	intermediateFBO.attachColorTexture(GL_R32F, GL_RED, GL_FLOAT);			// revealge texture

	// Create shadow map buffers
	std::vector<Framebuffer> pointShadowFBOs{};
	std::vector<Framebuffer> spotShadowFBOs{};

	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	for (const auto& pointLight : pointLights) {
		pointShadowFBOs.emplace_back(Framebuffer(SHADOW_WIDTH, SHADOW_HEIGHT));
		pointShadowFBOs.back().attachDepthTexture(DepthStencilType::DEPTH);
		if (++pointLightCount >= MAX_LIGHTS_PER_TYPE) break;
	}

	for (const auto& spotLight : spotLights) {
		spotShadowFBOs.emplace_back(Framebuffer(SHADOW_WIDTH, SHADOW_HEIGHT));
		spotShadowFBOs.back().attachDepthTexture(DepthStencilType::DEPTH);
		if (++spotLightCount >= MAX_LIGHTS_PER_TYPE) break;
	}

	// Create screen quad
	ScreenQuad screenQuad;

	// Load shader programs
	Shader shadowShader = Shader(
		"assets/shaders/Effects/ShadowMap/ShadowInstanced.vert",
		"assets/shaders/Effects/ShadowMap/Shadow.frag"
	);
	Shader skyboxShader = Shader(
		"assets/shaders/Effects/Skybox/Skybox.vert",
		"assets/shaders/Effects/Skybox/Skybox.frag"
	);
	Shader opaqueShader = Shader(
		"assets/shaders/Shading/BlinnPhong/BlinnPhongInstanced.vert",
		"assets/shaders/Shading/BlinnPhong/BlinnPhong.frag"
	);
	Shader transparentShader = Shader(
		"assets/shaders/Shading/BlinnPhong/BlinnPhongInstanced.vert",
		"assets/shaders/Shading/BlinnPhong/BlinnPhongTransparency.frag"
	);
	Shader blendShader = Shader(
		"assets/shaders/Blending/ScreenQuad.vert",
		"assets/shaders/Blending/WeightedBlended.frag"
	);
	Shader screenShader = Shader(
		"assets/shaders/PostProcessing/ScreenQuad.vert",
		"assets/shaders/PostProcessing/Default.frag"
	);

	// Create uniform block layouts
	UniformBuffer viewProjUBO = UniformBuffer(2 * sizeof(glm::mat4));
	UniformBuffer lightsUBO = UniformBuffer(MAX_LIGHTS_PER_TYPE * (POINT_LIGHT_BYTES + SPOT_LIGHT_BYTES));

	// Load skybox and models
	Skybox skybox("assets/environment/Skybox", 0, true);
	Model vase("assets/models/Vase/Vase.obj");
	Model translucentVase("assets/models/Vase0.3/Vase.obj");
	Model backpack("assets/models/Backpack/Backpack.obj", true);
	Model bathroomFloor("assets/models/BathroomFloor/bathroom_floor.obj");

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Show fps
		glfwSetWindowTitle(window, ("FirstScene: " + std::to_string(static_cast<int>(1.0f / deltaTime))).c_str());

		// Device input
		processInput(window);

		// Light uniforms
		lightsUBO.bind(1);
		lightsUBO.resetOffset();

		for (unsigned int idx{}; const auto & pointLight : pointLights) {
			lightsUBO.loadLightUniforms(pointLight);
			if (++idx >= pointLightCount) break;
		}

		lightsUBO.alignOffset(MAX_LIGHTS_PER_TYPE * POINT_LIGHT_BYTES);

		for (unsigned int idx{}; const auto & spotLight : spotLights) {
			lightsUBO.loadLightUniforms(spotLight, idx == 0 ? cameraLightActive : true);
			if (++idx >= spotLightCount) break;
		}

		// Shadow mapping render pass
		for (const auto& [spotShadowFBO, spotLight] : std::views::zip(spotShadowFBOs, spotLights)) {
			viewProjUBO.bind(0);
			viewProjUBO.resetOffset();
			viewProjUBO.loadViewProjUniforms(spotLight);

			spotShadowFBO.bind();

			// Pre-render settings
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// Render drawcalls
			shadowShader.use();
			shadowShader.setUniformBlock("VPMatrices", 0);

			model = glm::mat4(1.0f);
			translations = {
				glm::vec3(-5.0f, 0.0f, -1.0f),
				glm::vec3(0.0f, 0.0f, -1.0f),
				glm::vec3(5.0f, 0.0f, -1.0f),
			};
			spotShadowFBO.drawInstanced(backpack, shadowShader, model, translations);

			model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.01f));
			spotShadowFBO.draw(bathroomFloor, shadowShader, model);

			// Post-render settings
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
		}

		viewProjUBO.bind(0);
		viewProjUBO.resetOffset();
		viewProjUBO.loadViewProjUniforms(camera);

		// Opaque render pass
		opaqueFBO.bind();
		opaqueFBO.configureColorAttachments(std::vector<unsigned int>{0});

		// Pre-render settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)));
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Render drawcalls
		opaqueShader.use();
		opaqueShader.setUniformBlock("VPMatrices", 0);
		opaqueShader.setUniformBlock("Lights", 1);
		for (unsigned int idx{}; const auto & spotShadowFBO : spotShadowFBOs) {
			static int maxBinding;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxBinding);
			spotShadowFBO.bindTexture(-1, maxBinding - 1 - idx);
			opaqueShader.setUniform("spotShadowMaps[" + std::to_string(idx) + "]", maxBinding - 1 - idx);
		}	// TODO
		opaqueShader.setUniform("cameraPosition", camera.getPosition());

		model = glm::mat4(1.0f);
		translations = {
			glm::vec3(-5.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(5.0f, 0.0f, -1.0f),
		};
		opaqueFBO.drawInstanced(backpack, opaqueShader, model, translations);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f));
		opaqueFBO.draw(bathroomFloor, opaqueShader, model);

		// Skybox drawcall
		skyboxShader.use();
		skyboxShader.setUniformBlock("VPMatrices", 0);

		glDepthFunc(GL_LEQUAL);
		skybox.draw(skyboxShader);
		glDepthFunc(GL_LESS);

		// Post-render settings
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// Transparent render pass
		transparentFBO.bind();
		transparentFBO.configureColorAttachments(std::vector<unsigned int>{0, 1});

		// Pre-render settings
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunci(0, GL_ONE, GL_ONE);					// accumulation blend target
		glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);	// revealge blend target
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f)));
		glClearBufferfv(GL_COLOR, 1, glm::value_ptr(glm::vec4(1.0f)));

		// Render drawcalls
		transparentShader.use();
		transparentShader.setUniformBlock("VPMatrices", 0);
		transparentShader.setUniformBlock("Lights", 1);
		for (unsigned int idx{}; const auto & spotShadowFBO : spotShadowFBOs) {
			static int maxBinding;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxBinding);
			spotShadowFBO.bindTexture(-1, maxBinding - 1 - idx);
			transparentShader.setUniform("spotShadowMaps[" + std::to_string(idx) + "]", maxBinding - 1 - idx);
		}	// TODO
		transparentShader.setUniform("cameraPosition", camera.getPosition());

		model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		translations.clear();
		for (unsigned int xOffset = 0; xOffset < 5; xOffset++) {
			for (unsigned int yOffset = 0; yOffset < 5; yOffset++) {
				translations.emplace_back(
					-3.0f + 1.5f * xOffset,
					-3.0f + 1.5f * yOffset,
					1.0f
				);
			}
		}
		transparentFBO.drawInstanced(translucentVase, transparentShader, model, translations);

		// Post-render settings
		glDepthMask(GL_TRUE);
		glDisable(GL_DEPTH_TEST);
		glBlendFunci(0, GL_ONE, GL_ZERO);
		glBlendFunci(1, GL_ONE, GL_ZERO);
		glDisable(GL_BLEND);

		// Blending pass
		opaqueFBO.blitColorTexture(0, intermediateFBO, 0);
		transparentFBO.blitColorTexture(0, intermediateFBO, 1);
		transparentFBO.blitColorTexture(1, intermediateFBO, 2);

		intermediateFBO.bind();
		intermediateFBO.configureColorAttachments(std::vector<unsigned int>{0});

		// Pre-render settings
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Render drawcalls
		screenQuad.draw(blendShader, std::vector<ScreenQuadTexture>{
			{"accumTexture", intermediateFBO, 1 },
			{ "revealTexture", intermediateFBO, 2 }
		});

		// Post-render settings
		glBlendFunc(GL_ONE, GL_ZERO);
		glDisable(GL_BLEND);

		// Post-processing pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		screenQuad.draw(screenShader, std::vector<ScreenQuadTexture>{
			{"screenTexture", intermediateFBO, 0}
		});

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Destroy window
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();

	return 0;
}

static int initOpenGL(unsigned int majorVer, unsigned int minorVer, unsigned int profile)
{
	int init = glfwInit();
	if (!init) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Set OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVer);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVer);

	// Set OpenGL profile to core
	glfwWindowHint(GLFW_OPENGL_PROFILE, profile);

	// Enable multisampling
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Enable forward compatibility for macOS
#ifdef __MACOS__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	return 0;
}

static GLFWwindow* createWindow(unsigned int width, unsigned int height, const char* title)
{
	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL functions using GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	// Set viewport size
	glViewport(0, 0, width, height);

	// Set callback for window resize
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int newWidth, int newHeight) {
		glViewport(0, 0, newWidth, newHeight);
	});

	// Set input mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Hide cursor and capture it

	// Set callback for mouse movement
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		static bool firstMouse = true;
		if (firstMouse) {
			lastX = static_cast<float>(xpos);
			lastY = static_cast<float>(ypos);
			firstMouse = false;
		}
		float offsetX = static_cast<float>(xpos) - lastX;
		float offsetY = lastY - static_cast<float>(ypos);	// Reversed since y-coordinates range from bottom to top
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		camera.updateDirection(offsetX, offsetY);
		cameraLight.updateDirection(camera.getDirection());
	});

	// Set callback for mouse scroll
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		camera.updateFov(static_cast<float>(yoffset));
	});

	// Set callback for right mouse button
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			cameraLightActive = !cameraLightActive;
		}
	});

	return window;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	bool cameraMoved = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.updatePosition(camera.getDirection(), deltaTime);
		cameraMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.updatePosition(-camera.getDirection(), deltaTime);
		cameraMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.updatePosition(-camera.getRight(), deltaTime);
		cameraMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.updatePosition(camera.getRight(), deltaTime);
		cameraMoved = true;
	}
	if (cameraMoved) {
		cameraLight.updatePosition(camera.getPosition());
	}
}