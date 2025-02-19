#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

class Shader
{
public:
	Shader(const char* vsPath, const char* fsPath);
	~Shader();
	Shader(const Shader& other);
	Shader& operator=(const Shader& other);
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
private:
	unsigned int ID;	// program ID
};