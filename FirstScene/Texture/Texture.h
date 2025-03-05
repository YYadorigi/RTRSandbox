#pragma once
#include <iostream>
#include <glad/glad.h>
#include <assimp/scene.h>

class Texture2D
{
public:
	Texture2D(const char* path, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter,
		unsigned int mipmapLevel, unsigned int sourceDataType, std::string name, std::string type);
	~Texture2D();
	Texture2D(const Texture2D& other) = delete;
	Texture2D& operator=(const Texture2D& other) = delete;
	Texture2D(Texture2D&& other) noexcept;
	Texture2D& operator=(Texture2D&& other) noexcept;
	inline std::string getName() const { return name; }
	inline std::string getType() const { return type; }
	inline void bind() const { glBindTexture(GL_TEXTURE_2D, ID); }
private:
	unsigned int ID;
	std::string name;	// used by assimp to identify textures
	std::string type;	// e.g., ambient, diffuse, specular
};