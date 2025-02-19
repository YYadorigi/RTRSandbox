#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture2D::Texture2D(const char* path, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter,
	unsigned int mipmapLevel, unsigned int saveFormat, unsigned int sourceFormat, unsigned int sourceDataType)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Set texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	// Load image
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, mipmapLevel, saveFormat, width, height, 0, sourceFormat, sourceDataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &ID);
}

Texture2D::Texture2D(const Texture2D& other)
{
	ID = other.ID;
}

Texture2D& Texture2D::operator=(const Texture2D& other)
{
	this->ID = other.ID;
	return *this;
}

Texture2D::Texture2D(Texture2D&& other) noexcept
{
	ID = other.ID;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
	this->ID = other.ID;
	return *this;
}

void Texture2D::bind()
{
	glBindTexture(GL_TEXTURE_2D, ID);
}
