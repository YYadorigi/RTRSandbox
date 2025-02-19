#include "shader.h"

Shader::Shader(const char* vsPath, const char* fsPath)
{
	std::string vsCode;
	std::string fsCode;
	std::ifstream vsFile;
	std::ifstream fsFile;

	// Ensure ifstream objects can throw exceptions
	vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// Open shader files
		vsFile.open(vsPath, std::ifstream::in);
		fsFile.open(fsPath, std::ifstream::in);

		// Read files
		std::stringstream vsStream, fsStream;
		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();

		// Close files
		vsFile.close();
		fsFile.close();

		// Convert stream to string
		vsCode = vsStream.str();
		fsCode = fsStream.str();
	} catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	// Convert string to C string
	const char* vsCodeCStr = vsCode.c_str();
	const char* fsCodeCStr = fsCode.c_str();

	unsigned int vs, fs;
	int success;
	char infoLog[512];

	// Compile vertex shader and fragment shader
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsCodeCStr, NULL);
	glCompileShader(vs);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsCodeCStr, NULL);
	glCompileShader(fs);

	// Check for shader compilation errors
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// Link shaders
	ID = glCreateProgram();
	glAttachShader(ID, vs);
	glAttachShader(ID, fs);
	glLinkProgram(ID);

	// Check for shader program linking errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete shaders after linking
	glDeleteShader(vs);
	glDeleteShader(fs);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

Shader::Shader(const Shader& other)
{
	ID = other.ID;
}

Shader& Shader::operator=(const Shader& other)
{
	this->ID = other.ID;
	return *this;
}

Shader::Shader(Shader&& other) noexcept
{
	ID = other.ID;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	this->ID = other.ID;
	return *this;
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
