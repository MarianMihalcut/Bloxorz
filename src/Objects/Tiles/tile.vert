#version 400

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_color;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;

out vec3 fragPos;
out vec3 normal;
out vec3 color;
out vec3 localPos;

void main() {
    localPos = v_pos;

    // Calculăm poziția fragmentului în coordonate universale
    fragPos = vec3(modelMatrix * vec4(v_pos, 1.0));

    // Transformăm normala pentru a ține cont de rotația obiectului
    normal = mat3(transpose(inverse(modelMatrix))) * v_normal;

    color = v_color;
    gl_Position = mvpMatrix * vec4(v_pos, 1.0);
}