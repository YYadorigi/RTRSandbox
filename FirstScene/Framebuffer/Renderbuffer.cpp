#include "Renderbuffer.h"

Renderbuffer::Renderbuffer(unsigned int width, unsigned int height, RBOType type, bool msaa) :
	width(width), height(height), type(type)
{
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	unsigned int format = 0;
	switch (type) {
		case RBOType::DEPTH:
			format = GL_DEPTH_COMPONENT24;
			break;
		case RBOType::STENCIL:
			format = GL_STENCIL_INDEX8;
			break;
		case RBOType::DEPTH_AND_STENCIL:
			format = GL_DEPTH24_STENCIL8;
			break;
		default:
			break;
	}
	if (msaa) {
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, format, width, height);
	} else {
		glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

Renderbuffer::~Renderbuffer()
{
	glDeleteRenderbuffers(1, &RBO);
}

Renderbuffer::Renderbuffer(Renderbuffer&& other) noexcept
{
	RBO = other.RBO;
	width = other.width;
	height = other.height;
	type = other.type;
	other.RBO = 0;
	other.width = 0;
	other.height = 0;
	other.type = RBOType::NONE;
}

Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept
{
	if (this != &other) {
		glDeleteRenderbuffers(1, &RBO);
		RBO = other.RBO;
		width = other.width;
		height = other.height;
		type = other.type;
		other.RBO = 0;
		other.width = 0;
		other.height = 0;
		other.type = RBOType::NONE;
	}
	return *this;
}
