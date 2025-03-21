#pragma once
#include <array>
#include "Texture.h"

class CubeMap : public Texture
{
public:
	CubeMap(
		std::array<std::string, 6> paths,
		unsigned int mipmapLevel,
		unsigned int wrapS,
		unsigned int wrapT,
		unsigned int wrapR,
		unsigned int minFilter,
		unsigned int magFilter,
		bool flipY = false
	);
	virtual ~CubeMap() override = default;
	CubeMap(const CubeMap& other) = delete;
	CubeMap& operator=(const CubeMap& other) = delete;
	CubeMap(CubeMap&& other) noexcept;
	CubeMap& operator=(CubeMap&& other) noexcept;
private:
};