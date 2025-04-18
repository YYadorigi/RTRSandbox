#include <glad/glad.h>
#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(unsigned int size)
{
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &UBO);
}

UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
{
	UBO = other.UBO;
	other.UBO = 0;
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept
{
	if (this != &other) {
		glDeleteBuffers(1, &UBO);
		UBO = other.UBO;
		other.UBO = 0;
	}
	return *this;
}

void UniformBuffer::loadData(const void* data, unsigned int size)
{
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	offset += size;
}

void UniformBuffer::resetOffset()
{
	offset = 0;
}

void UniformBuffer::alignOffset(unsigned int alignment)
{
	offset = (offset + alignment - 1) & ~(alignment - 1);
}

void UniformBuffer::bind(unsigned int bindingPoint) const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
}

