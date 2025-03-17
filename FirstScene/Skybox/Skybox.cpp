#include "Skybox.h"

Skybox::Skybox(const char* path, unsigned int mipmapLevel, bool flipY)
{
	std::string pathStr = std::string(path);
	std::array<std::string, 6> paths = {
		pathStr + '/' + std::string("right.jpg"),
		pathStr + '/' + std::string("left.jpg"),
		pathStr + '/' + std::string("top.jpg"),
		pathStr + '/' + std::string("bottom.jpg"),
		pathStr + '/' + std::string("front.jpg"),
		pathStr + '/' + std::string("back.jpg"),
	};

	cubeMap = std::make_shared<CubeMap>(
		paths, mipmapLevel,
		GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, flipY
	);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
}

Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

Skybox::Skybox(Skybox&& other) noexcept
{
	cubeMap = std::move(other.cubeMap);
	VAO = other.VAO;
	VBO = other.VBO;
	other.VAO = 0;
	other.VBO = 0;
}

Skybox& Skybox::operator=(Skybox&& other) noexcept
{
	if (this != &other) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		cubeMap = std::move(other.cubeMap);
		VAO = other.VAO;
		VBO = other.VBO;
		other.VAO = 0;
		other.VBO = 0;
	}
	return *this;
}

void Skybox::draw(Shader& shader)
{
	shader.use();
	cubeMap->bind(0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
