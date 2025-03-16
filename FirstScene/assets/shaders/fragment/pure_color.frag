#version 430 core
layout(early_fragment_tests) in;

uniform vec4 color;

out vec4 FragColor;

void main()
{
    FragColor = color;
}