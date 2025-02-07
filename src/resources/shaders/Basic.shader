#Shader Vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location =1) in vec3 anormal;
layout (location =2) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPos; 
uniform mat4 projection;
uniform float t;

out vec3 Normal;
out vec3 FragPos;
struct WaveData {
    vec3 position;
    vec3 normal;
};

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
vec3 circularWave(vec3 pos, vec2 center, float amplitude, float wavelength, float speed, float time) {
    float distance = length(pos.xz - center);  // Compute radial distance from center
    float wave = sin(2.0 * 3.14159 * (distance / wavelength - speed * time));

    vec3 newPos = pos;
    newPos.y += amplitude * wave;  // Displacement along the Y-axis

    // Approximate Normal Calculation
    float delta = 0.1;
    float waveD1 = sin(2.0 * 3.14159 * ((distance + delta) / wavelength - speed * time));
    float waveD2 = sin(2.0 * 3.14159 * ((distance - delta) / wavelength - speed * time));

    vec3 normal = normalize(vec3(0.0, waveD1 - waveD2, delta));

    return vec3(newPos.y, normal.y, normal.z);
}
// Apply multiple waves
WaveData computeWaves(vec3 pos, float time) {
    vec3 finalPos = pos;
    vec3 finalNormal = vec3(0.0);
    const int samples=9;

    vec2 directions[samples] = vec2[](  
        vec2(1.0, 0.0), vec2(0.7, 0.7), vec2(0.0, 1.0), vec2(-0.7, 0.7),
        vec2(-1.0, 1.0), vec2(-0.7, -0.7), vec2(0.0, -1.0), vec2(0.7, -0.3),
         vec2(0.2, -0.3)
    );

    float amplitudes[samples] = float[](0.02, 0.2, 0.05, 0.1, 0.04, 0.25, 0.2, 0.15,0.43);
    float wavelengths[samples] = float[](6.0, 5.0, 4.5, 2.5, 6.3, 5.5, 4.5, 3.2,2.3);
    float speeds[samples] = float[](0.5, 1.2, 2,0.3, 1.0, 1.2, 1.1, 1.3,1.7);


    for (int i = 0; i < samples; i++) {
        WaveData wave = gerstnerWave(finalPos, directions[i], amplitudes[i], wavelengths[i], speeds[i], time);
        finalPos = wave.position;
        finalNormal += wave.normal;
    }

    return WaveData(finalPos, normalize(finalNormal));
}
int mode=0;

void main()
{
    if (mode==1){
        FragPos = computeWaves(aPos,t).position;
        Normal = computeWaves(aPos,t).normal;
        gl_Position = projection * view * model * vec4(computeWaves(aPos,t).position,1.0);

    }
    else if(mode==1){
        vec2 waveCenter = vec2(lightPos);  // Center of the circular wave
        float amplitude = 0.5;
        float wavelength = 5.0;
        float speed = 1.0;
        vec3 waveEffect = circularWave(aPos, waveCenter, amplitude, wavelength, speed, t);
        vec3 displacedPosition = vec3(aPos.x, waveEffect.x, aPos.z);
        vec3 normal = normalize(vec3(0.0, waveEffect.y, waveEffect.z));
        gl_Position = projection * view * model * vec4(displacedPosition, 1.0);
        FragPos = displacedPosition;
    }
    else{
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = anormal;
    } 
   
};


#Shader Fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

in vec3 FragPos;
in vec3 Normal;


uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {

        // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor*0.6;

        // specular
    float specularStrength = 0.9;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
            
    vec3 result = (specular+diffuse+ambient)* objectColor;
    FragColor = vec4(result, 0.5);
    FragColor.a=0.3;
}


