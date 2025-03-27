#include <iostream>
#include <glad/glad.h>
#include "Framebuffer.h"

Framebuffer::Framebuffer(unsigned int width, unsigned int height, bool msaa) :
	width(width), height(height), msaa(msaa)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &FBO);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
{
	colorAttachments = std::move(other.colorAttachments);
	depthStencilAttachment = std::move(other.depthStencilAttachment);
	FBO = other.FBO;
	width = other.width;
	height = other.height;
	msaa = other.msaa;
	colorAttachmentCount = other.colorAttachmentCount;
	other.FBO = 0;
	other.width = 0;
	other.height = 0;
	other.msaa = false;
	other.colorAttachmentCount = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	if (this != &other) {
		glDeleteFramebuffers(1, &FBO);
		colorAttachments = std::move(other.colorAttachments);
		depthStencilAttachment = std::move(other.depthStencilAttachment);
		FBO = other.FBO;
		width = other.width;
		height = other.height;
		msaa = other.msaa;
		colorAttachmentCount = other.colorAttachmentCount;
		other.FBO = 0;
		other.width = 0;
		other.height = 0;
		other.msaa = false;
		other.colorAttachmentCount = 0;
	}
	return *this;
}

std::shared_ptr<RenderTexture2D> Framebuffer::attachColorTexture(unsigned int internalFormat, unsigned int format, unsigned dataType)
{
	unsigned int index = colorAttachmentCount++;

	int maxAttach;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
	if (index >= static_cast<unsigned int>(maxAttach)) {
		std::cerr << "Exceeded maximum number of color attachments" << std::endl;
		return nullptr;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	std::shared_ptr<RenderTexture2D> texture = std::make_shared<RenderTexture2D>(
		width,
		height,
		internalFormat,
		format,
		dataType,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_EDGE,
		GL_LINEAR,
		GL_LINEAR,
		msaa
	);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture->getTarget(), texture->getID(), 0);

	colorAttachments.emplace_back(texture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return texture;
}

void Framebuffer::attachColorTexture(const std::shared_ptr<RenderTexture2D>& texture)
{
	unsigned int index = colorAttachmentCount++;
	int maxAttach;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
	if (index >= static_cast<unsigned int>(maxAttach)) {
		std::cerr << "Exceeded maximum number of color attachments" << std::endl;
		return;
	}

	if (msaa != texture->isMultisampled()) {
		std::cerr << "Texture multisampling does not match framebuffer multisampling" << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture->getTarget(), texture->getID(), 0);

	colorAttachments.emplace_back(texture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<RenderTexture2D> Framebuffer::attachDepthTexture(DepthStencilType type)
{
	if (depthStencilAttachment != nullptr) {
		std::cerr << "Depth stencil attachment already exists" << std::endl;
		return nullptr;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int internalFormat = 0;
	unsigned int format = 0;
	unsigned int dataType = 0;
	unsigned int attachment = 0;
	switch (type) {
		case DepthStencilType::DEPTH:
		{
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			dataType = GL_FLOAT;
			attachment = GL_DEPTH_ATTACHMENT;
			break;
		}
		case DepthStencilType::STENCIL:
		{
			internalFormat = GL_STENCIL_INDEX8;
			format = GL_STENCIL_INDEX;
			dataType = GL_UNSIGNED_BYTE;
			attachment = GL_STENCIL_ATTACHMENT;
			break;
		}
		case DepthStencilType::DEPTH_STENCIL:
		{
			internalFormat = GL_DEPTH24_STENCIL8;
			format = GL_DEPTH_STENCIL;
			dataType = GL_UNSIGNED_INT_24_8;
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		}
		default: break;
	}

	std::shared_ptr<RenderTexture2D> texture = std::make_shared<RenderTexture2D>(
		width,
		height,
		internalFormat,
		format,
		dataType,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_EDGE,
		GL_LINEAR,
		GL_LINEAR,
		msaa
	);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture->getTarget(), texture->getID(), 0);

	depthStencilAttachment = texture;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return texture;
}

void Framebuffer::attachDepthTexture(const std::shared_ptr<RenderTexture2D>& texture)
{
	if (depthStencilAttachment != nullptr) {
		std::cerr << "Depth stencil attachment already exists" << std::endl;
		return;
	}

	if (msaa != texture->isMultisampled()) {
		std::cerr << "Texture multisampling does not match framebuffer multisampling" << std::endl;
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int attachment = 0;
	switch (texture->getInternalFormat()) {
		case GL_DEPTH_COMPONENT24:
			attachment = GL_DEPTH_ATTACHMENT;
			break;
		case GL_STENCIL_INDEX8:
			attachment = GL_STENCIL_ATTACHMENT;
			break;
		case GL_DEPTH24_STENCIL8:
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		default:
			break;
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture->getTarget(), texture->getID(), 0);

	depthStencilAttachment = texture;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::configureColorAttachments(const std::vector<unsigned int>& indices) const
{
	std::vector<unsigned int> attachments;
	for (const auto& index : indices) {
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + index);
	}
	glDrawBuffers(static_cast<unsigned int>(attachments.size()), attachments.data());
}

void Framebuffer::configureColorAttachments(std::vector<unsigned int>&& indices) const
{
	std::vector<unsigned int> attachments;
	for (const auto& index : indices) {
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + index);
	}
	glDrawBuffers(static_cast<unsigned int>(attachments.size()), attachments.data());
}

void Framebuffer::blitColorTexture(unsigned int selfIndex, const Framebuffer& other, unsigned int otherIndex) const
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

void Framebuffer::blitDepthTexture(const Framebuffer& other) const
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

void Framebuffer::bindTexture(int texIndex, unsigned int targetIndex) const
{
	if (texIndex < 0) {
		depthStencilAttachment->bind(targetIndex);
	}
	else {
		colorAttachments[texIndex]->bind(targetIndex);
	}
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, width, height);
}
