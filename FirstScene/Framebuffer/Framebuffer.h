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
private:
	unsigned int getColorAttachmentIndex();
	unsigned int FBO;
	unsigned int width, height;
	std::vector<RenderTexture2D> colorAttachments;
	std::shared_ptr<Renderbuffer> renderbuffer;
};