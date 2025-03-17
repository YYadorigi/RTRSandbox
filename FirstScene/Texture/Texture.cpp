#include "Texture.h"

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::bind(unsigned int targetIndex) const
{
	glActiveTexture(GL_TEXTURE0 + targetIndex);
	glBindTexture(target, ID);
}

Texture::Texture(unsigned int target) : target(target)
{
	glGenTextures(1, &ID);
	glBindTexture(target, ID);
}

Texture::Texture(Texture&& other) noexcept
{
	ID = other.ID;
	target = other.target;
	other.ID = 0;
	other.target = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this != &other) {
		glDeleteTextures(1, &ID);
		ID = other.ID;
		target = other.target;
		other.ID = 0;
		other.target = 0;
	}
	return *this;
}

void Texture::setWrapParameters(unsigned int wrapS, unsigned int wrapT, unsigned int wrapR) const
{
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	if (wrapR) {
		glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
	}
}

void Texture::setFilterParameters(unsigned int minFilter, unsigned int magFilter) const
{
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
}