#version 430 core
in vec2 texCoords;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
	vec4 fragColor = texture(screenTexture, texCoords);
    float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;

    fragColor = vec4(average, average, average, 1.0);
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2));	// gamma correction
	FragColor = fragColor;
}