#pragma once

#include <glm/glm.hpp>
#include <vector>

/**
 * ============================================================================
 * SIMPLE PERLIN NOISE IMPLEMENTATION
 * ============================================================================
 * 
 * Genera ruido Perlin 2D para terreno procedural realista.
 * Uso: noise.sample(x, z) devuelve valor 0-1
 */
class PerlinNoise {
private:
    std::vector<int> p;
    
    static float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    
    static float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }
    
    static float grad(int hash, float x, float z) {
        int h = hash & 15;
        float u = h < 8 ? x : z;
        float v = h < 8 ? z : x;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    PerlinNoise(int seed = 12345) {
        p.resize(512);
        std::vector<int> perm(256);
        for (int i = 0; i < 256; ++i) perm[i] = i;
        
        // Fisher-Yates shuffle with custom seed
        for (int i = 255; i > 0; --i) {
            seed = (seed * 1103515245 + 12345) & 0x7fffffff;
            int j = seed % (i + 1);
            std::swap(perm[i], perm[j]);
        }
        
        for (int i = 0; i < 256; ++i) {
            p[i] = perm[i];
            p[i + 256] = perm[i];
        }
    }
    
    float sample(float x, float z, float scale = 1.0f, float persistance = 0.5f, int octaves = 4) {
        x *= scale;
        z *= scale;
        
        float result = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float maxValue = 0.0f;
        
        for (int i = 0; i < octaves; ++i) {
            int xi = (int)glm::floor(x * frequency) & 255;
            int zi = (int)glm::floor(z * frequency) & 255;
            
            float xf = glm::fract(x * frequency);
            float zf = glm::fract(z * frequency);
            
            float u = fade(xf);
            float v = fade(zf);
            
            int p00 = p[p[xi] + zi];
            int p10 = p[p[xi + 1] + zi];
            int p01 = p[p[xi] + zi + 1];
            int p11 = p[p[xi + 1] + zi + 1];
            
            float g00 = grad(p00, xf, zf);
            float g10 = grad(p10, xf - 1, zf);
            float g01 = grad(p01, xf, zf - 1);
            float g11 = grad(p11, xf - 1, zf - 1);
            
            float x0 = lerp(u, g00, g10);
            float x1 = lerp(u, g01, g11);
            float z0 = lerp(v, x0, x1);
            
            result += z0 * amplitude;
            maxValue += amplitude;
            
            amplitude *= persistance;
            frequency *= 2.0f;
        }
        
        return (result / maxValue + 1.0f) * 0.5f;
    }
};

/**
 * ============================================================================
 * PLANT GEOMETRY GENERATOR
 * ============================================================================
 * 
 * Genera geometría procedural para plantas realistas
 */
class PlantGeometry {
public:
    struct PlantMesh {
        std::vector<float> vertices;  // posición + normal
        std::vector<unsigned int> indices;
        glm::vec3 color;
    };
    
    // Generar geometría de hierba (cilindro simple)
    static PlantMesh GenerateGrass(float height = 1.0f) {
        PlantMesh mesh;
        mesh.color = glm::vec3(0.3f, 0.8f, 0.2f);  // Verde claro
        
        const int segments = 4;
        float radius = 0.15f;
        
        for (int i = 0; i <= segments; ++i) {
            float angle = (i / (float)segments) * 6.283185307179586f; // 2*pi
            float x = cos(angle) * radius;
            float z = sin(angle) * radius;
            
            // Base
            mesh.vertices.push_back(x);
            mesh.vertices.push_back(0.0f);
            mesh.vertices.push_back(z);
            mesh.vertices.push_back(0.0f);
            mesh.vertices.push_back(-1.0f);
            mesh.vertices.push_back(0.0f);
            
            // Tope
            mesh.vertices.push_back(x * 0.7f);
            mesh.vertices.push_back(height);
            mesh.vertices.push_back(z * 0.7f);
            mesh.vertices.push_back(sin(angle));
            mesh.vertices.push_back(0.3f);
            mesh.vertices.push_back(cos(angle));
        }
        
        for (int i = 0; i < segments; ++i) {
            mesh.indices.push_back(i * 2);
            mesh.indices.push_back(i * 2 + 1);
            mesh.indices.push_back((i + 1) * 2);
            mesh.indices.push_back((i + 1) * 2);
            mesh.indices.push_back(i * 2 + 1);
            mesh.indices.push_back((i + 1) * 2 + 1);
        }
        
        return mesh;
    }
    
