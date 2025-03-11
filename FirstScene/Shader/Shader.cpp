#include "Shader.h"

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
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
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
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
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
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete shaders after linking
	glDeleteShader(vs);
	glDeleteShader(fs);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

Shader::Shader(Shader&& other) noexcept
{
	ID = other.ID;
	other.ID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other) {
		glDeleteProgram(ID);
		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}
