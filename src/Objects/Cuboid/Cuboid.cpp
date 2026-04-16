//
// Created by Marian on 11.04.2026.
//

#include <glew.h>
#include "Cuboid.h"
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cmath>
#include <cstdio>
#include "../ObjectModelInterface.h"

namespace ObjectModel {

    // -------------------------------------------------------------------------
    // Constructori
    // -------------------------------------------------------------------------

    /// Constructor implicit.
    /// Bloc de dimensiune 1x2x1 (in spatiu world), pozitionat deasupra tile-ului (0,0), culoare albastra.
    Cuboid::Cuboid(){
        position = glm::vec3(0.0f, 1.1f, 0.0f); // Y = TILE_H/2 + Ly = 0.1 + 1.0
        targetPosition = position;
        scale = glm::vec3(1.0f, 2.0f, 1.0f); // in picioare: lat 1 tile, inalt 2
        targetScale = scale;
        color = glm::vec3(0.2f, 0.6f, 1.0f);
        rotation = 0.0f;
        targetRotation = 0.0f;
        rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        nrFaces = 6;
        nrVerticesPerFace = 6;
        nrVertices = nrFaces * nrVerticesPerFace;

        lightPos = glm::vec3(5.0f, 10.0f, 5.0f);
        viewPos = glm::vec3(5.0f, 5.0f, 10.0f);

        projectionMatrix = glm::perspective(glm::pi<float>() / 6.0f, 1.0f, 0.1f, 100.0f);
        viewMatrix = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

        isStanding = true;
        animState = IDLE;
        animProgress = 1.0f;
        animSpeed = 2.0f;  // 2 secunde pentru o animație completă

        generateVertices();
    }

    Cuboid::Cuboid(glm::vec3 pos, glm::vec3 scl, glm::vec3 col) {
        position = pos;
        targetPosition = pos;
        scale = scl;
        targetScale = scl;
        color = col;

        rotation = 0.0f;
        targetRotation = 0.0f;
        rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

        nrFaces = 6;
        nrVerticesPerFace = 6;
        nrVertices = nrFaces * nrVerticesPerFace;

        lightPos = glm::vec3(5.0f, 10.0f, 5.0f);
        viewPos = glm::vec3(5.0f, 5.0f, 10.0f);

        projectionMatrix = glm::perspective(glm::pi<float>() / 6.0f, 1.0f, 0.1f, 100.0f);
        viewMatrix = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

        animState = IDLE;
        animProgress = 1.0f;
        animSpeed = 4.0f;

        // Determinăm dacă stă în picioare bazat pe înălțimea (Y) primită
        // Dacă înălțimea e ~1.0, înseamnă că e în picioare.
        updateOrientation();

        //generam geometria initiala
        generateVertices();
    }

