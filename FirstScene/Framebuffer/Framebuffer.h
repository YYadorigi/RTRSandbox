#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include "Texture/Texture2D.h"
#include "Renderbuffer.h"

class Framebuffer
{
public:
	Framebuffer(unsigned int width, unsigned int height);
	void attachColorTexture(unsigned int internalFormat, unsigned int format, unsigned int dataType, unsigned int filter);
	void attachRenderbuffer(std::shared_ptr<Renderbuffer> renderbuffer);
	void drawBuffers();
	inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, FBO); }
	inline void bindColorTexture(unsigned int index) const { colorAttachments[index].bind(); }
private:
	unsigned int FBO;
	unsigned int width, height;
	std::vector<RenderTexture2D> colorAttachments;
	unsigned int attachmentCount = 0;
	std::shared_ptr<Renderbuffer> renderbuffer;
};