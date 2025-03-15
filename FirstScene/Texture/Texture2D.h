#pragma once
#include <iostream>
#include <glad/glad.h>
#include "Texture.h"

class TextureMap2D : public Texture
{
public:
	TextureMap2D(const char* path, std::string name, std::string type, unsigned int mipmapLevel,
		unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter, bool flipY = false);
	virtual ~TextureMap2D() override = default;
	TextureMap2D(const TextureMap2D& other) = delete;
	TextureMap2D& operator=(const TextureMap2D& other) = delete;
	TextureMap2D(TextureMap2D&& other) noexcept;
	TextureMap2D& operator=(TextureMap2D&& other) noexcept;
	inline std::string getName() const { return name; }
	inline std::string getType() const { return type; }
private:
	std::string name;	// used by assimp to identify textures
	std::string type;	// used for drawing meshes, e.g., ambient, diffuse, specular
};

class RenderTexture2D : public Texture
{
public:
	RenderTexture2D(unsigned int width, unsigned int height, unsigned int internalFormat,
		unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter);
	RenderTexture2D(unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int dataType,
		unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter);
	virtual ~RenderTexture2D() override = default;
	RenderTexture2D(const RenderTexture2D& other) = delete;
	RenderTexture2D& operator=(const RenderTexture2D& other) = delete;
	RenderTexture2D(RenderTexture2D&& other) noexcept;
	RenderTexture2D& operator=(RenderTexture2D&& other) noexcept;
};