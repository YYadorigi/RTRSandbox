#pragma once
#include <iostream>
#include <glad/glad.h>

enum class RBOType
{
	NONE,
	DEPTH,
	STENCIL,
	DEPTH_AND_STENCIL,
};

class Renderbuffer
{
public:
	Renderbuffer() = default;
	Renderbuffer(unsigned int width, unsigned int height, RBOType type, bool msaa = false);
	~Renderbuffer();
	Renderbuffer(const Renderbuffer& other) = delete;
	Renderbuffer& operator=(const Renderbuffer& other) = delete;
	Renderbuffer(Renderbuffer&& other) noexcept;
	Renderbuffer& operator=(Renderbuffer&& other) noexcept;
	inline unsigned int getID() const { return RBO; }
	inline RBOType getType() const { return type; }
	inline std::pair<unsigned int, unsigned int> getSize() const { return { width, height }; }
private:
	unsigned int RBO;
	unsigned int width, height;
	RBOType type;
};

