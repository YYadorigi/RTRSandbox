#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aInstancePos;

uniform mat4 model;
uniform mat4 invModel;
layout (std140) uniform VPMatrices 
{
    mat4 lightView;
    mat4 lightProj;
};

void main()
{
    gl_Position = lightProj * lightView * (model * vec4(aPos, 1.0) + vec4(aInstancePos, 0.0));
}