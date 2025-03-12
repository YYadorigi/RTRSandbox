#version 400 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D accumTexture;
uniform sampler2D revealTexture;

const float EPSILON = 0.00001f;

bool approx(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float maxVec3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
    float revealage = texture(revealTexture, texCoords).r;

    if (approx(revealage, 1.0f)) {
        discard;
    }

    vec4 accumulation = texture(accumTexture, texCoords);

    if (isinf(maxVec3(abs(accumulation.rgb)))) {
        accumulation.rgb = vec3(accumulation.a);
    }

    vec3 averageColor = accumulation.rgb / max(accumulation.a, EPSILON);

    FragColor = vec4(averageColor, 1.0f - revealage);
}