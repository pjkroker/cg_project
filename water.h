#pragma once
#ifndef Water_H
#define Water_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include<vector>
#include <iostream>

using namespace glm;


struct WaveData {
    vec3 position;
    vec3 normal;
};

WaveData gerstnerWave(vec3 pos, vec2 dir, float amplitude, float wavelength, float speed, float time);
WaveData computeWaves(vec3 pos, float time);
WaveData circularWave(vec3 pos, vec2 center, float amplitude, float wavelength, float speed, float time, float decay);


uniform float t;


int mode = 1;

void main()
{

    if (mode == 0) {
        FragPos = computeWaves(aPos, t).position;
        Normal = computeWaves(aPos, t).normal;
        gl_Position = projection * view * model * vec4(computeWaves(aPos, t).position, 1.0);

    }
    else if (mode == 1) {
        vec3 displacedPosition = aPos;
        vec3 totalWavePos, totalWaveNorm;
        vec3 normal;
        float amplitude = 0.002;
        float wavelength = 6;
        float speed = 3;
        float decay = 0.09;

        for (int i = 0; i < 4; i++) {
            amplitude += i * 0.1;
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
    else if (mode == 2) {
        vec2 waveCenter = vec2(lightPos.xz);  // Center of the circular wave
        float amplitude = 1.5;
        float wavelength = 5.0;
        float speed = 1.0;


        WaveData waveEffect = circularWave(aPos, waveCenter, amplitude, wavelength, speed, t, 0.8f);
        vec3 displacedPosition = vec3(aPos.x, waveEffect.position.x, aPos.z);
        vec3 normal = normalize(vec3(0.0, waveEffect.position.y, waveEffect.position.z));

        FragPos = computeWaves(displacedPosition, t).position;
        Normal = computeWaves(displacedPosition, t).normal;
        gl_Position = projection * view * model * vec4(computeWaves(FragPos, t).position, 1.0);
        //Normal = normal;
    }
    else {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = anormal;
    }


};

// Gerstner Wave Function
WaveData gerstnerWave(vec3 pos, vec2 dir, float amplitude, float wavelength, float speed, float time) {
    float k = 2.0 * 3.14159 / wavelength;
    float w = 2 / wavelength;
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
    time -= 5.5;
    float k = 2.0 * 3.14159 / wavelength;  // Wave number
    float w = speed * k;  // Angular frequency
    float distance = length(pos.xz - center);  // Radial distance
    float wavePhase = k * distance - w * time;  // Correct wave phase

    // Apply attenuation (decay factor)
    float attenuation = exp(-decay * distance);  // Exponential falloff
    float waveHeight = amplitude * sin(wavePhase) * attenuation;

    vec3 newPos = vec3(pos.x, pos.y + waveHeight, pos.z);

    // Compute normal (derivative in radial direction)
    float waveSlope = amplitude * k * cos(wavePhase) * attenuation;  // Derivative of sine wave
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

    float amplitudes[samples] = float[](0.02, 0.2, 0.05, 0.1, 0.04, 0.025, 0.2, 0.15, 0.23);
    float wavelengths[samples] = float[](6.0, 5.0, 4, 2, 6, 5, 4, 3, 2);
    float speeds[samples] = float[](0.5, 1.2, 2, 0.3, 1.0, 1.2, 1.1, 1.3, 1.7);


    for (int i = 0; i < samples; i++) {
        WaveData wave = gerstnerWave(finalPos, directions[i], amplitudes[i], wavelengths[i], speeds[i], time);
        finalPos = wave.position;
        finalNormal += wave.normal;
    }

    return WaveData(finalPos, normalize(finalNormal));
}














#endif