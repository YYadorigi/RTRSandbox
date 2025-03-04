#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "texture.h"

Texture2D::Texture2D(const char* path, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter,
	unsigned int mipmapLevel, unsigned int sourceDataType, std::string type)
{
	glGenTextures(1, &ID);
	this->type = type;

	int width, height, nrChannels;
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
		std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);
	}
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &ID);
}
