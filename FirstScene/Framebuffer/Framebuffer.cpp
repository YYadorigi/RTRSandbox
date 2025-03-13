#include "Framebuffer.h"

Framebuffer::Framebuffer(unsigned int width, unsigned int height, bool msaa) :
	width(width), height(height), msaa(msaa)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachColorTexture(unsigned int internalFormat, unsigned int format, unsigned dataType)
{
	unsigned int index = attachmentCount++;

	int maxAttach;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
	if (index >= static_cast<unsigned int>(maxAttach)) {
		std::cerr << "Exceeded maximum number of color attachments" << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	if (msaa) {
		RenderTexture2D textureMSAA = RenderTexture2D(
			width, height, internalFormat,
			GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
			GL_LINEAR, GL_LINEAR
		);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, textureMSAA.getID(), 0);
		colorAttachments.emplace_back(std::move(textureMSAA));
	} else {
		RenderTexture2D texture = RenderTexture2D(
			width, height, internalFormat, format, dataType,
			GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
			GL_LINEAR, GL_LINEAR
		);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.getID(), 0);
		colorAttachments.emplace_back(std::move(texture));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachRenderbuffer(RBOType type)
{
	std::shared_ptr<Renderbuffer> renderbuffer = std::make_shared<Renderbuffer>(width, height, type, msaa);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	unsigned int attachment = 0;
	switch (type) {
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

void Framebuffer::attachRenderbuffer(std::shared_ptr<Renderbuffer> renderbuffer)
{
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

void Framebuffer::configureColorAttachments()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	std::vector<unsigned int> attachments;
	for (unsigned int idx{}; idx < colorAttachments.size(); idx++) {
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + idx);
	}
	glDrawBuffers(static_cast<unsigned int>(attachments.size()), attachments.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::blitColorTexture(unsigned int selfIndex, Framebuffer& other, unsigned int otherIndex) const
{
	int currentFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other.FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + selfIndex);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + otherIndex);
	glBlitFramebuffer(0, 0, width, height, 0, 0, other.width, other.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
}

void Framebuffer::blitRenderbuffer(Framebuffer& other) const
{
	int currentFBO;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other.FBO);
	glBlitFramebuffer(
		0, 0, width, height,
		0, 0, other.width, other.height,
		GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
		GL_NEAREST
	);

	glBindFramebuffer(GL_FRAMEBUFFER, currentFBO);
}

void Framebuffer::bindColorTexture(unsigned int index) const
{
	colorAttachments[index].bind();
}
