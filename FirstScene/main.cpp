#include <iostream>
#include <vector>
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

static float deltaTime = 0.0f;	// Time between current frame and last frame
static float lastFrame = 0.0f;	// Time of last frame
static float lastX = static_cast<float>(SCREEN_WIDTH / 2);	// Cursor X position
static float lastY = static_cast<float>(SCREEN_HEIGHT / 2);	// Cursor Y position

Frustum frustum(45.0f, static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);

Camera camera(
	glm::vec3(0.0f, 0.0f, 5.0f),	// position
	glm::vec3(0.0f, 0.0f, -1.0f),	// look-at direction
	glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
	frustum,
	5.0f,	// speed
	0.05f	// sensitivity
);

Light pointLights[] = {
	Light(
		glm::vec3(1.0f, 1.0f, 1.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 0.5f, 1.0f),	// color
		1.0f	// intensity
	),
	Light(
		glm::vec3(-1.0f, -1.0f, -2.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 0.5f),	// color
		1.0f	// intensity
	),
	Light(
		glm::vec3(-1.0f, 1.0f, 1.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(0.5f, 1.0f, 1.0f),	// color
		1.0f	// intensity
	)
};

AmbientLight ambient = {
	glm::vec3(1.0f),	// color
	0.1f,	// intensity
};

DirectionalLight directional = {
	glm::vec3(-0.2f, -1.0f, -0.3f),	// direction
	glm::vec3(1.0f),	// color
	0.25f,	// intensity
};

SpotLight spotLight(
	glm::vec3(0.0f, 0.0f, 5.0f),	// position
	glm::vec3(0.0f),				// target
	glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
	frustum,
	glm::vec3(1.0f, 1.0f, 1.0f),	// color
	1.0f,	// intensity
	glm::cos(glm::radians(12.5f)),	// cutoff
	glm::cos(glm::radians(17.5f))	// outer cutoff
);
bool spotLightActive = true;

void processInput(GLFWwindow* window);

static int initOpenGL(unsigned int majorVer, unsigned int minorVer, unsigned int profile);

static GLFWwindow* createWindow(unsigned int width, unsigned int height, const char* title);

int main()
{
	// Initialize GLFW
	initOpenGL(3, 3, GLFW_OPENGL_CORE_PROFILE);	// Use OpenGL 3.3 core profile

	// Create window
	GLFWwindow* window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FirstScene");
	if (!window) {
		return -1;
	}

	// Load model
	Model backpack(static_cast<std::string>("assets/objects/backpack/backpack.obj"));

	// Load shader program
	Shader shader = Shader("assets/shaders/blinn_phong.vert", "assets/shaders/blinn_phong.frag");

	shader.use();
	shader.setVec3("dirLight.direction", glm::value_ptr(directional.direction));	// directional light
	shader.setVec3("dirLight.color", glm::value_ptr(directional.color));
	shader.setFloat("dirLight.intensity", directional.intensity);

	for (size_t idx{}; const auto & pointLight : pointLights) {						// point lights
		std::string lightName = "pointLights[" + std::to_string(idx++) + "]";
		shader.setVec3(lightName + ".position", glm::value_ptr(pointLight.getPosition()));
		shader.setVec3(lightName + ".color", glm::value_ptr(pointLight.getColor()));
		shader.setFloat(lightName + ".intensity", pointLight.getIntensity());
	}

	shader.setVec3("spotLight.color", glm::value_ptr(spotLight.getColor()));		// spot light
	shader.setFloat("spotLight.cutoff", spotLight.getCutoff());
	shader.setFloat("spotLight.outerCutoff", spotLight.getOuterCutoff());

	shader.setVec3("ambientLight.color", glm::value_ptr(ambient.color));			// ambient light
	shader.setFloat("ambientLight.intensity", ambient.intensity);

	// Set render mode
	glEnable(GL_DEPTH_TEST);

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Handle device input
		processInput(window);

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	// Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Drawcall logic
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();

		glm::mat4 backpackModel = glm::mat4(1.0f);
		backpackModel = glm::translate(backpackModel, glm::vec3(0.0f, 0.0f, 0.0f));
		backpackModel = glm::scale(backpackModel, glm::vec3(1.0f));

		shader.setTransform("model", glm::value_ptr(backpackModel));
		shader.setTransform("invModel", glm::value_ptr(glm::inverse(backpackModel)));
		shader.setTransform("view", glm::value_ptr(view));
		shader.setTransform("projection", glm::value_ptr(projection));
		shader.setVec3("viewPos", glm::value_ptr(camera.getPosition()));
		shader.setVec3("spotLight.position", glm::value_ptr(spotLight.getPosition()));
		shader.setVec3("spotLight.direction", glm::value_ptr(spotLight.getDirection()));
		shader.setFloat("spotLight.intensity", spotLight.getIntensity() * float(spotLightActive));

		backpack.Draw(shader);

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
	spotLight.updatePosition(camera.getPosition());
}

static int initOpenGL(unsigned int majorVer, unsigned int minorVer, unsigned int profile)
{
	int init = glfwInit();
	if (!init) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Set OpenGL version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVer);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVer);

	// Set OpenGL profile to core
	glfwWindowHint(GLFW_OPENGL_PROFILE, profile);

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
		camera.updatePitchYaw(offsetX, offsetY);
		camera.updateDirection();
		spotLight.updateDirection(camera.getDirection());
	});

	// Set callback for mouse scroll
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		camera.updateFov(static_cast<float>(yoffset));
	});

	// Set callback for right mouse button
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			spotLightActive = !spotLightActive;
		}
	});

	return window;
}