#version 430 core
in vec2 texCoords;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
	FragColor = texture(screenTexture, texCoords);
}