#include "ScreenQuad.h"

ScreenQuad::ScreenQuad(float scale, glm::vec2 movement)
{
	float quadVertices[] = {
		// positions													// texCoords
		((-1.0f * scale) + movement.x), ((1.0f * scale) + movement.y),  0.0f, 1.0f,
		((-1.0f * scale) + movement.x), ((-1.0f * scale) + movement.y), 0.0f, 0.0f,
		((1.0f * scale) + movement.x),  ((-1.0f * scale) + movement.y), 1.0f, 0.0f,

		((-1.0f * scale) + movement.x), ((1.0f * scale) + movement.y),  0.0f, 1.0f,
		((1.0f * scale) + movement.x),  ((-1.0f * scale) + movement.y), 1.0f, 0.0f,
		((1.0f * scale) + movement.x),  ((1.0f * scale) + movement.y),  1.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

ScreenQuad::~ScreenQuad()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

ScreenQuad::ScreenQuad(ScreenQuad&& other) noexcept
{
	VAO = other.VAO;
	VBO = other.VBO;
	other.VAO = 0;
	other.VBO = 0;
}

ScreenQuad& ScreenQuad::operator=(ScreenQuad&& other) noexcept
{
	if (this != &other) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		VAO = other.VAO;
		VBO = other.VBO;
		other.VAO = 0;
		other.VBO = 0;
	}
	return *this;
}

void ScreenQuad::draw(Framebuffer& framebuffer, Shader& shader) const
{
	glActiveTexture(GL_TEXTURE0);
	framebuffer.bindTex();
	shader.use();
	shader.setInt("screenTexture", 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
