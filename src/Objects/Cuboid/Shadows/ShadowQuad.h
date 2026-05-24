//
// Created by Marian on 24.05.2026.
//

#ifndef BLOXORZ_SPG_SHADOWQUAD_H
#define BLOXORZ_SPG_SHADOWQUAD_H

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Clasa folosita pentru a desena umbra cuboidului pe tile-uri.\n
 * Concept teoretic:\n
 * - Considerăm lumina ca fiind o sursă direcțională (de exemplu, din poziția lightPos).\n
 * - Calculăm intersecția dintre raza de lumină care trece prin colțurile de jos ale cuboidului
 *  și planul solului (Y = 0.05f).\n
 * - Umbra va fi un poligon (patrulater) proiectat.\n
 * - În practică, pentru un obiect de tip cuboid, putem aproxima:
 * umbra este un paralelipiped proiectat pe sol, cu aceeași formă ca baza cuboidului,
 * dar translatat orizontal în funcție de direcția luminii.
 */
class ShadowQuad {
public:
    /**
     * @brief Constructor.
     */
    ShadowQuad();

    /**
     * @brief Destructor – eliberează resursele OpenGL.
     */
    ~ShadowQuad();

    /**
     * @brief Inițializează geometria (VAO, VBO, EBO).
     * @note Trebuie apelată după ce contextul OpenGL este activ.
     */
    void init();

    /**
     * @brief Desenează umbra la poziția și scara specificate.
     * @param position  Poziția centrului bazei umbrei (pe sol, Y de obicei 0.05f)
     * @param scale     Dimensiunile umbrei (lățime pe X, 1, adâncime pe Z)
     * @param vpMatrix  Matricea View-Projection curentă (pentru transformarea în clip space)
     */
    void render(const glm::vec3& position, const glm::vec3& scale,
                const glm::mat4& vpMatrix);

private:
    GLuint vao;          ///< Vertex Array Object
    GLuint vbo;          ///< Vertex Buffer Object
    GLuint ebo;          ///< Element Buffer Object
    GLsizei indexCount;  ///< Numărul de indecși (6 pentru două triunghiuri)
    GLuint shaderProgram; ///< ID-ul programului shader (dacă folosim unul dedicat pentru umbră)
};



#endif //BLOXORZ_SPG_SHADOWQUAD_H
