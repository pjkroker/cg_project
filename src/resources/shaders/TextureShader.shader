#Shader Vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
uniform mat4 model;
uniform mat4 view;
uniform vec2 texureScale;

uniform mat4 projection;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
	    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
	TexCoords = texureScale*vec2(1-aTexCoord.x,aTexCoord.y);
   //TexCoord = vec2(aTexCoord.x, aTexCoord.y);

}

#Shader Fragment
#version 330 core
out vec4 FragColor;

// Material properties
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Directional light
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Point light
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Spot light
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

// Input values from the vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Uniforms
uniform sampler2D texture_uniform;  // Renamed texture uniform to texture_uniform
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // Normalized vectors
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    // Directional light
    result += CalcDirLight(dirLight, norm, viewDir);

    // Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    // Spotlight (optional)
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    vec4 texColor = texture(texture_uniform, TexCoords);
    FragColor = vec4(result, texColor.a);  // Result for lighting, texColor.a for alpha transparency

    //FragColor = vec4(result, 1.0);
}

// Directional light calculation
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Calculate final color
    vec3 ambient = light.ambient * texture(texture_uniform, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_uniform, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular;

    return ambient + diffuse + specular;
}

// Point light calculation
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Calculate final color
    vec3 ambient = light.ambient * texture(texture_uniform, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_uniform, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

// Spotlight calculation
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Calculate final color
    vec3 ambient = light.ambient * texture(texture_uniform, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_uniform, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}
