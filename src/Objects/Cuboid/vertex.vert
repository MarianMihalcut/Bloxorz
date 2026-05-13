#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec2 texCoord;    // NEW: texture coordinates

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;            // NEW: light view-projection matrix
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 fragNormal;
out vec3 fragColor;
out vec3 fragPos;
out vec3 lightDir;
out vec3 viewDir;
out vec2 texCoord_out;                    // NEW: pass to fragment shader
out vec4 fragPosLightSpace;               // NEW: position in light space

void main(){
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    fragPos = worldPos.xyz;
    fragNormal = normalize(normalMatrix * normal);
    fragColor = color;

    texCoord_out = texCoord;              // NEW
    fragPosLightSpace = lightSpaceMatrix * worldPos;  // NEW

    lightDir = normalize(lightPos - fragPos);
    viewDir = normalize(viewPos - fragPos);

    gl_Position = mvpMatrix * vec4(position, 1.0);
}