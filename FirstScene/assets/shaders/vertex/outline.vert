#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 invModel;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineWidth = 0.03;

void main()
{
    normal = normalize(transpose(mat3(invModel)) * aNorm);
    vec4 displacement = vec4(normal * outlineWidth, 0.0);
    fragPos = vec3(model * vec4(aPos, 1.0) + displacement);
    texCoords = aTexCoords;
    gl_Position = projection * view * (model * vec4(aPos, 1.0) + displacement);
}