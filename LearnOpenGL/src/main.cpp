#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <direct.h>
#include <assert.h>
#include <shader.h>
#include <texture.h>
#include <camera.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 800;
static Camera camera(
	glm::vec3(0.0f, 0.0f, 5.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	45.0f,
	5.0f,
	0.05f
);

static float deltaTime = 0.0f;	// Time between current frame and last frame
static float lastFrame = 0.0f;	// Time of last frame
static float lastX = static_cast<float>(SCREEN_WIDTH / 2);	// Cursor X position
static float lastY = static_cast<float>(SCREEN_HEIGHT / 2);	// Cursor Y position

float cubeMesh[] = {
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
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

glm::vec3 cubeColors[] = {
	glm::vec3(1.0f, 0.5f, 0.1f),
	glm::vec3(1.0f, 0.5f, 0.2f),
	glm::vec3(1.0f, 0.5f, 0.3f),
	glm::vec3(1.0f, 0.5f, 0.4f),
	glm::vec3(1.0f, 0.5f, 0.5f),
	glm::vec3(1.0f, 0.5f, 0.6f),
	glm::vec3(1.0f, 0.5f, 0.7f),
	glm::vec3(1.0f, 0.5f, 0.8f),
	glm::vec3(1.0f, 0.5f, 0.9f),
	glm::vec3(1.0f, 0.5f, 1.0f),
};

glm::vec3 lightPosition(0.0f, 0.0f, 3.0f);
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightScale = glm::vec3(0.1f);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);	// vertex coords
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));	// normal coords
	glEnableVertexAttribArray(1);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeMesh), cubeMesh, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // vertex coords
	glEnableVertexAttribArray(0);

	// Random number generator
	srand(static_cast<unsigned int>(time(0)));
	std::vector<double> randNum;
	for (auto pos : cubePositions) {
		randNum.emplace_back(static_cast<double>(rand() % (sizeof(cubePositions) / sizeof(cubePositions[0]))));
	}

	// Load texture
	// Texture2D containerTex = Texture2D("src/textures/container.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR,
	// 0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	// Texture2D faceTex = Texture2D("src/textures/awesomeface.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR,
	// 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	// glActiveTexture(GL_TEXTURE0);
	// containerTex.bind();
	// glActiveTexture(GL_TEXTURE1);
	// faceTex.bind();

	// Load shader program
	Shader cubeShader = Shader("src/shaders/blinn_phong_vs.glsl", "src/shaders/blinn_phong_fs.glsl");
	Shader lightShader = Shader("src/shaders/light_vs.glsl", "src/shaders/light_fs.glsl");

	// Set render mode
	glEnable(GL_DEPTH_TEST);

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		lightPosition.x = 3.0f * sin(static_cast<float>(glfwGetTime()));
		lightPosition.z = 3.0f * cos(static_cast<float>(glfwGetTime()));

		// Handle device input
		processInput(window);

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	// Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw
		glm::mat4 projection = glm::perspective(
			glm::radians(camera.getFov()),
			static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT),
			0.1f,
			100.0f
		);

		glm::mat4 view = camera.getViewMatrix();

		// Draw the light source
		glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPosition);
		lightModel = glm::scale(lightModel, lightScale);

		lightShader.use();
		lightShader.setTransform("model", glm::value_ptr(lightModel));
		lightShader.setTransform("view", glm::value_ptr(view));
		lightShader.setTransform("projection", glm::value_ptr(projection));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLES);

		// Draw the cubes
		cubeShader.use();
		cubeShader.setVec3("ambientColor", glm::value_ptr(glm::vec3(1.0f)));
		cubeShader.setFloat("ambientIntensity", 0.05f);
		cubeShader.setVec3("lightColor", glm::value_ptr(lightColor));
		cubeShader.setFloat("lightIntensity", 1.0f);
		cubeShader.setVec3("lightPos", glm::value_ptr(lightPosition));
		cubeShader.setVec3("viewPos", glm::value_ptr(camera.getPosition()));
		cubeShader.setTransform("view", glm::value_ptr(view));
		cubeShader.setTransform("projection", glm::value_ptr(projection));

		glBindVertexArray(VAO);
		auto it = randNum.begin();
		for (auto pos : cubePositions) {
			glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), pos);
			cubeModel = glm::rotate(
				cubeModel,
				glm::radians(static_cast<float>((glfwGetTime() + *it) * 180.0 / M_PI)),
				glm::vec3(1.0f, 1.0f, 0.0f)
			);
			cubeShader.setVec3("surfaceColor", glm::value_ptr(cubeColors[static_cast<int>(*it)]));
			cubeShader.setTransform("model", glm::value_ptr(cubeModel));
			glDrawArrays(GL_TRIANGLES, 0, NUM_TRIANGLES);
			++it;
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