    // Generar geometría de arbusto (esfera subdivida)
    static PlantMesh GenerateBush(float radius = 0.8f) {
        PlantMesh mesh;
        mesh.color = glm::vec3(0.2f, 0.7f, 0.1f);  // Verde medio
        
        const int rings = 6;
        const int segs = 12;
        
        // Generar esfera
        for (int ring = 0; ring <= rings; ++ring) {
            float phi = (ring / (float)rings) * 3.141592653589793f; // pi
            
            for (int seg = 0; seg <= segs; ++seg) {
                float theta = (seg / (float)segs) * 6.283185307179586f; // 2*pi
                
                float x = radius * sin(phi) * cos(theta);
                float y = radius * cos(phi) + radius;
                float z = radius * sin(phi) * sin(theta);
                
                mesh.vertices.push_back(x);
                mesh.vertices.push_back(y);
                mesh.vertices.push_back(z);
                mesh.vertices.push_back(sin(phi) * cos(theta));
                mesh.vertices.push_back(cos(phi));
                mesh.vertices.push_back(sin(phi) * sin(theta));
            }
        }
        
        // Índices para esfera
        for (int ring = 0; ring < rings; ++ring) {
            for (int seg = 0; seg < segs; ++seg) {
                int a = ring * (segs + 1) + seg;
                int b = a + 1;
                int c = a + (segs + 1);
                int d = c + 1;
                
                mesh.indices.push_back(a);
                mesh.indices.push_back(c);
                mesh.indices.push_back(b);
                mesh.indices.push_back(b);
                mesh.indices.push_back(c);
                mesh.indices.push_back(d);
            }
        }
        
        return mesh;
    }
    
    // Generar geometría de árbol (cono + cilindro)
    static PlantMesh GenerateTree(float height = 3.0f) {
        PlantMesh mesh;
        mesh.color = glm::vec3(0.1f, 0.5f, 0.05f);  // Verde oscuro
        
        // Tronco (cilindro)
        const int trunkSegs = 6;
        float trunkRadius = 0.3f;
        float trunkHeight = height * 0.3f;
        
        for (int i = 0; i <= trunkSegs; ++i) {
            float angle = (i / (float)trunkSegs) * 6.283185307179586f; // 2*pi
            float x = cos(angle) * trunkRadius;
            float z = sin(angle) * trunkRadius;
            
            // Base del tronco
            mesh.vertices.push_back(x);
            mesh.vertices.push_back(0.0f);
            mesh.vertices.push_back(z);
            mesh.vertices.push_back(cos(angle));
            mesh.vertices.push_back(0.0f);
            mesh.vertices.push_back(sin(angle));
            
            // Tope del tronco
            mesh.vertices.push_back(x * 0.8f);
            mesh.vertices.push_back(trunkHeight);
            mesh.vertices.push_back(z * 0.8f);
            mesh.vertices.push_back(cos(angle));
            mesh.vertices.push_back(0.0f);
            mesh.vertices.push_back(sin(angle));
        }
        
        // Copa (cono)
        const int conicSegs = 8;
        float conicRadius = 1.2f;
        float conicHeight = height * 0.7f;
        
        for (int i = 0; i <= conicSegs; ++i) {
            float angle = (i / (float)conicSegs) * 6.283185307179586f; // 2*pi
            float x = cos(angle) * conicRadius;
            float z = sin(angle) * conicRadius;
            
            mesh.vertices.push_back(x);
            mesh.vertices.push_back(trunkHeight);
            mesh.vertices.push_back(z);
            mesh.vertices.push_back(cos(angle));
            mesh.vertices.push_back(-0.5f);
            mesh.vertices.push_back(sin(angle));
        }
        
        // Ápice
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(height);
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(0.0f);
        mesh.vertices.push_back(1.0f);
        mesh.vertices.push_back(0.0f);
        
        // Índices
        for (int i = 0; i < trunkSegs; ++i) {
            int a = i * 2;
            int b = a + 1;
            int c = (i + 1) * 2;
            int d = c + 1;
            
            mesh.indices.push_back(a);
            mesh.indices.push_back(c);
            mesh.indices.push_back(b);
            mesh.indices.push_back(b);
            mesh.indices.push_back(c);
            mesh.indices.push_back(d);
        }
        
        // Cono
        int baseIdx = (trunkSegs + 1) * 2;
        int apexIdx = baseIdx + conicSegs + 1;
        
        for (int i = 0; i < conicSegs; ++i) {
            mesh.indices.push_back(baseIdx + i);
            mesh.indices.push_back(apexIdx);
            mesh.indices.push_back(baseIdx + i + 1);
        }
        
        return mesh;
    }
};
