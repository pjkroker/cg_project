#Shader Vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location =1) in vec3 anormal;

    const int samples=9;
    const int wavesAnzahl=4;
// Pseudo-random value in half-open range [0:1].
struct WaveData {
    vec3 position;
    vec3 normal;
};

WaveData gerstnerWave(vec3 pos, vec2 dir, float amplitude, float wavelength, float speed, float time);
WaveData computeWaves(vec3 pos, float time) ;
WaveData circularWave(vec3 pos, vec2 center, float amplitude, float wavelength, float speed, float time, float decay) ;

#define NR_POINT_LIGHTS 4

uniform vec3 objPos[NR_POINT_LIGHTS];


uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPos; 
uniform mat4 projection;
uniform float t;

out vec3 Normal;
out vec3 FragPos;

int mode=2;

void main()
{

    if (mode==0){
        FragPos = computeWaves(aPos,t).position;
        Normal = computeWaves(aPos,t).normal;
        gl_Position = projection * view * model * vec4(computeWaves(aPos,t).position,1.0);

    }
    else if(mode==1){
        vec3 displacedPosition = aPos;
        vec3 totalWavePos,totalWaveNorm;
        vec3 normal;
        float amplitude =0.29;
        float wavelength =6;
        float speed = 3;
        float decay=0.09;

        for (int i = 0; i < 1; i++) {
            //amplitude+=i*0.5;
            vec2 waveCenter = vec2(objPos[i].xz);  // Center of the circular wave
            WaveData waveEffect = circularWave(displacedPosition, waveCenter, amplitude, wavelength, speed, t, decay);
                // Apply displacement (only height should accumulate, not full position)
            displacedPosition.y += waveEffect.position.y;
            totalWaveNorm += waveEffect.normal;
        }
        gl_Position = projection * view * model * vec4(displacedPosition, 1.0);
        FragPos = displacedPosition;
        Normal = normalize(totalWaveNorm);        
    }
    else if(mode==2){
        vec2 waveCenter = vec2(objPos[0].xz);  // Center of the circular wave
        float amplitude = 0.5;
        float wavelength = 5.0;
        float speed = 1.0;


        WaveData waveEffect = circularWave(aPos, waveCenter, amplitude, wavelength, speed, t,9.0f);
        vec3 displacedPosition = waveEffect.position;

        FragPos = computeWaves(displacedPosition,t).position;
        Normal = computeWaves(displacedPosition,t).normal;
        gl_Position = projection * view * model * vec4(computeWaves(FragPos,t).position,1.0);
        //Normal = normal;
    }
    else{
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = anormal;
    }
 
   
}

// Gerstner Wave Function
WaveData gerstnerWave(vec3 pos, vec2 dir, float amplitude, float wavelength, float speed, float time) {
    float k = 2.0 * 3.14159 / wavelength;  
    float w = 2/wavelength;  
    float wavePhase = k * (dot(dir, pos.xz) - speed * time);

    float cosWave = cos(wavePhase);
    float sinWave = sin(wavePhase);

    vec3 newPos = pos;
    newPos.x += amplitude * dir.x * cosWave;
    newPos.z += amplitude * dir.y * cosWave;
    newPos.y += amplitude * sinWave;

    vec3 tangentX = vec3(1.0 - amplitude * dir.x * dir.x * sinWave, amplitude * dir.x * cosWave, -amplitude * dir.x * dir.y * sinWave);
    vec3 tangentZ = vec3(-amplitude * dir.x * dir.y * sinWave, amplitude * dir.y * cosWave, 1.0 - amplitude * dir.y * dir.y * sinWave);

    vec3 normal = normalize(cross(tangentZ, tangentX));

    return WaveData(newPos, normal);
}
WaveData circularWave(vec3 pos, vec2 center, float amplitude, float wavelength, float speed, float time, float decay) {
    //time-=5.5;
    float k = 2.0 * 3.14159 / wavelength;  // Wave number
    float w = speed * k;  // Angular frequency
    float distance = length(pos.xz - center);  // Radial distance
    float wavePhase = k * distance - w * time;  

    // Apply attenuation (decay factor)
    //float attenuation = exp(-decay * distance);  // Exponential falloff
    float waveHeight = amplitude * sin(wavePhase); //* attenuation;

    vec3 newPos = vec3(pos.x, pos.y + waveHeight, pos.z);

    // Compute normal (derivative in radial direction)
    float waveSlope = amplitude * k * cos(wavePhase); //* attenuation;  // Derivative of sine wave
    vec3 normal = normalize(vec3((pos.x - center.x) * waveSlope, 1.0, (pos.z - center.y) * waveSlope));

    return WaveData(newPos, normal);
}
// Apply multiple waves
WaveData computeWaves(vec3 pos, float time) {
    vec3 finalPos = pos;
    vec3 finalNormal = vec3(0.0);


    vec2 directions[samples] = vec2[](  
        vec2(1.0, 0.0), vec2(0.7, 0.7), vec2(0.0, 1.0), vec2(-0.7, 0.7),
        vec2(-1.0, 1.0), vec2(-0.7, -0.7), vec2(0.0, -1.0), vec2(0.7, -0.3),
         vec2(0.2, -0.3)
    );

    float amplitudes[samples] = float[](0.08, 0.06, 0.05, 0.06, 0.04, 0.025, 0.02, 0.15,0.023);
    float wavelengths[samples] = float[](6.0, 5.0, 4, 2, 6, 5, 4, 3,2);
    float speeds[samples] = float[](1.5, 1.2, 2,0.3, 1.0, 1.2, 1.1, 1.3,1.7);


    for (int i = 0; i < wavesAnzahl; i++) {
        WaveData wave = gerstnerWave(finalPos, directions[i], amplitudes[i], wavelengths[i], speeds[i], time);
        finalPos = wave.position;
        finalNormal += wave.normal*amplitudes[i];
    }

    return WaveData(finalPos, normalize(finalNormal));
}


#Shader Fragment
#version 330 core
out vec4 FragColor;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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

in vec3 FragPos;
in vec3 Normal;
uniform vec3 objectColor;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 lightColor;
// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result;
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
     result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
 
    float fresnel = pow(1.0 - dot(normalize(vec3(0.0, 1.0, 0.0)), normalize(viewDir)), 3.0);
    result.rgb += vec3(0.2, 0.3, 0.4) * fresnel;

    FragColor = vec4(result*objectColor, 0.8);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec *vec3(material.specular);
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(material.diffuse);
    vec3 specular = light.specular * spec * vec3(material.specular);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


	    