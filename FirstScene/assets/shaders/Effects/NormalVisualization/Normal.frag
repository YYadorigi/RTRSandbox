#version 430 core
uniform vec3 color = vec3(1.0, 0.5, 0.31);

out vec4 FragColor;

void main() 
{
    FragColor = vec4(pow(color, vec3(2.2)), 1.0);   // gamma correction
}