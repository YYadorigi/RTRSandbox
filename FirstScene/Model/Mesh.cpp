#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture2D> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	// Load vertex data
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
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int ambientNr = 0;
	unsigned int diffuseNr = 0;
	unsigned int specularNr = 0;
	for (unsigned int idx{}; const auto & texture: textures) {
		std::string number;
		std::string texType = texture.getType();
		if (texType == "ambient") {
			number = std::to_string(ambientNr++);
		} else if (texType == "diffuse") {
			number = std::to_string(diffuseNr++);
		} else if (texType == "specular") {
			number = std::to_string(specularNr++);
		} else {
			std::cout << "Unknown texture type" << std::endl;
			continue;
		}
		glActiveTexture(GL_TEXTURE0 + idx);
		shader.setInt("material." + texType + number, texture.bind());
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
}
