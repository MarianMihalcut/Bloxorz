#version 400

in vec3 fragPos;
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
    frag_colour = vec4(result, 1.0);
}