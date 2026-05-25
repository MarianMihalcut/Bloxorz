#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;      // normale pentru fiecare vertex, necesare pentru iluminare
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texCoord;    // coordonatele de textură pentru fiecare vertex

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;            // matricea pentru transformarea în spațiul luminii
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 fragNormal;
out vec3 fragColor;
out vec3 fragPos;
out vec3 lightDir;
out vec3 viewDir;
out vec2 texCoord_out;                    // trimis catre fragment shader
out vec4 fragPosLightSpace;               // pozitia fragmentului in spatiul luminii, pentru umbrire

void main(){
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    fragPos = worldPos.xyz;
    fragNormal = normalize(normalMatrix * normal);
    fragColor = color;

    texCoord_out = texCoord;
    fragPosLightSpace = lightSpaceMatrix * worldPos;

    lightDir = normalize(lightPos - fragPos);
    viewDir = normalize(viewPos - fragPos);

    gl_Position = mvpMatrix * vec4(position, 1.0);
}