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

out vec3 vertexColor;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
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
 * Cambio dinámico según la proximidad de luces.
*/
inline const char* terrainFragmentShaderSource = R"(
#version 330 core
in vec3 vertexNormal;
in vec3 vertexPos;

out vec4 FragColor;

uniform float uTime;  // Tiempo transcurrido en segundos

void main()
{
    // Color base: tierra marrón oscuro
    vec3 colorSoil = vec3(0.35, 0.25, 0.15);
    
    // Color más oscuro: tierra mojada
    vec3 colorDarkSoil = vec3(0.25, 0.18, 0.1);
    
    // Calcularharia influencia de luces
    float lightInfluence = 0.0;
    
    // Patrón de grid para detectar si "hay luces"
    float gridX = mod(vertexPos.x + 2.0, 4.0);
    float gridZ = mod(vertexPos.z + 2.0, 4.0);
    
    // Si estamos en una esquina del grid, consideramos que hay luz
    if ((gridX < 1.0 || gridX > 3.0) && (gridZ < 1.0 || gridZ > 3.0)) {
        lightInfluence = 0.5;  // 50% más oscuro aquí
    } else if (sin(uTime * 0.5) > 0.0) {
        lightInfluence = 0.2 + 0.1 * sin(uTime * 0.3);
    }
    
    // Mezclar colores
    vec3 color = mix(colorSoil, colorDarkSoil, lightInfluence);
    
    // Luz direccional simple (desde arriba)
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(vertexNormal, lightDir), 0.4);  // 0.4 = tierra más oscura
    
    // Aplicar iluminación
    vec3 result = color * diff;
    
    // Patrón de cracks/textura sutil
    float cracks = mod(vertexPos.x * 0.3, 1.0) + mod(vertexPos.z * 0.3, 1.0);
    cracks = step(1.8, cracks);
    result += cracks * 0.05;
    
    FragColor = vec4(result, 1.0);
}
)";
