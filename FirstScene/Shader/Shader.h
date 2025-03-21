#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

class Shader
{
public:
	Shader(const char* csPath);
	Shader(const char* vsPath, const char* fsPath);
	Shader(const char* vsPath, const char* gsPath, const char* fsPath);
	~Shader();
	Shader(const Shader& other) = delete;
	Shader& operator=(const Shader& other) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	inline void use() const { glUseProgram(ID); }
	inline void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
	}
	inline void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	inline void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	inline void setVec3(const std::string& name, const float* value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
	}
	inline void setTransform(const std::string& name, const float* value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
	}
	inline void setUniformBlock(const std::string& name, unsigned int bindingPoint) const
	{
		glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), bindingPoint);
	}
private:
	unsigned int ID;	// program ID
};