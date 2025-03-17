#version 430 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform VPMatrices 
{
    mat4 view;
    mat4 projection;
};

out vec3 cubeCoords;

void main()
{
    cubeCoords = aPos;
    vec4 position = projection * mat4(mat3(view)) * vec4(aPos, 1.0);    // Remove translation
    gl_Position = position.xyww;
}