#version 400 core
in vec3 cubeCoords;

out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, cubeCoords);
}