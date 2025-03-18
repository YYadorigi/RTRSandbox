#version 430 core
layout(early_fragment_tests) in;

#define MAX_POINT_LIGHTS 4

struct Material
{
    sampler2D ambient1;
    sampler2D ambient2;
    sampler2D diffuse1;
    sampler2D diffuse2;
    sampler2D diffuse3;
    sampler2D specular1;
    sampler2D specular2;
    sampler2D specular3;
    float shininess;
    float opacity;
};

struct DirLight 
{
    vec3 direction;
	
    vec3 color;
    float intensity;
};  // 32 bytes under std140

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
};  // 64 bytes under std140

struct AmbientLight 
{
    vec3 color;
    float intensity;
};  // 16 bytes under std140

centroid in vec3 fragPos;
centroid in vec3 normal;
centroid in vec2 texCoords;

uniform vec3 viewPos;
uniform Material material;
layout (std140) uniform Lights
{
    DirLight dirLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLight;
    AmbientLight ambientLight;
};

out vec4 FragColor;

// Function prototypes
vec3 shadingDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 shadingPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 shadingSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(viewPos - fragPos);
    
    vec3 result = vec3(0.0);

    result += shadingDirLight(dirLight, normal, viewDir) * 10.0;
    
    for(int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        result += shadingPointLight(pointLights[i], normal, fragPos, viewDir);  
    }
    
    result += shadingSpotLight(spotLight, normal, fragPos, viewDir);  
    
    result += texture(material.ambient1, texCoords).rgb * ambientLight.intensity * ambientLight.color;

    result = clamp(result, 0.0, 1.0);

    FragColor = vec4(result, material.opacity);
}

// Function definitions
vec3 shadingDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 halfDir = normalize(light.direction + viewDir);

    vec3 diffuse = texture(material.diffuse1, texCoords).rgb * max(dot(normal, light.direction), 0.0);
    vec3 specular = texture(material.specular1, texCoords).rgb * pow(max(dot(normal, halfDir), 0.0), material.shininess);
    return (diffuse + specular) * light.color * light.intensity;
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