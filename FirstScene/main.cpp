#include <iostream>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "macro.h"
#include "Shader/Shader.h"
#include "Viewer/Camera.h"
#include "Viewer/Light.h"
#include "Model/Model.h"
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

DirectionalLight directional = {
	glm::vec3(-0.2f, -1.0f, -0.3f),	// direction
	glm::vec3(1.0f),	// color
	0.25f,	// intensity
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

int main()
{
	// Initialize GLFW
	initOpenGL(OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, GLFW_OPENGL_CORE_PROFILE);	// Use OpenGL 4.0 core profile

	// Create window
	GLFWwindow* window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FirstScene");
	if (!window) {
		return -1;
	}

	// Create framebuffers
	std::shared_ptr<Renderbuffer> depthRBO = std::make_shared<Renderbuffer>(
		Renderbuffer(SCREEN_WIDTH, SCREEN_HEIGHT, RBOType::DEPTH)
	);

	std::shared_ptr<Framebuffer> intermediateFBO = std::make_shared<Framebuffer>(
		Framebuffer(SCREEN_WIDTH, SCREEN_HEIGHT, nullptr)
	);

	Framebuffer opaqueFBO(SCREEN_WIDTH, SCREEN_HEIGHT, intermediateFBO);
	opaqueFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// opaque color texture
	opaqueFBO.attachRenderbuffer(depthRBO);								// opaque depth texture
	opaqueFBO.configureColorAttachments();

	Framebuffer transparentFBO(SCREEN_WIDTH, SCREEN_HEIGHT, intermediateFBO);
	transparentFBO.attachColorTexture(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);	// accumulation texture
	transparentFBO.attachColorTexture(GL_R8, GL_RED, GL_FLOAT);				// revealge texture
	transparentFBO.attachRenderbuffer(depthRBO);							// opaque depth texture
	transparentFBO.configureColorAttachments();

	// Create screen quad
	ScreenQuad screenQuad;

	// Load models
	Model backpack("assets/objects/backpack/backpack.obj");
	Model vase("assets/objects/vase/vase.obj");
	Model vase20("assets/objects/vase_0.2/vase.obj");
	Model vase35("assets/objects/vase_0.35/vase.obj");
	Model vase50("assets/objects/vase_0.5/vase.obj");

	// Load shader programs
	Shader opaqueShader = Shader(
		"assets/shaders/vertex/main.vert",
		"assets/shaders/fragment/blinn_phong.frag"
	);
	Shader transparentShader = Shader(
		"assets/shaders/vertex/main.vert",
		"assets/shaders/fragment/blinn_phong_transparent.frag"
	);
	Shader blendShader = Shader(
		"assets/shaders/vertex/screen.vert",
		"assets/shaders/post_processing/alpha_blending.frag"
	);
	Shader screenShader = Shader(
		"assets/shaders/vertex/screen.vert",
		"assets/shaders/post_processing/main.frag"
	);

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

		// Opauqe render pass
		opaqueFBO.bind();

		// Pre-render settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render drawcalls
		opaqueShader.use();

		opaqueShader.setVec3("dirLight.direction", glm::value_ptr(directional.direction));
		opaqueShader.setVec3("dirLight.color", glm::value_ptr(directional.color));
		opaqueShader.setFloat("dirLight.intensity", directional.intensity);

		for (size_t idx{}; const auto & pointLight : pointLights) {
			std::string lightName = "pointLights[" + std::to_string(idx++) + "]";
			opaqueShader.setVec3(lightName + ".position", glm::value_ptr(pointLight.getPosition()));
			opaqueShader.setVec3(lightName + ".color", glm::value_ptr(pointLight.getColor()));
			opaqueShader.setFloat(lightName + ".intensity", pointLight.getIntensity());
		}

		opaqueShader.setVec3("spotLight.position", glm::value_ptr(cameraLight.getPosition()));
		opaqueShader.setVec3("spotLight.direction", glm::value_ptr(cameraLight.getDirection()));
		opaqueShader.setVec3("spotLight.color", glm::value_ptr(cameraLight.getColor()));
		opaqueShader.setFloat("spotLight.intensity", cameraLight.getIntensity() * float(cameraLightActive));
		opaqueShader.setFloat("spotLight.cutoff", cameraLight.getCutoff());
		opaqueShader.setFloat("spotLight.outerCutoff", cameraLight.getOuterCutoff());

		opaqueShader.setVec3("ambientLight.color", glm::value_ptr(ambient.color));
		opaqueShader.setFloat("ambientLight.intensity", ambient.intensity);

		opaqueShader.setTransform("view", glm::value_ptr(camera.getViewMatrix()));
		opaqueShader.setTransform("projection", glm::value_ptr(camera.getProjectionMatrix()));
		opaqueShader.setVec3("viewPos", glm::value_ptr(camera.getPosition()));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 0.0f, -1.0f));
		sceneDraw(backpack, opaqueShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, -1.0f));
		sceneDraw(backpack, opaqueShader, model);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		sceneDraw(vase, opaqueShader, model);

		// Post-render settings
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// Transparent render pass
		transparentFBO.bind();

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

		transparentShader.setVec3("dirLight.direction", glm::value_ptr(directional.direction));
		transparentShader.setVec3("dirLight.color", glm::value_ptr(directional.color));
		transparentShader.setFloat("dirLight.intensity", directional.intensity);

		for (size_t idx{}; const auto & pointLight : pointLights) {
			std::string lightName = "pointLights[" + std::to_string(idx++) + "]";
			transparentShader.setVec3(lightName + ".position", glm::value_ptr(pointLight.getPosition()));
			transparentShader.setVec3(lightName + ".color", glm::value_ptr(pointLight.getColor()));
			transparentShader.setFloat(lightName + ".intensity", pointLight.getIntensity());
		}

		transparentShader.setVec3("spotLight.position", glm::value_ptr(cameraLight.getPosition()));
		transparentShader.setVec3("spotLight.direction", glm::value_ptr(cameraLight.getDirection()));
		transparentShader.setVec3("spotLight.color", glm::value_ptr(cameraLight.getColor()));
		transparentShader.setFloat("spotLight.intensity", cameraLight.getIntensity() * float(cameraLightActive));
		transparentShader.setFloat("spotLight.cutoff", cameraLight.getCutoff());
		transparentShader.setFloat("spotLight.outerCutoff", cameraLight.getOuterCutoff());

		transparentShader.setVec3("ambientLight.color", glm::value_ptr(ambient.color));
		transparentShader.setFloat("ambientLight.intensity", ambient.intensity);

		transparentShader.setTransform("view", glm::value_ptr(camera.getViewMatrix()));
		transparentShader.setTransform("projection", glm::value_ptr(camera.getProjectionMatrix()));
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

		// Post-processing blending pass
		opaqueFBO.bind();

		// Pre-render settings
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Render drawcalls
		transparentFBO.resolve();
		screenQuad.draw(blendShader, std::vector<ScreenQuadTexture>{
			{"accumTexture", transparentFBO, 0 },
			{ "revealTexture", transparentFBO, 1 }
		});

		// Post-render settings
		glDepthFunc(GL_LESS);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ZERO);
		glDisable(GL_BLEND);

		// Draw on screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		opaqueFBO.resolve();
		screenQuad.draw(screenShader, std::vector<ScreenQuadTexture>{
			{"screenTexture", opaqueFBO, 0}
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