    Cuboid::~Cuboid() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteProgram(shader_programme);
    }

    // -------------------------------------------------------------------------
    // Geometrie
    // -------------------------------------------------------------------------

    ///Genereaza vertecsii cubului. Per total sunt 6 fete si 6 vertex-uri pe fata = 36 de vertex-uri
    void Cuboid::generateVertices() {
        float Lx = scale.x / 2.0f; //primul elem din vec3
        float Ly = scale.y / 2.0f; //al doilea elem din vec3
        float Lz = scale.z / 2.0f; //al treilea element din vec3

        vertices.clear();

        /* Un vertex al cuboidului are:
         * - 3 coordonate pentru pozitie: x,y,z
         * - 3 coordonate pentru normala la suprafata: nx,ny,nz
         * - 3 coordonate pentru culoare: r,g,b
         */

        //Fata X pozitiv
        std::vector<float> face1 = {
            Lx, -Ly, -Lz,  1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            Lx, -Ly,  Lz,  1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            Lx,  Ly,  Lz,  1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,

            Lx, -Ly, -Lz,  1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            Lx,  Ly, -Lz,  1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            Lx,  Ly,  Lz,  1.0f, 0.0f, 0.0f,  color.r, color.g, color.b
        };
        vertices.insert(vertices.end(), face1.begin(), face1.end());

        //Fata X negativ
        std::vector<float> face2 = {
            -Lx, -Ly, -Lz,  -1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            -Lx,  Ly, -Lz,  -1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            -Lx,  Ly,  Lz,  -1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,

            -Lx, -Ly, -Lz,  -1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            -Lx, -Ly,  Lz,  -1.0f, 0.0f, 0.0f,  color.r, color.g, color.b,
            -Lx,  Ly,  Lz,  -1.0f, 0.0f, 0.0f,  color.r, color.g, color.b
        };
        vertices.insert(vertices.end(), face2.begin(), face2.end());

        //Fata Y pozitiv
        std::vector<float> face3 = {
            Lx,  Ly, -Lz,  0.0f, 1.0f, 0.0f,  color.r, color.g, color.b,
            Lx,  Ly,  Lz,  0.0f, 1.0f, 0.0f,  color.r, color.g, color.b,
           -Lx,  Ly,  Lz,  0.0f, 1.0f, 0.0f,  color.r, color.g, color.b,

            Lx,  Ly, -Lz,  0.0f, 1.0f, 0.0f,  color.r, color.g, color.b,
           -Lx,  Ly, -Lz,  0.0f, 1.0f, 0.0f,  color.r, color.g, color.b,
           -Lx,  Ly,  Lz,  0.0f, 1.0f, 0.0f,  color.r, color.g, color.b
        };
        vertices.insert(vertices.end(),face3.begin(),face3.end());

        //Fata Y negativ
        std::vector<float> face4 = {
            Lx, -Ly, -Lz,  0.0f, -1.0f, 0.0f,  color.r, color.g, color.b,
            Lx, -Ly,  Lz,  0.0f, -1.0f, 0.0f,  color.r, color.g, color.b,
           -Lx, -Ly,  Lz,  0.0f, -1.0f, 0.0f,  color.r, color.g, color.b,

            Lx, -Ly, -Lz,  0.0f, -1.0f, 0.0f,  color.r, color.g, color.b,
           -Lx, -Ly, -Lz,  0.0f, -1.0f, 0.0f,  color.r, color.g, color.b,
           -Lx, -Ly,  Lz,  0.0f, -1.0f, 0.0f,  color.r, color.g, color.b
        };
        vertices.insert(vertices.end(), face4.begin(), face4.end());

        //Fata Z pozitiv
        std::vector<float> face5 = {
            Lx, -Ly,  Lz,  0.0f, 0.0f, 1.0f,  color.r, color.g, color.b,
            Lx,  Ly,  Lz,  0.0f, 0.0f, 1.0f,  color.r, color.g, color.b,
           -Lx,  Ly,  Lz,  0.0f, 0.0f, 1.0f,  color.r, color.g, color.b,

            Lx, -Ly,  Lz,  0.0f, 0.0f, 1.0f,  color.r, color.g, color.b,
           -Lx, -Ly,  Lz,  0.0f, 0.0f, 1.0f,  color.r, color.g, color.b,
           -Lx,  Ly,  Lz,  0.0f, 0.0f, 1.0f,  color.r, color.g, color.b
        };
        vertices.insert(vertices.end(), face5.begin(), face5.end());

        //Fata Z negativ
        std::vector<float> face6 = {
            Lx, -Ly, -Lz,  0.0f, 0.0f, -1.0f,  color.r, color.g, color.b,
            Lx,  Ly, -Lz,  0.0f, 0.0f, -1.0f,  color.r, color.g, color.b,
           -Lx,  Ly, -Lz,  0.0f, 0.0f, -1.0f,  color.r, color.g, color.b,

            Lx, -Ly, -Lz,  0.0f, 0.0f, -1.0f,  color.r, color.g, color.b,
           -Lx, -Ly, -Lz,  0.0f, 0.0f, -1.0f,  color.r, color.g, color.b,
           -Lx,  Ly, -Lz,  0.0f, 0.0f, -1.0f,  color.r, color.g, color.b
        };
        vertices.insert(vertices.end(), face6.begin(), face6.end());
    }

    // -------------------------------------------------------------------------
    // Init / Display / Update
    // -------------------------------------------------------------------------

    void Cuboid::init() {

        //incarcare shadere
        std::string vstext = textFileRead("../src/Objects/Cuboid/vertex.vert");
        std::string fstext = textFileRead("../src/Objects/Cuboid/fragment.frag");
        const char* vertex_shader = vstext.c_str();
        const char* fragment_shader = fstext.c_str();

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_shader, NULL);
        glCompileShader(vs);
        printShaderInfoLog(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_shader, NULL);
        glCompileShader(fs);
        printShaderInfoLog(fs);

        shader_programme = glCreateProgram();
        glAttachShader(shader_programme, fs);
        glAttachShader(shader_programme, vs);
        glLinkProgram(shader_programme);
        printProgramInfoLog(shader_programme);

        //Creare VBO
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
            vertices.data(), GL_STATIC_DRAW);

        //Creare VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //atribut pozitie (location = 0) - in vertex shader (9 puncte per fata), offset = 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

        //atribut normala (location = 1) - in vertex shader (9 puncte per fata), offset = 3
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            9 * sizeof(float), (void*)(3 * sizeof(float)));

        //atribut culoare (location = 2) - in vertex shader (9 puncte per fata), offset = 6
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
            9*sizeof(float), (void*)(6 * sizeof(float)));
    }

    void Cuboid::display() {
        glUseProgram(shader_programme);
        glBindVertexArray(vao);

        //Matricea model
        // - se aplica o rotatie cu unghiul "rotation" si cu normala pe y
        // - se aplica o translatie ls coordonata din "position"
        glm::mat4 modelMatrix = glm::mat4(1.0f); //matricea identitate)
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        //Matricea MVP - preluare din shadere
        glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
        GLuint mvpMatrixLocation = glGetUniformLocation(shader_programme, "mvpMatrix");
        glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        //Matricea model - preluare din shadere
        GLuint modelMatrixLoc = glGetUniformLocation(shader_programme, "modelMatrix");
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        //Matricea normala
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix)); //transpusa inversei matricei model
        GLuint normalMatrixLoc = glGetUniformLocation(shader_programme, "normalMatrix");
        glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        //Pozitie lumina
        GLuint lightPosLoc = glGetUniformLocation(shader_programme, "lightPos");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        //Pozitie observator
        GLuint viewPosLoc = glGetUniformLocation(shader_programme, "viewPos");
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

        //In final desenam corpul
        glDrawArrays(GL_TRIANGLES, 0, nrVertices); //36 de vertex-uri
    }

    void Cuboid::update(float deltaTime) {
        if (animState != IDLE) {
            animProgress += deltaTime * animSpeed;

            if (animProgress >= 1.0f) {
                // Animație completă
                animProgress = 1.0f;
                position = targetPosition;
                scale = targetScale;
                rotation = targetRotation;
                animState = IDLE;

                // Actualizează geometria
                generateVertices();
                if (vbo != 0) {
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                        vertices.data(), GL_STATIC_DRAW);
                }
            } else {
                // Interpolare liniară pentru poziție și scale
                position = glm::mix(position, targetPosition, animProgress);
                scale = glm::mix(scale, targetScale, animProgress);
                rotation = glm::mix(rotation, targetRotation, animProgress);

                // Actualizează geometria pentru scale intermediar
                generateVertices();
                if (vbo != 0) {
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                        vertices.data(), GL_STATIC_DRAW);
                }
            }
            updateOrientation();
        }
    }

    bool Cuboid::isAnimating() {
        return animState != IDLE;
    }

    // -------------------------------------------------------------------------
    // Setteri / Getteri
    // -------------------------------------------------------------------------

    void Cuboid::setProjectionMatrix(glm::mat4 proj) {
        projectionMatrix = proj;
    }

    void Cuboid::setViewMatrix(glm::mat4 view) {
        viewMatrix = view;
    }

    void Cuboid::setLightPos(glm::vec3 light) {
        lightPos = light;
    }

    void Cuboid::setViewPos(glm::vec3 vPos) {
        viewPos = vPos;
    }

    void Cuboid::setPosition(glm::vec3 pos) {
        position = pos;
    }

    /// Este setata marimea obiectului.
    /// Pentru ca se modifica corpul, el trebuie redesenat
    void Cuboid::setScale(glm::vec3 scl) {
        scale = scl;
        generateVertices();

        if (vbo!=0) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                vertices.data(), GL_STATIC_DRAW);
        }
    }

    /// Este setata culoarea obiectului(in sistem RGB)
    /// Schimbarea culorii obiectului duce la redesenarea lui
    void Cuboid::setColor(glm::vec3 col) {
        color = col;
        generateVertices();
        if (vbo!=0) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                vertices.data(), GL_STATIC_DRAW);
        }
    }

    void Cuboid::setRotation(float rot) { rotation = rot; }

    glm::vec3 Cuboid::getPosition() { return position; }

    glm::vec3 Cuboid::getScale() { return scale; }

    bool Cuboid::getIsStanding() {
        updateOrientation();
        return isStanding;
    }

    // -------------------------------------------------------------------------
    // Animatie interna
    // -------------------------------------------------------------------------

    void Cuboid::startAnimation(AnimationState state, glm::vec3 newPos, glm::vec3 newScale,
                                float newRot, glm::vec3 rotAxis) {
        animState = state;
        targetPosition = newPos;
        targetScale = newScale;
        targetRotation = newRot;
        rotationAxis = rotAxis;
        animProgress = 0.0f;
    }

    // -------------------------------------------------------------------------
    /// Legenda Orientare
    /// Bloc in picioare:  scale.y > 1.5  (scale.y = 2.0)
    /// Culcat pe X:       scale.x > 1.5  (scale.x = 2.0)
    /// Culcat pe Z:       scale.z > 1.5  (scale.z = 2.0)
    // -------------------------------------------------------------------------

    void Cuboid::updateOrientation() {
        //Daca coordonata maxima e pe y, sigur este cuboidul in picioare
        isStanding = (scale.y > 1.5f);
    }

    // -------------------------------------------------------------------------
    /// Ghid Miscari
    //
    /// Dimensiuni tile: 1.0 x 0.2 x 1.0 (W x H x D)
    /// Fata superioara tile la Y = +0.1
    //
    /// Stari si Y-center corespunzator:
    ///   In picioare  scale(1,2,1)  → Ly=1.0 → Y_center = 0.1 + 1.0 = 1.1
    ///   Culcat       scale(2,1,1)  → Ly=0.5 → Y_center = 0.1 + 0.5 = 0.6
    ///                scale(1,1,2)
    //
    /// Deltas la trecerea intre stari:
    ///   In picioare  → Culcat:      ΔY = 0.6 - 1.1 = -0.5
    ///   Culcat       → In picioare: ΔY = 1.1 - 0.6 = +0.5
    //
    /// Distanta horizontala la rasturnare (pivot = muchia de jos a fetei):
    ///   In picioare (Lz=0.5): centru sare 0.5 + 1.0 = 1.5 tile-uri in directia miscarii
    ///   Culcat (Lz=1.0):      centru sare 1.0 + 0.5 = 1.5 tile-uri
    ///   Alunecare:            centru sare 1.0 tile
    ///
    ///   ATENTIE! Unghiul nu se va modifica in animatie, pentru ca duce la miscari
    ///   nefiresti ale cuboidului
    // -------------------------------------------------------------------------

    /// Tasta sus - blocul se misca in directia -Z
    void Cuboid::moveUp() {
        if (animState != IDLE) return;  // Nu permite input în timpul animației
        updateOrientation();

        glm::vec3 newPos = position;
        glm::vec3 newScale = scale;

        if (isStanding) {
            // In picioare → Culcat pe Z (larg pe axa Z)
            newPos += glm::vec3(0.0f, -0.5f, -1.5f);
            newScale = glm::vec3(1.0f, 1.0f, 2.0f);
            startAnimation(MOVING_UP, newPos, newScale, 0.0f,
                glm::vec3(1.0f, 0.0f, 0.0f));
        } else {
            if (scale.z > 1.5f) {
                // Culcat pe Z → se ridica in picioare
                newPos += glm::vec3(0.0f, 0.5f, -1.5f);
                newScale = glm::vec3(1.0f, 2.0f, 1.0f);
                startAnimation(STANDING_UP, newPos, newScale, 0.0f,
                    glm::vec3(1.0f, 0.0f, 0.0f));
            } else {
                // Culcat pe X → alunecare -Z
                newPos += glm::vec3(0.0f, 0.0f, -1.0f);
                startAnimation(MOVING_UP, newPos, newScale, 0.0f,
                    glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
    }

    /// Tasta jos - bloc se misca in directia +Z
    void Cuboid::moveDown() {
        if (animState != IDLE) return;
        updateOrientation();

        glm::vec3 newPos = position;
        glm::vec3 newScale = scale;

        if (isStanding) {
            // In picioare → Culcat pe Z
            newPos += glm::vec3(0.0f, -0.5f, 1.5f);
            newScale = glm::vec3(1.0f, 1.0f, 2.0f);
            startAnimation(MOVING_DOWN, newPos, newScale, 0.0f,
                glm::vec3(1.0f, 0.0f, 0.0f));
        } else {
            if (scale.z > 1.5f) {
                // Culcat pe Z → se ridica in picioare
                newPos += glm::vec3(0.0f, 0.5f, 1.5f);
                newScale = glm::vec3(1.0f, 2.0f, 1.0f);
                startAnimation(STANDING_UP, newPos, newScale, 0.0f,
                    glm::vec3(1.0f, 0.0f, 0.0f));
            } else {
                // Culcat pe X → alunecare +Z
                newPos += glm::vec3(0.0f, 0.0f, 1.0f);
                startAnimation(MOVING_DOWN, newPos, newScale, 0.0f,
                    glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
    }

    /// Tasta stanga - bloc se misca in directia -X
    void Cuboid::moveLeft() {
        if (animState != IDLE) return;
        updateOrientation();

        glm::vec3 newPos = position;
        glm::vec3 newScale = scale;

        if (isStanding) {
            // In picioare → Culcat pe X (larg pe axa X)
            newPos += glm::vec3(-1.5f, -0.5f, 0.0f);
            newScale = glm::vec3(2.0f, 1.0f, 1.0f);
            startAnimation(MOVING_LEFT, newPos, newScale, 0.0f,
                glm::vec3(0.0f, 0.0f, 1.0f));
        } else {
            if (scale.x > 1.5f) {
                // Culcat pe X → se ridica in picioare
                newPos += glm::vec3(-1.5f, 0.5f, 0.0f);
                newScale = glm::vec3(1.0f, 2.0f, 1.0f);
                startAnimation(STANDING_UP, newPos, newScale, 0.0f,
                    glm::vec3(0.0f, 0.0f, 1.0f));
            } else {
                // Culcat pe Z → alunecare -X
                newPos += glm::vec3(-1.0f, 0.0f, 0.0f);
                startAnimation(MOVING_LEFT, newPos, newScale, 0.0f,
                    glm::vec3(0.0f, 0.0f, 1.0f));
            }
        }
    }

    /// Tasta dreapta - bloc se misca in directia +X
    void Cuboid::moveRight() {
        if (animState != IDLE) return;
        updateOrientation();

        glm::vec3 newPos = position;
        glm::vec3 newScale = scale;

        if (isStanding) {
            // In picioare → Culcat pe X
            newPos += glm::vec3(1.5f, -0.5f, 0.0f);
            newScale = glm::vec3(2.0f, 1.0f, 1.0f);
            startAnimation(MOVING_RIGHT, newPos, newScale, 0.0f,
                glm::vec3(0.0f, 0.0f, 1.0f));
        } else {
            if (scale.x > 1.5f) {
                // Culcat pe X → se ridica in picioare
                newPos += glm::vec3(1.5f, 0.5f, 0.0f);
                newScale = glm::vec3(1.0f, 2.0f, 1.0f);
                startAnimation(STANDING_UP, newPos, newScale, 0.0f,
                    glm::vec3(0.0f, 0.0f, 1.0f));
            } else {
                // Culcat pe Z → alunecare +X
                newPos += glm::vec3(1.0f, 0.0f, 0.0f);
                startAnimation(MOVING_RIGHT, newPos, newScale, 0.0f,
                    glm::vec3(0.0f, 0.0f, 1.0f));
            }
        }
    }
}
