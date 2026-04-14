#version 400

in vec3 fragPos;
in vec3 localPos;
in vec3 normal;
in vec3 color;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 frag_colour;

void main() {
    // 1. Componenta Ambientală (lumină minimă pentru a vedea fețele umbrite)
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * color;

    // 2. Componenta Difuză (bazată pe produsul scalar - dot product)
    // Aplicăm formula cos(N,L) = dot(normalize(N), normalize(L))
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Rezultatul final combină lumina ambientală cu cea difuză
    vec3 result = ambient + diffuse;

    //Implementam logica pentru muchii
    if (norm.y > 0.9) {
        // Presupunând că tile-ul are lățimea 1.0, marginile sunt la +/- 0.5
        float edgeThreshold = 0.45; // Cu cât e mai mare, cu atât e mai subțire muchia

        // Verificăm distanța față de centru pe axele X și Z
        bool isNearEdgeX = abs(localPos.x) > edgeThreshold;
        bool isNearEdgeZ = abs(localPos.z) > edgeThreshold;

        if (isNearEdgeX || isNearEdgeZ) {
            vec3 edgeColor = vec3(1.0, 1.0, 1.0); // Culoarea muchiei (ex: Alb sau un albastru deschis)
            // Putem face un mix între culoarea calculată și cea a muchiei pentru un aspect mai fin
            result = mix(result, edgeColor, 0.8);
        }
    }

    frag_colour = vec4(result, 1.0);
}