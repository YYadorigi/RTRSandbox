#version 400 core
layout (location = 0) in vec3 aPos;

out vec3 cubeCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    cubeCoords = aPos;
    vec4 position = projection * view * vec4(aPos, 1.0);
    gl_Position = position.xyww;
}