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
	renderbuffer = std::move(other.renderbuffer);
	FBO = other.FBO;
	width = other.width;
	height = other.height;
	msaa = other.msaa;
	attachmentCount = other.attachmentCount;
	other.FBO = 0;
	other.width = 0;
	other.height = 0;
	other.msaa = false;
	other.attachmentCount = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	if (this != &other) {
		glDeleteFramebuffers(1, &FBO);

		colorAttachments = std::move(other.colorAttachments);
		renderbuffer = std::move(other.renderbuffer);
		FBO = other.FBO;
		width = other.width;
		height = other.height;
		msaa = other.msaa;
		attachmentCount = other.attachmentCount;
		other.FBO = 0;
		other.width = 0;
		other.height = 0;
		other.msaa = false;
		other.attachmentCount = 0;
	}
	return *this;
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
		std::shared_ptr<RenderTexture2D> textureMSAA = std::make_shared<RenderTexture2D>(
			width, height, internalFormat,
			GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
			GL_LINEAR, GL_LINEAR
		);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, textureMSAA->getID(), 0);
		colorAttachments.emplace_back(textureMSAA);
	} else {
		std::shared_ptr<RenderTexture2D> texture = std::make_shared<RenderTexture2D>(
			width, height, internalFormat, format, dataType,
			GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
			GL_LINEAR, GL_LINEAR
		);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->getID(), 0);
		colorAttachments.emplace_back(texture);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachColorTexture(std::shared_ptr<RenderTexture2D> texture)
{
	unsigned int index = attachmentCount++;
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
	if (msaa) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, texture->getID(), 0);
	} else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->getID(), 0);
	}
	colorAttachments.emplace_back(texture);
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

void Framebuffer::configureColorAttachments(std::vector<unsigned int> indices) const
{
	std::vector<unsigned int> attachments;
	for (const auto& index : indices) {
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + index);
	}
	glDrawBuffers(static_cast<unsigned int>(attachments.size()), attachments.data());
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
	colorAttachments[index]->bind();
}
