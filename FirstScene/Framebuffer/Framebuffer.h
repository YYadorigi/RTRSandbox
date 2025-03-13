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
	void attachColorTexture(unsigned int internalFormat, unsigned int format, unsigned dataType);
	void attachRenderbuffer(RBOType type);
	void attachRenderbuffer(std::shared_ptr<Renderbuffer> renderbuffer);
	void configureColorAttachments();
	inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }
	void blitColorTexture(unsigned int selfIndex, Framebuffer& other, unsigned int otherIndex) const;
	void blitRenderbuffer(Framebuffer& other) const;
	void bindColorTexture(unsigned int index) const;
private:
	unsigned int FBO;
	unsigned int width, height;
	bool msaa;
	std::vector<RenderTexture2D> colorAttachments;
	unsigned int attachmentCount = 0;
	std::shared_ptr<Renderbuffer> renderbuffer;
};