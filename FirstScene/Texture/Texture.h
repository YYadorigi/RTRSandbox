#pragma once
#include <iostream>
#include <glad/glad.h>

class Texture
{
public:
	virtual ~Texture();
	inline void bind() const { glBindTexture(target, ID); }
	inline unsigned int getID() const { return ID; }
protected:
	Texture(unsigned int target);
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
	void setWrapParameters(unsigned int wrapS, unsigned int wrapT) const;
	void setFilterParameters(unsigned int minFilter, unsigned int magFilter) const;
protected:
	unsigned int ID;
	unsigned int target;
};