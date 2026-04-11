#version 330 core

in vec3 fragNormal;
in vec3 fragColor;
in vec3 fragPos;
in vec3 lightDir;
in vec3 viewDir;

out vec4 outColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){
    //Lumina ambientala
    vec3 ambient = 0.2 * fragColor;

    //Lumina de difuzie
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(lightDir);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diff * fragColor;

    //Lumina speculara
    vec3 viewDirection = normalize(viewDir);
    vec3 reflectDir = reflect(-lightDirection,normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32.0); //de revizuit
    vec3 specular = spec * vec3(1.0,1.0,1.0);

    //combinam rezultatele
    vec3 result = ambient + diffuse + specular;

    //adaugare umbra simpla(in partea de jos)
    if(fragPos.y < -0.4){
        result *= 0.5;
    }

    outColor = vec4(result, 1.0);
}