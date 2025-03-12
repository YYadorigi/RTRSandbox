#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader/Shader.h"
#include "Framebuffer.h"

struct ScreenQuadTexture
{
	std::string name;
	Framebuffer& framebuffer;
	unsigned int attachmentIndex;
};

class ScreenQuad
{
public:
	ScreenQuad(float scale = 1.0f, glm::vec2 movement = glm::vec2(0.0f));
	~ScreenQuad();
	ScreenQuad(const ScreenQuad& other) = delete;
	ScreenQuad& operator=(const ScreenQuad& other) = delete;
	ScreenQuad(ScreenQuad&& other) noexcept;
	ScreenQuad& operator=(ScreenQuad&& other) noexcept;
	void draw(Shader& shader, std::vector<ScreenQuadTexture> textures) const;
private:
	unsigned int VAO, VBO;
};

