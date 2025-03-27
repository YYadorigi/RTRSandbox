#pragma once
#include <memory>
#include "Texture/Texture.h"
#include "Shader/shader.h"

class Skybox
{
public:
	Skybox(const char* path, unsigned int mipmapLevel = 0, bool sRGB = false, bool flipY = false);
	~Skybox();
	Skybox(const Skybox& other) = delete;
	Skybox& operator=(const Skybox& other) = delete;
	Skybox(Skybox&& other) noexcept;
	Skybox& operator=(Skybox&& other) noexcept;

	void draw(Shader& shader);
private:
	std::shared_ptr<TextureMapCube> cubeMap;
	unsigned int VAO, VBO;
};

