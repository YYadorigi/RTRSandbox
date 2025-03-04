#pragma once
#include <iostream>
#include <glad/glad.h>
#include <assimp/scene.h>

class Texture2D
{
public:
	Texture2D(const char* path, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter,
		unsigned int mipmapLevel, unsigned int sourceDataType, std::string type);
	~Texture2D();
	Texture2D(const Texture2D& other) = default;
	Texture2D& operator=(const Texture2D& other) = default;
	Texture2D(Texture2D&& other) noexcept = default;
	Texture2D& operator=(Texture2D&& other) noexcept = default;
	inline std::string getType() const { return type; }
	inline unsigned int bind() const { glBindTexture(GL_TEXTURE_2D, ID); return ID; }
private:
	unsigned int ID;
	std::string type;	// e.g., ambient, diffuse, specular
};