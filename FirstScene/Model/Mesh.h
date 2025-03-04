#pragma once
#include <iostream>
#include <vector>
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
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture2D> textures);
	~Mesh();
	Mesh(const Mesh& other) = default;
	Mesh& operator=(const Mesh& other) = default;
	Mesh(Mesh&& other) noexcept = default;
	Mesh& operator=(Mesh&& other) noexcept = default;
	void Draw(Shader& shader);
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;
	unsigned int VAO, VBO, EBO;
};