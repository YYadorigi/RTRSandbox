#pragma once

class UniformBuffer
{
public:
	UniformBuffer(unsigned int size);
	~UniformBuffer();
	UniformBuffer(const UniformBuffer& other) = delete;
	UniformBuffer& operator=(const UniformBuffer& other) = delete;
	UniformBuffer(UniformBuffer&& other) noexcept;
	UniformBuffer& operator=(UniformBuffer&& other) noexcept;

	void setData(const void* data, unsigned int size, unsigned int offset) const;
	void bind(unsigned int bindingPoint) const;
private:
	unsigned int UBO;
};

