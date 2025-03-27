#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

class Shader
{
public:
	Shader(const std::string& csPath);
	Shader(const std::string& vsPath, const std::string& fsPath);
	Shader(const std::string& vsPath, const std::string& gsPath, const std::string& fsPath);
	~Shader();
	Shader(const Shader& other) = delete;
	Shader& operator=(const Shader& other) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	void use() const;
	void setUniformBlock(const std::string& name, unsigned int bindingPoint) const;

	template<typename T>
	void setUniform(const std::string& name, T&& value) const;
private:
	unsigned int ID;
};

template<typename T>
void Shader::setUniform(const std::string& name, T&& value) const
{
	if constexpr (std::is_same_v<std::decay_t<T>, int>) {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), std::forward<T>(value));
	}
	else if constexpr (std::is_same_v<std::decay_t<T>, unsigned int>) {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), std::forward<T>(value));
	}
	else if constexpr (std::is_same_v<std::decay_t<T>, float>) {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), std::forward<T>(value));
	}
	else if constexpr (std::is_same_v<std::decay_t<T>, glm::vec3>) {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(std::forward<T>(value)));
	}
	else if constexpr (std::is_same_v<std::decay_t<T>, glm::vec4>) {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(std::forward<T>(value)));
	}
	else if constexpr (std::is_same_v<std::decay_t<T>, glm::mat4>) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(std::forward<T>(value)));
	}
	else {
		static_assert(sizeof(T) == 0, "Unsupported uniform type");
	}
}