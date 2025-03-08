#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class TestingType
{
	NONE,
	DEPTH,
	STENCIL,
	DEPTH_AND_STENCIL,
};

class Framebuffer
{
public:
	Framebuffer(unsigned int width, unsigned int height, TestingType testingType);
	~Framebuffer();
	Framebuffer(const Framebuffer& other) = delete;
	Framebuffer& operator=(const Framebuffer& other) = delete;
	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;
	inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }
	inline void bindTex() const { glBindTexture(GL_TEXTURE_2D, colorTex); }
private:
	unsigned int FBO, colorTex, RBO;
};