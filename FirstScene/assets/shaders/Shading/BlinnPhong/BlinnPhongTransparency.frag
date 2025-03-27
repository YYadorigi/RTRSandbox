#version 430 core
layout(early_fragment_tests) in;
#define MAX_LIGHTS_PER_TYPE 4

struct Material
{
    sampler2D ambient1;
    sampler2D ambient2;
    sampler2D ambient3;
    sampler2D diffuse1;
    sampler2D diffuse2;
    sampler2D diffuse3;
    sampler2D specular1;
    sampler2D specular2;
    sampler2D specular3;
    float shininess;
    float opacity;
};

struct PointLight 
{
    vec3 position;
	
    vec3 color;
    float intensity;
};  // 32 bytes under std140

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    
    vec3 color;
    float intensity;

    float cutoff;
    float outerCutoff;  
    
    mat4 lightView;
    mat4 lightProj;
};  // 192 bytes under std140

centroid in vec3 fragPos;
centroid in vec3 normal;
centroid in vec2 texCoords;

uniform Material material;
layout (std140) uniform Lights
{
    PointLight pointLights[MAX_LIGHTS_PER_TYPE];
    SpotLight spotLights[MAX_LIGHTS_PER_TYPE];
};
uniform sampler2D spotShadowMaps[MAX_LIGHTS_PER_TYPE];
uniform vec3 cameraPosition;
uniform float offset = 1.0 / 300.0;

out vec4 FragColor;

layout (location = 0) out vec4 accumulation;
layout (location = 1) out float revealage;

vec3 shadingPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 shadingSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float calculateSpotShadow(SpotLight light, sampler2D shadowMap, vec3 fragPos, float bias);

void main()
{
    vec3 viewDir = normalize(cameraPosition - fragPos);
    
    vec3 result = vec3(0.0);
    
    for(int i = 0; i < MAX_LIGHTS_PER_TYPE; i++)
    {
        result += shadingPointLight(pointLights[i], normal, fragPos, viewDir);
    }

    for(int i = 0; i < MAX_LIGHTS_PER_TYPE; i++)
    {
        vec3 lightDir = normalize(spotLights[i].position - fragPos);
        float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
        float shadowed = calculateSpotShadow(spotLights[i], spotShadowMaps[i], fragPos, bias);
        result += (1.0 - shadowed) * shadingSpotLight(spotLights[i], normal, fragPos, viewDir);
    }
    
    vec4 color = vec4(result, material.opacity);

    float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

    accumulation = vec4(color.rgb * color.a, color.a) * weight;

    revealage = color.a;
}

vec3 shadingPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float dist = length(light.position - fragPos);

    vec3 diffuse = texture(material.diffuse1, texCoords).rgb * max(dot(normal, lightDir), 0.0);
    vec3 specular = texture(material.specular1, texCoords).rgb * pow(max(dot(normal, halfDir), 0.0), material.shininess);
    return (diffuse + specular) * pow(inversesqrt(1.0 + dist * dist), 2) * light.color * light.intensity;
}

vec3 shadingSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float dist = length(light.position - fragPos);

    vec3 diffuse = texture(material.diffuse1, texCoords).rgb * max(dot(normal, lightDir), 0.0);
    vec3 specular = texture(material.specular1, texCoords).rgb * pow(max(dot(normal, halfDir), 0.0), material.shininess);
    vec3 original = (diffuse + specular) * pow(inversesqrt(1.0 + dist * dist), 2) * light.color * light.intensity;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    return original * intensity;
}

float calculateSpotShadow(SpotLight light, sampler2D shadowMap, vec3 fragPos, float bias)
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),      // top-left
        vec2(0.0f, offset),         // top-center
        vec2(offset, offset),       // top-right
        vec2(-offset, 0.0f),        // center-left
        vec2(0.0f, 0.0f),           // center-center
        vec2(offset, 0.0f),         // center-right
        vec2(-offset, -offset),     // bottom-left
        vec2(0.0f, -offset),        // bottom-center
        vec2(offset, -offset)       // bottom-right   
    );

    vec4 fragPosLightSpace = light.lightProj * light.lightView * vec4(fragPos, 1.0);
    vec3 NDCLightSpace = fragPosLightSpace.xyz / fragPosLightSpace.w;
    vec3 shadowMapCoords = NDCLightSpace * 0.5 + 0.5;

    if (shadowMapCoords.z > 1.0) {
        return 0.0;
    }

    float shadowed = 0.0;
    for (int i = 0; i < 9; i++)
    {
        float closestDepth = texture(shadowMap, shadowMapCoords.xy + offsets[i]).r;
        float currentDepth = shadowMapCoords.z;
        shadowed += currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    return shadowed / 9.0;
}