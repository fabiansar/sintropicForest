#pragma once

/**
 * VERTEX SHADER (Sombreador de Vértices)
 * 
 * Este shader se ejecuta para cada vértice que enviamos a OpenGL.
 * Transforma la posición 3D del punto de luz usando las matrices de proyección, vista y modelo.
 * También pasa el color a través del proceso de renderizado.
*/
inline const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aPointSize;

out vec3 vertexColor;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    gl_PointSize = aPointSize;
    vertexColor = aColor;
}
)";

/**
 * FRAGMENT SHADER (Sombreador de Fragmentos/Píxeles)
 * 
 * Este shader determina el color final de cada píxel.
 * Crea un efecto de brillo (glow) para los puntos de luz
 * usando la distancia del píxel respecto al centro del punto.
*/
inline const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    float distance = length(gl_PointCoord - vec2(0.5));
    if (distance > 0.5)
        discard;
    
    float alpha = 1.0 - (distance * 2.0);
    FragColor = vec4(vertexColor, alpha);
}
)";

/**
 * VERTEX SHADER PARA EL TERRENO
 * 
 * Renderiza el terreno (grid/plano). Es más simple que el de puntos.
*/
inline const char* terrainVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

out vec3 vertexNormal;
out vec3 vertexPos;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    vertexPos = vec3(uModel * vec4(aPosition, 1.0));
    vertexNormal = mat3(transpose(inverse(uModel))) * aNormal;
    gl_Position = uProjection * uView * vec4(vertexPos, 1.0);
}
)";

/**
 * FRAGMENT SHADER PARA EL TERRENO
 * 
 * Renderiza el terreno tipo tierra/suelo con variaciones de marrón.
 * Iluminación dinámica real basada en posiciones de plantas.
*/
inline const char* terrainFragmentShaderSource = R"(
#version 330 core
in vec3 vertexNormal;
in vec3 vertexPos;

out vec4 FragColor;

uniform float uTime;
uniform int uPlantCount;
uniform vec3 uPlantPositions[500];
uniform vec3 uPlantColors[500];

void main()
{
    // Color base: tierra marrón oscuro
    vec3 colorSoil = vec3(0.35, 0.25, 0.15);
    
    // Luz direccional simple (desde arriba)
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(vertexNormal, lightDir), 0.4);
    
    // Iluminación base
    vec3 result = colorSoil * diff;
    
    // Acumular iluminación REAL de cada planta cercana
    float totalLight = 0.0;
    vec3 lightColor = vec3(0.0);
    
    for (int i = 0; i < uPlantCount; i++) {
        vec3 toPlant = uPlantPositions[i] - vertexPos;
        float dist = length(vec2(toPlant.x, toPlant.z));
        float radius = 15.0;
        
        if (dist < radius) {
            float attenuation = 1.0 - (dist / radius);
            attenuation = attenuation * attenuation;  // Cuadrático = más realista
            float intensity = attenuation * 0.6;
            totalLight += intensity;
            lightColor += uPlantColors[i] * intensity;
        }
    }
    
    // Aplicar iluminación de plantas al terreno
    if (totalLight > 0.0) {
        lightColor /= totalLight;  // Normalizar color
        totalLight = min(totalLight, 1.5);  // Clamp para evitar sobreexposición
        result += lightColor * totalLight * 0.4;
    }
    
    // Textura sutil de tierra
    float cracks = mod(vertexPos.x * 0.3, 1.0) + mod(vertexPos.z * 0.3, 1.0);
    cracks = step(1.8, cracks);
    result += cracks * 0.03;
    
    FragColor = vec4(result, 1.0);
}
)";
