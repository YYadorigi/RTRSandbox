#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include "Texture/Texture2D.h"
#include "Renderbuffer.h"

class Framebuffer
{
public:
	Framebuffer(unsigned int width, unsigned int height, bool msaa = false);
	~Framebuffer();
	Framebuffer(const Framebuffer& other) = delete;
	Framebuffer& operator=(const Framebuffer& other) = delete;
	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	void attachColorTexture(unsigned int internalFormat, unsigned int format, unsigned dataType);
	void attachColorTexture(const std::shared_ptr<RenderTexture2D> texture);

	void attachRenderbuffer(RBOType type);
	void attachRenderbuffer(const std::shared_ptr<Renderbuffer> renderbuffer);

	void configureColorAttachments(const std::vector<unsigned int>& indices) const;
	void configureColorAttachments(std::vector<unsigned int>&& indices) const;

	inline void bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glViewport(0, 0, width, height);
	}
	void blitColorTexture(unsigned int selfIndex, const Framebuffer& other, unsigned int otherIndex) const;
	void blitRenderbuffer(const Framebuffer& other) const;
	void bindColorTexture(unsigned int index, unsigned int targetIndex) const;
	unsigned int getColorTextureID(unsigned int index) const;
private:
	unsigned int FBO;
	unsigned int width, height;
	bool msaa;
	std::vector<std::shared_ptr<RenderTexture2D>> colorAttachments;
	unsigned int attachmentCount = 0;
	std::shared_ptr<Renderbuffer> renderbuffer;
};