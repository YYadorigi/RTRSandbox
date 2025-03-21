#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <glad/glad.h>

/**
* This class represents an OpenGL texture object.
*
* It is the base class for all texture types.
*
*/
class Texture
{
public:
	virtual ~Texture();
	inline unsigned int getID() const { return ID; }
	inline unsigned int getTarget() const { return target; }
	inline void bind(unsigned int targetIndex) const
	{
		glActiveTexture(GL_TEXTURE0 + targetIndex);
		glBindTexture(target, ID);
	}
protected:
	Texture(unsigned int target);
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
	inline void setWrapParameters(unsigned int wrapS, unsigned int wrapT, unsigned int wrapR = NULL) const
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
		if (wrapR) {
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
		}
	}
	inline void setFilterParameters(unsigned int minFilter, unsigned int magFilter) const
	{
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
	}
protected:
	unsigned int ID;
	unsigned int target;
};

/**
* This class represents a texture map that loads data from a source image.
*
* sRGB refers to whether the image was created in RGB space by the artist.
* flipY refers to whether the image should be flipped along the Y axis when loaded.
* Mipmapping is supported.
*
*/
class TextureMap : public Texture
{
public:
	virtual ~TextureMap() override = default;
	inline bool isLinear() const { return !sRGB; }
protected:
	TextureMap(unsigned int target, bool sRGB = false, bool flipY = false);
	TextureMap(const TextureMap& other) = delete;
	TextureMap& operator=(const TextureMap& other) = delete;
	TextureMap(TextureMap&& other) noexcept;
	TextureMap& operator=(TextureMap&& other) noexcept;
protected:
	bool sRGB;
	bool flipY;
};

/**
* This class represents a texture attachment for a framebuffer.
*
* It serves as either a color texture or a test texture.
* In both cases, mipmapping is not allowed as it makes no sense.
* Multisampling is supported.
*
*/
class RenderTexture : public Texture
{
public:
	virtual ~RenderTexture() override = default;
	inline bool isMultisampled() const { return multisampled; }
	inline unsigned int getInternalFormat() const { return internalFormat; }
protected:
	RenderTexture(unsigned int target, unsigned int internalFormat, bool multisampled = false);
	RenderTexture(const RenderTexture& other) = delete;
	RenderTexture& operator=(const RenderTexture& other) = delete;
	RenderTexture(RenderTexture&& other) noexcept;
	RenderTexture& operator=(RenderTexture&& other) noexcept;
protected:
	bool multisampled;
	unsigned int internalFormat;
};

/**
* This class represents a texture map that loads data from a 2D image.
*/
class TextureMap2D : public TextureMap
{
public:
	TextureMap2D(
		const char* path,
		std::string name,
		std::string type,
		unsigned int mipmapLevel,
		unsigned int wrapS,
		unsigned int wrapT,
		unsigned int minFilter,
		unsigned int magFilter,
		bool sRGB = false,
		bool flipY = false
	);
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

/**
* This class represents a texture map that loads data from a cube image.
*/
class TextureMapCube : public TextureMap
{
public:
	TextureMapCube(
		std::array<std::string, 6> paths,
		unsigned int mipmapLevel,
		unsigned int wrapS,
		unsigned int wrapT,
		unsigned int wrapR,
		unsigned int minFilter,
		unsigned int magFilter,
		bool sRGB = false,
		bool flipY = false
	);
	virtual ~TextureMapCube() override = default;
	TextureMapCube(const TextureMapCube& other) = delete;
	TextureMapCube& operator=(const TextureMapCube& other) = delete;
	TextureMapCube(TextureMapCube&& other) noexcept;
	TextureMapCube& operator=(TextureMapCube&& other) noexcept;
};

/**
* This class represents a 2D texture attachment for a framebuffer.
*/
class RenderTexture2D : public RenderTexture
{
public:
	RenderTexture2D(
		unsigned int width,
		unsigned int height,
		unsigned int internalFormat,
		unsigned int format,
		unsigned int dataType,
		unsigned int wrapS,
		unsigned int wrapT,
		unsigned int minFilter,
		unsigned int magFilter,
		bool multisampled = false
	);
	virtual ~RenderTexture2D() override = default;
	RenderTexture2D(const RenderTexture2D& other) = delete;
	RenderTexture2D& operator=(const RenderTexture2D& other) = delete;
	RenderTexture2D(RenderTexture2D&& other) noexcept;
	RenderTexture2D& operator=(RenderTexture2D&& other) noexcept;
};

/**
* This class represents a cube texture attachment for a framebuffer.
*/
class RenderTextureCube : public RenderTexture
{
public:
	RenderTextureCube(
		unsigned int width,
		unsigned int height,
		unsigned int internalFormat,
		unsigned int format,
		unsigned int dataType,
		unsigned int wrapS,
		unsigned int wrapT,
		unsigned int minFilter,
		unsigned int magFilter,
		bool multisampled = false
	);
	virtual ~RenderTextureCube() override = default;
	RenderTextureCube(const RenderTextureCube& other) = delete;
	RenderTextureCube& operator=(const RenderTextureCube& other) = delete;
	RenderTextureCube(RenderTextureCube&& other) noexcept;
	RenderTextureCube& operator=(RenderTextureCube&& other) noexcept;
};