#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture/Texture.h"
#include "Shader/Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh
{
public:
	Mesh(
		std::vector<Vertex> vertices,
		std::vector<unsigned int> indices,
		std::vector<std::shared_ptr<TextureMap2D>> textures,
		float shininess,
		float opacity
	);
	~Mesh();
	Mesh(const Mesh& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;

	void draw(const Shader& shader) const;
	void drawInstanced(const Shader& shader, const std::vector<glm::vec3>& translations) const;
private:
	void setupMesh();
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<TextureMap2D>> textures;
	float shininess, opacity;
	unsigned int VAO, VBO, EBO;
};