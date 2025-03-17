#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 model;
uniform mat4 invModel;
layout (std140) uniform VPMatrices 
{
    mat4 view;
    mat4 projection;
};

out VS_OUT 
{
    vec3 normal;
} vs_out;

void main()
{
    vs_out.normal = normalize(transpose(mat3(invModel)) * aNorm);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}