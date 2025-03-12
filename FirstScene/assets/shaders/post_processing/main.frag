#version 400 core
out vec4 FragColor;

uniform sampler2DMS screenTexture;

void main()
{
	FragColor = texelFetch(screenTexture, ivec2(gl_FragCoord.xy), gl_SampleID);
}