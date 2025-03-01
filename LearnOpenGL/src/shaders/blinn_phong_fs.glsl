#version 330 core
in vec3 fragPos;
in vec3 normal;

uniform vec3 ambientColor;
uniform float ambientIntensity;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 surfaceColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 ambient = ambientIntensity * ambientColor;

    vec3 lightDir = normalize(lightPos - fragPos);
    float dist = length(lightPos - fragPos);
    float diffuse = max(dot(normal, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);

    vec3 direct = (diffuse + spec) * lightIntensity * lightColor * pow(inversesqrt(dist), 2.0);

    gl_FragColor = vec4((ambient + direct) * surfaceColor, 1.0);
}