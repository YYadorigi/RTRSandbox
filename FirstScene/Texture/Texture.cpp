#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "texture.h"

Texture2D::Texture2D(const char* path, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter,
	unsigned int mipmapLevel, unsigned int sourceDataType, std::string name, std::string type)
{
	glGenTextures(1, &ID);
	this->name = name;
	this->type = type;

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, mipmapLevel, format, width, height, 0, format, sourceDataType, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		stbi_image_free(data);
	} else {
		std::cerr << "Failed to load texture" << std::endl;
		stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &ID);
}

Texture2D::Texture2D(Texture2D&& other) noexcept
{
	ID = other.ID;
	name = other.name;
	type = other.type;
	other.ID = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
	if (this != &other) {
		glDeleteTextures(1, &ID);
		ID = other.ID;
		name = other.name;
		type = other.type;
		other.ID = 0;
	}
	return *this;
}
