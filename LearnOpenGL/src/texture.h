#pragma once
#include <iostream>
#include <glad/glad.h>

class Texture2D
{
public:
	Texture2D(const char* path, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter,
		unsigned int mipmapLevel, unsigned int saveFormat, unsigned int sourceFormat, unsigned int sourceDataType);
	~Texture2D();
	Texture2D(const Texture2D& other);
	Texture2D& operator=(const Texture2D& other);
	Texture2D(Texture2D&& other) noexcept;
	Texture2D& operator=(Texture2D&& other) noexcept;
	inline void bind() const { glBindTexture(GL_TEXTURE_2D, ID); }
private:
	unsigned int ID;
};