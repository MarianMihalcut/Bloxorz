#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 fragNormal;
out vec3 fragColor;
out vec3 fragPos;
out vec3 lightDir;
out vec3 viewDir;

void main(){
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    fragPos = worldPos.xyz; //toate coordonatele tridimensionale

    fragNormal = normalize((normalMatrix) * vec4(normal,0.0).xyz);
    fragColor = color;

    lightDir = normalize(lightPos - fragPos);
    viewDir = normalize(viewPos - fragPos);

    gl_Position = mvpMatrix * vec4(position, 1.0);
}