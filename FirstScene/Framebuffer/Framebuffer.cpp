#include "Framebuffer.h"

Framebuffer::Framebuffer(unsigned int width, unsigned int height) : width(width), height(height)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachColorTexture(unsigned int internalFormat, unsigned int format, unsigned int dataType, unsigned int filter)
{
	unsigned int index = attachmentCount++;

	int maxAttach;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
	if (index >= (unsigned int)maxAttach) {
		std::cerr << "Exceeded maximum number of color attachments" << std::endl;
		return;
	}

	RenderTexture2D texture = RenderTexture2D(
		width, height,
		internalFormat, format, dataType,
		GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
		filter, filter
	);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.getID(), 0);
	colorAttachments.emplace_back(std::move(texture));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachRenderbuffer(std::shared_ptr<Renderbuffer> renderbuffer)
{
	std::pair<unsigned int, unsigned int> size = renderbuffer->getSize();
	if (size.first != width || size.second != height) {
		std::cerr << "Renderbuffer size does not match framebuffer size" << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	unsigned int attachment = 0;
	switch (renderbuffer->getType()) {
		case RBOType::DEPTH:
			attachment = GL_DEPTH_ATTACHMENT;
			break;
		case RBOType::STENCIL:
			attachment = GL_STENCIL_ATTACHMENT;
			break;
		case RBOType::DEPTH_AND_STENCIL:
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			break;

	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getID());
	this->renderbuffer = renderbuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::drawBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	std::vector<unsigned int> attachments;
	for (unsigned int idx{}; idx < colorAttachments.size(); idx++) {
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + idx);
	}
	glDrawBuffers(static_cast<unsigned int>(attachments.size()), attachments.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}