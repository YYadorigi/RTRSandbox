#version 330 core
out vec4 fragColor;

in vec2 ourTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    fragColor = mix(texture(texture1, ourTexCoord), texture(texture2, ourTexCoord), 0.25);
}