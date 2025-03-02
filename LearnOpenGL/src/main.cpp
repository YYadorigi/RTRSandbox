#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "macro.h"
#include "Shader.h"
#include "Texture.h"
#include "Viewers/Camera.h"
#include "Viewers/Light.h"

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
		glm::vec3(0.7f, 0.2f, 2.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		1.0f	// intensity
	),
	Light(
		glm::vec3(2.3f, -3.3f, -4.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		1.0f	// intensity
	),
	Light(
		glm::vec3(-4.0f, 2.0f, -12.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
		1.0f	// intensity
	),
	Light(
		glm::vec3(0.0f, 0.0f, -3.0f),	// position
		glm::vec3(0.0f),				// target
		glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
		frustum,
		glm::vec3(1.0f, 1.0f, 1.0f),	// color
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
	0.5f,	// intensity
};

SpotLight spotLight(
	glm::vec3(0.0f, 0.0f, 5.0f),	// position
	glm::vec3(0.0f),				// target
	glm::vec3(0.0f, 1.0f, 0.0f),	// up direction
	frustum,
	glm::vec3(1.0f, 1.0f, 1.0f),	// color
	2.0f,	// intensity
	glm::cos(glm::radians(12.5f)),	// cutoff
	glm::cos(glm::radians(17.5f))	// outer cutoff
);

float cubeMesh[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};
const unsigned int NUM_TRIANGLES = 36;

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f),
};

void processInput(GLFWwindow* window);

static int initOpenGL(unsigned int majorVer, unsigned int minorVer, unsigned int profile);

static GLFWwindow* createWindow(unsigned int width, unsigned int height, const char* title);

int main()
{
	// Initialize GLFW
	initOpenGL(3, 3, GLFW_OPENGL_CORE_PROFILE);	// Use OpenGL 3.3 core profile

	// Create window
	GLFWwindow* window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL");
	if (!window) {
		return -1;
	}

	// Load vertex data
	unsigned int VAO, lightVAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeMesh), cubeMesh, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	// vertex coords
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	// normal coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	// texture coords
	glEnableVertexAttribArray(2);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeMesh), cubeMesh, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertex coords
	glEnableVertexAttribArray(0);

	// Load texture
	Texture2D cubeAmbient = Texture2D("src/Textures/container.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR, 0, GL_UNSIGNED_BYTE);
	Texture2D cubeDiffuse = Texture2D("src/Textures/container.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR, 0, GL_UNSIGNED_BYTE);
	Texture2D cubeSpec = Texture2D("src/Textures/container_spec.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR, 0, GL_UNSIGNED_BYTE);

	glActiveTexture(GL_TEXTURE0);
	cubeAmbient.bind();
	glActiveTexture(GL_TEXTURE1);
	cubeDiffuse.bind();
	glActiveTexture(GL_TEXTURE2);
	cubeSpec.bind();

	// Load shader program
	Shader cubeShader = Shader("src/Shaders/blinn_phong_vs.glsl", "src/Shaders/blinn_phong_fs.glsl");
	Shader lightShader = Shader("src/Shaders/light_vs.glsl", "src/Shaders/light_fs.glsl");

	cubeShader.use();
	cubeShader.setInt("material.ambient", 0);
	cubeShader.setInt("material.diffuse", 1);
	cubeShader.setInt("material.specular", 2);
	cubeShader.setFloat("material.shininess", 64.0f);
	cubeShader.setVec3("dirLight.direction", glm::value_ptr(directional.direction));
	cubeShader.setVec3("dirLight.color", glm::value_ptr(directional.color));
	cubeShader.setFloat("dirLight.intensity", directional.intensity);
	for (size_t idx{}; const auto & pointLight : pointLights) {
		std::string lightName = "pointLights[" + std::to_string(idx++) + "]";
		cubeShader.setVec3(lightName + ".position", glm::value_ptr(pointLight.getPosition()));
		cubeShader.setVec3(lightName + ".color", glm::value_ptr(pointLight.getColor()));
		cubeShader.setFloat(lightName + ".intensity", pointLight.getIntensity());
	}
	cubeShader.setVec3("spotLight.color", glm::value_ptr(spotLight.getColor()));
	cubeShader.setFloat("spotLight.intensity", spotLight.getIntensity());
	cubeShader.setFloat("spotLight.cutoff", spotLight.getCutoff());
	cubeShader.setFloat("spotLight.outerCutoff", spotLight.getOuterCutoff());
	cubeShader.setVec3("ambientLight.color", glm::value_ptr(ambient.color));
	cubeShader.setFloat("ambientLight.intensity", ambient.intensity);

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

		spotLight.updatePosition(camera.getPosition());
		spotLight.updateDirection(camera.getDirection());

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	// Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Drawcall logic
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();

		// Draw the point lights
		lightShader.use();
		lightShader.setTransform("view", glm::value_ptr(view));
		lightShader.setTransform("projection", glm::value_ptr(projection));

		glBindVertexArray(lightVAO);
		for (const auto& pointLight : pointLights) {
			glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), pointLight.getPosition());
			lightModel = glm::scale(lightModel, glm::vec3(0.2f));
			lightShader.setTransform("model", glm::value_ptr(lightModel));
			glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLES);
		};

		// Draw the cubes
		cubeShader.use();
		cubeShader.setTransform("view", glm::value_ptr(view));
		cubeShader.setTransform("projection", glm::value_ptr(projection));
		cubeShader.setVec3("viewPos", glm::value_ptr(camera.getPosition()));
		cubeShader.setVec3("spotLight.position", glm::value_ptr(spotLight.getPosition()));
		cubeShader.setVec3("spotLight.direction", glm::value_ptr(spotLight.getDirection()));

		glBindVertexArray(VAO);
		for (size_t idx{}; const auto & pos : cubePositions) {
			glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), pos);
			cubeModel = glm::rotate(cubeModel, static_cast<float>(glfwGetTime() + 5.0 * idx++), glm::vec3(1.0f, 0.3f, 0.5f));
			cubeShader.setTransform("model", glm::value_ptr(cubeModel));
			cubeShader.setTransform("invModel", glm::value_ptr(glm::inverse(cubeModel)));
			glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLES);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up data
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

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
	});

	// Set callback for mouse scroll
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		camera.updateFov(static_cast<float>(yoffset));
	});

	return window;
}