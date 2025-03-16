#version 430 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
	vec4 fragColor = texture(screenTexture, texCoords);
    float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}