#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
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

TextureMap::TextureMap(unsigned int target, bool sRGB, bool flipY) :
	Texture(target), sRGB(sRGB), flipY(flipY)
{}

TextureMap::TextureMap(TextureMap&& other) noexcept : Texture(std::move(other))
{
	sRGB = other.sRGB;
	flipY = other.flipY;
	other.sRGB = false;
	other.flipY = false;
}

TextureMap& TextureMap::operator=(TextureMap&& other) noexcept
{
	if (this != &other) {
		Texture::operator=(std::move(other));
		sRGB = other.sRGB;
		flipY = other.flipY;
		other.sRGB = false;
		other.flipY = false;
	}
	return *this;
}

RenderTexture::RenderTexture(unsigned int target, unsigned int internalFormat, bool multisampled) :
	Texture(target), multisampled(multisampled), internalFormat(internalFormat)
{}

RenderTexture::RenderTexture(RenderTexture&& other) noexcept : Texture(std::move(other))
{
	multisampled = other.multisampled;
	internalFormat = other.internalFormat;
	other.multisampled = false;
	other.internalFormat = 0;
}

RenderTexture& RenderTexture::operator=(RenderTexture&& other) noexcept
{
	if (this != &other) {
		Texture::operator=(std::move(other));
		multisampled = other.multisampled;
		internalFormat = other.internalFormat;
		other.multisampled = false;
		other.internalFormat = 0;
	}
	return *this;
}

TextureMap2D::TextureMap2D(
	const char* path,
	std::string name,
	std::string type,
	unsigned int mipmapLevel,
	unsigned int wrapS,
	unsigned int wrapT,
	unsigned int minFilter,
	unsigned int magFilter,
	bool sRGB,
	bool flipY
) : TextureMap(GL_TEXTURE_2D, sRGB, flipY), name(name), type(type)
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

TextureMap2D::TextureMap2D(TextureMap2D&& other) noexcept : TextureMap(std::move(other))
{
	name = std::move(other.name);
	type = std::move(other.type);
}

TextureMap2D& TextureMap2D::operator=(TextureMap2D&& other) noexcept
{
	if (this != &other) {
		TextureMap::operator=(std::move(other));
		name = std::move(other.name);
		type = std::move(other.type);
	}
	return *this;
}

TextureMapCube::TextureMapCube(
	std::array<std::string, 6> paths,
	unsigned int mipmapLevel,
	unsigned int wrapS,
	unsigned int wrapT,
	unsigned int wrapR,
	unsigned int minFilter,
	unsigned int magFilter,
	bool sRGB,
	bool flipY
) : TextureMap(GL_TEXTURE_CUBE_MAP, sRGB, flipY)
{
	int width, height, nrChannels;

	for (unsigned int idx{}; const auto & path : paths) {
		stbi_set_flip_vertically_on_load(flipY);

		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
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

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, mipmapLevel, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		} else {
			std::cerr << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);
		++idx;
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	setWrapParameters(wrapS, wrapT, wrapR);
	setFilterParameters(minFilter, magFilter);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


TextureMapCube::TextureMapCube(TextureMapCube&& other) noexcept : TextureMap(std::move(other))
{}

TextureMapCube& TextureMapCube::operator=(TextureMapCube&& other) noexcept
{
	if (this != &other) {
		TextureMap::operator=(std::move(other));
	}
	return *this;
}

RenderTexture2D::RenderTexture2D(
	unsigned int width,
	unsigned int height,
	unsigned int internalFormat,
	unsigned int format,
	unsigned int dataType,
	unsigned int wrapS,
	unsigned int wrapT,
	unsigned int minFilter,
	unsigned int magFilter,
	bool multisampled
) : RenderTexture(multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, internalFormat, multisampled)
{
	if (multisampled) {
		glTexImage2DMultisample(target, 4, internalFormat, width, height, GL_TRUE);
	} else {
		glTexImage2D(target, 0, internalFormat, width, height, 0, format, dataType, nullptr);
	}
	setWrapParameters(wrapS, wrapT);
	setFilterParameters(minFilter, magFilter);
	glBindTexture(target, 0);
}

RenderTexture2D::RenderTexture2D(RenderTexture2D&& other) noexcept : RenderTexture(std::move(other))
{}

RenderTexture2D& RenderTexture2D::operator=(RenderTexture2D&& other) noexcept
{
	if (this != &other) {
		RenderTexture::operator=(std::move(other));
	}
	return *this;
}

RenderTextureCube::RenderTextureCube(
	unsigned int width,
	unsigned int height,
	unsigned int internalFormat,
	unsigned int format,
	unsigned int dataType,
	unsigned int wrapS,
	unsigned int wrapT,
	unsigned int minFilter,
	unsigned int magFilter,
	bool multisampled
) : RenderTexture(GL_TEXTURE_CUBE_MAP, internalFormat, multisampled)
{
	for (unsigned int idx = 0; idx < 6; ++idx) {
		if (multisampled) {
			glTexImage2DMultisample(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 4, internalFormat, width, height, GL_TRUE);
		} else {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, internalFormat, width, height, 0, format, dataType, nullptr);
		}
	}
	setWrapParameters(wrapS, wrapT);
	setFilterParameters(minFilter, magFilter);
	glBindTexture(target, 0);
}

RenderTextureCube::RenderTextureCube(RenderTextureCube&& other) noexcept : RenderTexture(std::move(other))
{}

RenderTextureCube& RenderTextureCube::operator=(RenderTextureCube&& other) noexcept
{
	if (this != &other) {
		RenderTexture::operator=(std::move(other));
	}
	return *this;
}
