#version 330 core

in vec3 fragNormal;
in vec3 fragColor;
in vec3 fragPos;
in vec3 lightDir;
in vec3 viewDir;
in vec2 texCoord_out;                     // Coordonatele UV pentru textură
in vec4 fragPosLightSpace;                // Poziția fragmentului în spațiul luminii pentru calculul umbrei

out vec4 outColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D textureSampler;         // textura aplicată pe cuboid
uniform sampler2D shadowMap;              // textura de adâncime generată de camera luminii pentru calculul umbrei

// Funcție pentru calculul umbrei folosind shadow mapping (PCF - Percentage Closer Filtering)
float calculateShadow(vec4 fragPosLight) {
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;  // Transform to [0,1] range

    if(projCoords.z > 1.0)
    return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - 0.005 > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main(){
    // Sample texture
    vec3 texColor = texture(textureSampler, texCoord_out).rgb;

    // Ambient lighting - o lumină de bază pentru a vedea fețele umbrite
    vec3 ambient = 0.6 * texColor;

    // Diffuse lighting - bazată pe produsul scalar (dot product) între normală și direcția luminii
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(lightDir);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = 1.2 * diff * texColor;

    // Specular lighting - bazată pe modelul Phong pentru reflexii
    vec3 viewDirection = normalize(viewDir);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 50.0);
    vec3 specular = 0.8 * spec * vec3(1.0, 1.0, 1.0);

    // Calculare shadow
    float shadow = calculateShadow(fragPosLightSpace);

    // Combinație finală: lumina ambientală + (1 - shadow) * (lumina difuză + lumina speculară)
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);

    outColor = vec4(result, 1.0);
}