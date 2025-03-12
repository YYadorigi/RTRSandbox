#version 400 core
out vec4 FragColor;

uniform sampler2DMS screenTexture;

void main()
{
	vec4 fragColor = texelFetch(screenTexture, ivec2(gl_FragCoord.xy), gl_SampleID);
    float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}