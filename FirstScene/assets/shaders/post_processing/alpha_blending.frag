#version 400 core
out vec4 FragColor;

uniform sampler2DMS accumTexture;
uniform sampler2DMS revealTexture;

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
    float revealage = texelFetch(revealTexture, ivec2(gl_FragCoord.xy), gl_SampleID).r;

    if (approx(revealage, 1.0f)) {
        discard;
    }

    vec4 accumulation = texelFetch(accumTexture, ivec2(gl_FragCoord.xy), gl_SampleID);

    if (isinf(maxVec3(abs(accumulation.rgb)))) {
        accumulation.rgb = vec3(accumulation.a);
    }

    vec3 averageColor = accumulation.rgb / max(accumulation.a, EPSILON);

    FragColor = vec4(averageColor, 1.0f - revealage);
}