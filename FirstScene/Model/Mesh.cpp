#include <iostream>
#include <glad/glad.h>
#include "Mesh.h"

Mesh::Mesh(
	std::vector<Vertex> vertices,
	std::vector<unsigned int> indices,
	std::vector<std::shared_ptr<TextureMap2D>> textures,
	float shininess,
	float opacity
) : vertices(vertices), indices(indices), textures(textures), shininess(shininess), opacity(opacity)
{
	setupMesh();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

Mesh::Mesh(Mesh&& other) noexcept
{
	vertices = std::move(other.vertices);
	indices = std::move(other.indices);
	textures = std::move(other.textures);
	shininess = other.shininess;
	opacity = other.opacity;
	VAO = other.VAO;
	VBO = other.VBO;
	EBO = other.EBO;
	other.shininess = 0.0f;
	other.opacity = 0.0f;
	other.VAO = 0;
	other.VBO = 0;
	other.EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);

		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
		textures = std::move(other.textures);
		shininess = other.shininess;
		opacity = other.opacity;
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		other.shininess = 0.0f;
		other.opacity = 0.0f;
		other.VAO = 0;
		other.VBO = 0;
		other.EBO = 0;
	}
	return *this;
}

void Mesh::draw(const Shader& shader)
{
	shader.use();
	unsigned int ambientNr = 0;
	unsigned int diffuseNr = 0;
	unsigned int specularNr = 0;
	for (unsigned int idx{}; const auto & texture: textures) {
		texture->bind(idx);
		std::string number;
		std::string texType = texture->getType();
		if (texType == "ambient") {
			number = std::to_string(++ambientNr);
		}
		else if (texType == "diffuse") {
			number = std::to_string(++diffuseNr);
		}
		else if (texType == "specular") {
			number = std::to_string(++specularNr);
		}
		else {
			std::cerr << "Unknown texture type" << std::endl;
			continue;
		}
		shader.setUniform("material." + texType + number, idx);
		++idx;
	}
	shader.setUniform("material.shininess", shininess);
	shader.setUniform("material.opacity", opacity);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	for (unsigned int idx{}; const auto & texture : textures) {
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, 0);
		++idx;
	}
}

void Mesh::drawInstanced(const Shader& shader, const std::vector<glm::vec3>& translations)
{
	shader.use();
	unsigned int ambientNr = 0;
	unsigned int diffuseNr = 0;
	unsigned int specularNr = 0;
	for (unsigned int idx{}; const auto & texture: textures) {
		texture->bind(idx);
		std::string number;
		std::string texType = texture->getType();
		if (texType == "ambient") {
			number = std::to_string(++ambientNr);
		}
		else if (texType == "diffuse") {
			number = std::to_string(++diffuseNr);
		}
		else if (texType == "specular") {
			number = std::to_string(++specularNr);
		}
		else {
			std::cerr << "Unknown texture type" << std::endl;
			continue;
		}
		shader.setUniform("material." + texType + number, idx);
		++idx;
	}
	shader.setUniform("material.shininess", shininess);
	shader.setUniform("material.opacity", opacity);

	for (unsigned int idx{}; const auto & translation : translations) {
		shader.setUniform(("offsets[" + std::to_string(idx) + "]"), translation);
		++idx;
	}

	glBindVertexArray(VAO);

	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, translations.size() * sizeof(glm::vec3), translations.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, static_cast<unsigned int>(translations.size()));

	glBindVertexArray(0);
	for (unsigned int idx{}; const auto & texture : textures) {
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, 0);
		++idx;
	}
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));	// texture coords
	glEnableVertexAttribArray(2);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}
