#version 400 core
out vec4 FragColor;

uniform sampler2DMS screenTexture;
uniform float offset = 1.0 / 300.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top-left
        vec2(0.0f, offset),     // top-center
        vec2(offset, offset),   // top-right
        vec2(-offset, 0.0f),    // center-left
        vec2(0.0f, 0.0f),       // center-center
        vec2(offset, 0.0f),     // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f, -offset),    // bottom-center
        vec2(offset, -offset)   // bottom-right    
    );

    float kernel[9] = float[](
        1.0, 1.0, 1.0,
        1.0, -7.0, 1.0,
        1.0, 1.0, 1.0
    );

    vec3 sampleTex[9];

    for (int i = 0; i < 9; i++)
    {
        ivec2 coord = ivec2(gl_FragCoord.xy) + ivec2(offsets[i] * ivec2(1.0));
        sampleTex[i] = texelFetch(screenTexture, coord, gl_SampleID).rgb;
    }

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++)
    {
        col += sampleTex[i] * kernel[i];
    }

    FragColor = vec4(col, 1.0);
}