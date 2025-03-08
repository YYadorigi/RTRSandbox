#pragma once
#include <glad/glad.h>
#include "Shader/Shader.h"
#include "Framebuffer.h"

class ScreenQuad
{
public:
	ScreenQuad();
	~ScreenQuad();
	ScreenQuad(const ScreenQuad& other) = delete;
	ScreenQuad& operator=(const ScreenQuad& other) = delete;
	ScreenQuad(ScreenQuad&& other) noexcept;
	ScreenQuad& operator=(ScreenQuad&& other) noexcept;
	void draw(Framebuffer& framebuffer, Shader& shader) const;
private:
	unsigned int VAO, VBO;
};

