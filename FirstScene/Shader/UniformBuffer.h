#pragma once
#include <iostream>
#include <glad/glad.h>

class UniformBuffer
{
public:
	UniformBuffer(unsigned int size);
	~UniformBuffer();
	UniformBuffer(const UniformBuffer& other) = delete;
	UniformBuffer& operator=(const UniformBuffer& other) = delete;
	UniformBuffer(UniformBuffer&& other) noexcept;
	UniformBuffer& operator=(UniformBuffer&& other) noexcept;
	inline void setData(const void* data, unsigned int size, unsigned int offset) const
	{
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}
	inline void bind(unsigned int bindingPoint) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
	}
private:
	unsigned int UBO;
};

