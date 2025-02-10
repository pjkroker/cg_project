#Shader Vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 textureScale;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal); // Ensure normals are normalized

    gl_Position = projection * view * vec4(FragPos, 1.0);
    TexCoords = textureScale * vec2(1 - aTexCoord.x, aTexCoord.y);
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

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_uniform;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal); // Ensure normal is normalized
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    // Directional light
    result += CalcDirLight(dirLight, norm, viewDir);

    // Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

    vec4 texColor = texture(texture_uniform, TexCoords);
    FragColor = vec4(result * texColor.rgb, texColor.a); // Apply texture color to lighting result
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * texture(texture_uniform, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_uniform, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular * texture(texture_uniform, TexCoords).rgb;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * texture(texture_uniform, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_uniform, TexCoords).rgb;
    vec3 specular = light.specular * spec * material.specular * texture(texture_uniform, TexCoords).rgb;

    return (ambient + diffuse + specular) * attenuation;
}
