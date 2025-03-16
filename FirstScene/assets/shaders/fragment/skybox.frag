#version 430 core
layout(early_fragment_tests) in;

in vec3 cubeCoords;

uniform samplerCube skybox;

out vec4 FragColor;

void main()
{
	FragColor = texture(skybox, cubeCoords);
}