#version 430 core
in vec2 texCoords;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
	vec4 fragColor = texture(screenTexture, texCoords);
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2));	// gamma correction
	FragColor = fragColor;
}