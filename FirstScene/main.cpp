#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "macro.h"
#include "Shader/Shader.h"
#include "Shader/UniformBuffer.h"
#include "Viewer/Camera.h"
#include "Viewer/Light.h"
#include "Model/Model.h"
#include "Skybox/Skybox.h"
#include "Framebuffer/Renderbuffer.h"
#include "Framebuffer/Framebuffer.h"
#include "Framebuffer/ScreenQuad.h"

static float deltaTime = 0.0f;	// Time between current frame and last frame
static float lastFrame = 0.0f;	// Time of last frame
static float lastX = static_cast<float>(SCREEN_WIDTH / 2);	// Cursor X position
static float lastY = static_cast<float>(SCREEN_HEIGHT / 2);	// Cursor Y position

Frustum frustum(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);

static Camera camera = Camera{
	glm::vec3(0.0f, 0.0f, 5.0f),	// position
	glm::vec3(0.0f, 0.0f, -1.0f),	// look-at direction
	glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
	frustum,
	5.0f,	// speed
	0.05f	// sensitivity
};

Light pointLights[] = {
	PointLight(
		glm::vec3(5.0f, 5.0f, 5.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		3.0f	// intensity
	),
	PointLight(
		glm::vec3(-5.0f, 5.0f, 5.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		3.0f	// intensity
	),
	PointLight(
		glm::vec3(0.0f, -5.0f, -5.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		3.0f	// intensity
	)
};

static SpotLight cameraLight = SpotLight{
	glm::vec3(0.0f, 0.0f, 5.0f),	// position
	glm::vec3(0.0f),				// target
	glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
	frustum,
	glm::vec3(1.0f, 1.0f, 1.0f),	// color
	1.0f,	// intensity
	glm::cos(glm::radians(12.5f)),	// cutoff
	glm::cos(glm::radians(17.5f))	// outer cutoff
}; static bool cameraLightActive = true;

AmbientLight ambient = {
	glm::vec3(1.0f),	// color
	0.1f,	// intensity
};

void processInput(GLFWwindow* window);

static int initOpenGL(unsigned int majorVer, unsigned int minorVer, unsigned int profile);

static GLFWwindow* createWindow(unsigned int width, unsigned int height, const char* title);

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform);

unsigned int uniformOffset(unsigned int increase, bool reset = false);

int main()
{
	// Initialize GLFW
	initOpenGL(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FirstScene");
	if (!window) {
		return -1;
	}

	// Create framebuffers
	std::shared_ptr<Renderbuffer> depthRBO = std::make_shared<Renderbuffer>(
		SCREEN_WIDTH, SCREEN_HEIGHT, RBOType::DEPTH, true
	);

	Framebuffer opaqueFBO(SCREEN_WIDTH, SCREEN_HEIGHT, true);
	opaqueFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);		// opaque color texture
	opaqueFBO.attachRenderbuffer(depthRBO);									// opaque depth texture

	Framebuffer transparentFBO(SCREEN_WIDTH, SCREEN_HEIGHT, true);
	transparentFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// accumulation texture
	transparentFBO.attachColorTexture(GL_R8, GL_RED, GL_FLOAT);				// revealge texture
	transparentFBO.attachRenderbuffer(depthRBO);							// opaque depth texture

	Framebuffer intermediateFBO(SCREEN_WIDTH, SCREEN_HEIGHT);
	intermediateFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// opaque color texture
	intermediateFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// accumulation texture
	intermediateFBO.attachColorTexture(GL_R8, GL_RED, GL_FLOAT);			// revealge texture

	// Create screen quad
	ScreenQuad screenQuad;

	// Load skybox
	Skybox skybox("assets/skybox", 0);

	// Load models
	Model backpack("assets/objects/backpack/backpack.obj", true);
	Model vase("assets/objects/vase/vase.obj");
	Model vase20("assets/objects/vase_0.2/vase.obj");
	Model vase35("assets/objects/vase_0.35/vase.obj");
	Model vase50("assets/objects/vase_0.5/vase.obj");

	// Load shader programs
	Shader skyboxShader = Shader(
		"assets/shaders/vertex/skybox.vert",
		"assets/shaders/fragment/skybox.frag"
	);
	Shader opaqueShader = Shader(
		"assets/shaders/vertex/main.vert",
		"assets/shaders/fragment/blinn_phong.frag"
	);
	Shader transparentShader = Shader(
		"assets/shaders/vertex/main.vert",
		"assets/shaders/fragment/blinn_phong_transparent.frag"
	);
	Shader blendShader = Shader(
		"assets/shaders/vertex/screen_quad.vert",
		"assets/shaders/blending/weighted_blended.frag"
	);
	Shader screenShader = Shader(
		"assets/shaders/vertex/screen_quad.vert",
		"assets/shaders/post_processing/main.frag"
	);

	// Create uniform block layouts
	UniformBuffer viewProjUBO = UniformBuffer(2 * sizeof(glm::mat4));
	UniformBuffer lightsUBO = UniformBuffer(21 * sizeof(glm::vec4));

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

		// Update uniform variables
		viewProjUBO.bind(0);
		uniformOffset(0, true);
		viewProjUBO.setData(glm::value_ptr(camera.getViewMatrix()), sizeof(glm::mat4), uniformOffset(sizeof(glm::mat4)));
		viewProjUBO.setData(glm::value_ptr(camera.getProjectionMatrix()), sizeof(glm::mat4), uniformOffset(sizeof(glm::mat4)));

		lightsUBO.bind(1);
		uniformOffset(2 * sizeof(glm::vec4), true);
		for (unsigned int idx{}; const auto & pointLight : pointLights) {
			lightsUBO.setData(glm::value_ptr(pointLight.getPosition()), sizeof(glm::vec4), uniformOffset(sizeof(glm::vec4)));
			lightsUBO.setData(glm::value_ptr(pointLight.getColor()), sizeof(glm::vec3), uniformOffset(sizeof(glm::vec3)));
			lightsUBO.setData(&static_cast<const float&>(pointLight.getIntensity()), sizeof(float), uniformOffset(sizeof(float)));
			if (++idx >= MAX_POINT_LIGHTS) break;
		}
		uniformOffset((1 + MAX_POINT_LIGHTS) * 2 * sizeof(glm::vec4), true);
		lightsUBO.setData(glm::value_ptr(cameraLight.getPosition()), sizeof(glm::vec4), uniformOffset(sizeof(glm::vec4)));
		lightsUBO.setData(glm::value_ptr(cameraLight.getDirection()), sizeof(glm::vec4), uniformOffset(sizeof(glm::vec4)));
		lightsUBO.setData(glm::value_ptr(cameraLight.getColor()), sizeof(glm::vec3), uniformOffset(sizeof(glm::vec3)));
		lightsUBO.setData(&static_cast<const float&>(cameraLight.getIntensity() * cameraLightActive), sizeof(float), uniformOffset(sizeof(float)));
		lightsUBO.setData(&static_cast<const float&>(cameraLight.getCutoff()), sizeof(float), uniformOffset(sizeof(float)));
		lightsUBO.setData(&static_cast<const float&>(cameraLight.getOuterCutoff()), sizeof(float), uniformOffset(sizeof(glm::vec4) - sizeof(float)));
		lightsUBO.setData(glm::value_ptr(ambient.color), sizeof(glm::vec3), uniformOffset(sizeof(glm::vec3)));
		lightsUBO.setData(&static_cast<const float&>(ambient.intensity), sizeof(float), uniformOffset(sizeof(float)));

		// Opauqe render pass
		opaqueFBO.bind();
		opaqueFBO.configureColorAttachments(std::vector<unsigned int>{0});

		// Pre-render settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)));
		glClearBufferfv(GL_DEPTH, 0, glm::value_ptr(glm::vec4(1.0f)));

		// Render drawcalls
		opaqueShader.use();

		opaqueShader.setUniformBlock("VPMatrices", 0);
		opaqueShader.setUniformBlock("Lights", 1);
		opaqueShader.setVec3("viewPos", glm::value_ptr(camera.getPosition()));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 0.0f, -1.0f));
		sceneDraw(backpack, opaqueShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, -1.0f));
		sceneDraw(backpack, opaqueShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase, opaqueShader, model);

		// Skybox drawcall
		glDepthFunc(GL_LEQUAL);

		skyboxShader.use();
		skyboxShader.setUniformBlock("VPMatrices", 0);

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
		transparentShader.setVec3("viewPos", glm::value_ptr(camera.getPosition()));

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase20, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase20, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase20, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase35, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase35, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase35, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -2.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase50, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase50, transparentShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase50, transparentShader, model);

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

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.updatePosition(camera.getDirection(), deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.updatePosition(-camera.getDirection(), deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.updatePosition(-camera.getRight(), deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.updatePosition(camera.getRight(), deltaTime);
	cameraLight.updatePosition(camera.getPosition());
}

static int initOpenGL(unsigned int majorVer, unsigned int minorVer, unsigned int profile)
{
	int init = glfwInit();
	if (!init) {
		std::cout << "Failed to initialize GLFW" << std::endl;
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
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	// Load OpenGL functions using GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
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

void sceneDraw(Model& model, Shader& shader, glm::mat4 transform)
{
	shader.use();
	shader.setTransform("model", glm::value_ptr(transform));
	shader.setTransform("invModel", glm::value_ptr(glm::inverse(transform)));
	model.draw(shader);
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