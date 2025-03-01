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
	5.0f
);

static float deltaTime = 0.0f;	// Time between current frame and last frame
static float lastFrame = 0.0f;	// Time of last frame

float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	// left-bottom-behind, 0
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	// left-bottom-front, 1
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	// left-top-behind, 2
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,	// left-top-front, 3

	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,	// right-bottom-behind, 4
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	// right-bottom-front, 5
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	// right-top-behind, 6
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	// right-top-front, 7
};

unsigned int indices[] = {
	1, 3, 5,	// front
	7, 3, 5,	// front
	0, 2, 4,	// back
	6, 2, 4,	// back
	1, 0, 3,	// left
	2, 0, 3,	// left
	5, 4, 7,	// right
	6, 4, 7,	// right
	1, 0, 5,	// bottom
	4, 0, 5,	// bottom
	3, 2, 7,	// top
	6, 2, 7,	// top
};

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
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);	// vertex coords
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));	// texture coords
	glEnableVertexAttribArray(1);

	// Load texture
	Texture2D containerTex = Texture2D("src/textures/container.jpg", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR,
		0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	Texture2D faceTex = Texture2D("src/textures/awesomeface.png", GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR,
		0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	glActiveTexture(GL_TEXTURE0);
	containerTex.bind();
	glActiveTexture(GL_TEXTURE1);
	faceTex.bind();

	// Load shader program
	Shader shader = Shader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
	shader.use();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

	// Set render mode
	glEnable(GL_DEPTH_TEST);

	// Render loop
	srand(static_cast<unsigned int>(time(0)));
	std::vector<double> randNum;
	for (auto pos : cubePositions) {
		randNum.emplace_back(static_cast<double>(rand() % (sizeof(cubePositions) / sizeof(cubePositions[0]))));
	}

	while (!glfwWindowShouldClose(window)) {
		// Time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// printf("FPS: %f\n", 1.0f / deltaTime);

		// Handle device input
		processInput(window);

		// Clear screen
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	// Background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Designate VAO
		glBindVertexArray(VAO);

		// Projection matrix
		static glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(SCREEN_WIDTH / SCREEN_HEIGHT),
			0.1f,
			100.0f
		);

		// View matrix
		glm::mat4 view = camera.getViewMatrix();

		// Model matrices and Drawcalls
		auto it = randNum.begin();
		for (auto pos : cubePositions) {
			glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
			model = glm::rotate(
				model,
				glm::radians(static_cast<float>((glfwGetTime() + *it) * 180.0 / M_PI)),
				glm::vec3(1.0f, 1.0f, 0.0f)
			);
			shader.setTransform("transform", glm::value_ptr(projection * view * model));
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			++it;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up data
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

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
		camera.updatePosition(-glm::cross(camera.getDirection(), camera.getUp()), deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.updatePosition(glm::cross(camera.getDirection(), camera.getUp()), deltaTime);
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

	return window;
}