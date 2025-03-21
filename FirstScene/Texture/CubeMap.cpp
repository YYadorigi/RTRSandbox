#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "CubeMap.h"

CubeMap::CubeMap(
	std::array<std::string, 6> paths,
	unsigned int mipmapLevel,
	unsigned int wrapS,
	unsigned int wrapT,
	unsigned int wrapR,
	unsigned int minFilter,
	unsigned int magFilter,
	bool flipY
) : Texture(GL_TEXTURE_CUBE_MAP)
{
	int width, height, nrChannels;
	for (unsigned int idx{}; const auto & path : paths) {
		stbi_set_flip_vertically_on_load(flipY);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			unsigned int format = GL_RGB;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, mipmapLevel, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

CubeMap::CubeMap(CubeMap&& other) noexcept :Texture(std::move(other))
{}

CubeMap& CubeMap::operator=(CubeMap&& other) noexcept
{
	if (this != &other) {
		Texture::operator=(std::move(other));
	}
	return *this;
}
