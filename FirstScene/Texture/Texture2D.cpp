#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture2D.h"

TextureMap2D::TextureMap2D(const char* path, std::string name, std::string type, unsigned int mipmapLevel,
	unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter, bool sRGB, bool flipY)
	: Texture(GL_TEXTURE_2D), name(name), type(type)
{
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(flipY);

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		unsigned int format = GL_RGB;
		unsigned int internalFormat = GL_RGB;
		if (nrChannels == 1) {
			format = GL_RED;
			internalFormat = GL_RED;
		} else if (nrChannels == 3) {
			format = GL_RGB;
			internalFormat = sRGB ? GL_SRGB : GL_RGB;
		} else if (nrChannels == 4) {
			format = GL_RGBA;
			internalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_2D, mipmapLevel, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		setWrapParameters(wrapS, wrapT);
		setFilterParameters(minFilter, magFilter);
	} else {
		std::cerr << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

TextureMap2D::TextureMap2D(TextureMap2D&& other) noexcept : Texture(std::move(other))
{
	name = std::move(other.name);
	type = std::move(other.type);
}

TextureMap2D& TextureMap2D::operator=(TextureMap2D&& other) noexcept
{
	if (this != &other) {
		Texture::operator=(std::move(other));
		name = std::move(other.name);
		type = std::move(other.type);
	}
	return *this;
}

RenderTexture2D::RenderTexture2D(unsigned int width, unsigned int height, unsigned int internalFormat,
	unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter) :
	Texture(GL_TEXTURE_2D_MULTISAMPLE), multisampled(true)
{
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, internalFormat, width, height, GL_TRUE);
	setWrapParameters(wrapS, wrapT);
	setFilterParameters(minFilter, magFilter);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

RenderTexture2D::RenderTexture2D(unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format,
	unsigned int dataType, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter) :
	Texture(GL_TEXTURE_2D), multisampled(false)
{
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, nullptr);
	setWrapParameters(wrapS, wrapT);
	setFilterParameters(minFilter, magFilter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

RenderTexture2D::RenderTexture2D(RenderTexture2D&& other) noexcept : Texture(std::move(other))
{
	multisampled = other.multisampled;
	other.multisampled = false;
}

RenderTexture2D& RenderTexture2D::operator=(RenderTexture2D&& other) noexcept
{
	if (this != &other) {
		Texture::operator=(std::move(other));
		multisampled = other.multisampled;
		other.multisampled = false;
	}
	return *this;
}
