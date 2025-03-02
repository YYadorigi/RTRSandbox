#version 330 core
struct Material 
{
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Lighting 
{
    vec3 ambientColor;
    float ambientIntensity;
    vec3 directColor;
    float directIntensity;
    vec3 pos;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

uniform Material material;
uniform Lighting lighting;
uniform vec3 viewPos;

void main()
{
    vec3 ambientLight = lighting.ambientIntensity * lighting.ambientColor;
    vec3 directLight = lighting.directIntensity * lighting.directColor;

    vec3 lightDir = normalize(lighting.pos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float dist = length(lighting.pos - fragPos);

    vec3 ambient = texture(material.ambient, texCoords).rgb * ambientLight;

    vec3 diffuse = texture(material.diffuse, texCoords).rgb * max(dot(normal, lightDir), 0.0);
    vec3 specular = texture(material.specular, texCoords).rgb * pow(max(dot(normal, halfDir), 0.0), material.shininess);
    vec3 direct = (diffuse + specular) * pow(inversesqrt(dist), 2.0) * directLight;

    gl_FragColor = vec4(ambient + direct, 1.0);
}