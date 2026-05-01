#version 400

in vec3 fragPos;
in vec3 normal;
in vec3 color;

// Pozitia tile-ului in grila - pentru a calcula UV din pozitia world
uniform float tileGridX;
uniform float tileGridZ;

// Textura aplicata pe fata de sus
uniform sampler2D buttonTex;

out vec4 frag_colour;

void main() {
    float ambientStrength = 0.3;

    vec3 norm     = normalize(normal);
    vec3 baseColor;

    // Daca suntem pe fata de sus (normal.y aproape de 1.0): aplicam textura
    // Altfel: folosim culoarea din vertex (comportament identic cu tile.frag)
    if (norm.y > 0.9) {
        // Derivam UV din pozitia world:
        // Centrul tile-ului e la (gridX, 0, gridZ), dimensiunea e 1x1
        // deci coltul stanga-jos e la (gridX-0.5, gridZ-0.5)
        vec2 uv = vec2(
        fragPos.x - (tileGridX - 0.5),
        1.0 - (fragPos.z - (tileGridZ - 0.5))
        );
        // Clamp pentru siguranta
        uv = clamp(uv, 0.0, 1.0);
        baseColor = texture(buttonTex, uv).rgb;
    } else {
        baseColor = color;
    }

    vec3 ambient = ambientStrength * baseColor;
    vec3 diffuse = vec3(0.0); // fara lumina directionala pe lateral - simplu

    frag_colour = vec4(ambient + baseColor * (1.0 - ambientStrength), 1.0);
}
