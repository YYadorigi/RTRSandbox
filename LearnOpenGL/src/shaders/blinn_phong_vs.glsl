#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    normal = normalize(vec3(transpose(inverse(model)) * vec4(aNorm, 0.0)));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}