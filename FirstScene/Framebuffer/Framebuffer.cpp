#include "Framebuffer.h"

Framebuffer::Framebuffer(unsigned int width, unsigned int height, TestingType testingType)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorTex);
	glBindTexture(GL_TEXTURE_2D, colorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);


	RBO = 0;
	if (testingType != TestingType::NONE) {
		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		unsigned int format = 0;
		unsigned int attachment = 0;
		switch (testingType) {
			case TestingType::DEPTH:
				format = GL_DEPTH_COMPONENT24;
				attachment = GL_DEPTH_ATTACHMENT;
				break;
			case TestingType::STENCIL:
				format = GL_STENCIL_INDEX8;
				attachment = GL_STENCIL_ATTACHMENT;
				break;
			case TestingType::DEPTH_AND_STENCIL:
				format = GL_DEPTH24_STENCIL8;
				attachment = GL_DEPTH_STENCIL_ATTACHMENT;
				break;
			default:
				break;
		}
		glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, RBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	};

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &colorTex);
	glDeleteRenderbuffers(1, &RBO);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
{
	FBO = other.FBO;
	colorTex = other.colorTex;
	RBO = other.RBO;
	other.FBO = 0;
	other.colorTex = 0;
	other.RBO = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	if (this != &other) {
		glDeleteFramebuffers(1, &FBO);
		glDeleteTextures(1, &colorTex);
		glDeleteRenderbuffers(1, &RBO);
		FBO = other.FBO;
		colorTex = other.colorTex;
		RBO = other.RBO;
		other.FBO = 0;
		other.colorTex = 0;
		other.RBO = 0;
	}
	return *this;
}
