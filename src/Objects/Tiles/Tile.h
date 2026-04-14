//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_TILE_H
#define BLOXORZ_SPG_TILE_H

#include "src/Interfaces/ObjectModelInterface.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <glew.h>

namespace ObjectModel {
    /// Tipurile de tile-uri posibile
    enum class TileType {
        NORMAL,
        BUTTON,
        BRIDGE
    };

    /// Structura unui vertex trimis catre shader
    /// Corespunde layout-ului: location=0 (pos), location=1 (normal), location=2 (color)
    struct TileVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

    class Tile : public ObjectModelInterface {
    protected:
        // --- Pozitia in grila ---
        int gridX, gridZ;

        // --- Dimensiunile tile-ului (spatiu world) ---
        float tileWidth;
        float tileHeight;
        float tileDepth;

        // --- Culoarea de baza a tile-ului ---
        glm::vec3 baseColor;

        // --- Matricea de model (pozitionare in scena) ---
        glm::mat4 modelMatrix;

        // --- Starea tile-ului (activ/inactiv - relevant pt BridgeTile) ---
        bool active;

        // --- Cai catre fisierele shader ---
        std::string vertShaderPath;
        std::string fragShaderPath;

        // --- Numarul de indici pentru draw call ---
        GLsizei indexCount;

        // ---------------------------------------------------------------
        // Metode ajutatoare protejate
        // ---------------------------------------------------------------

        /// Incarca, compileaza si leaga vertex + fragment shader.
        /// Citeste fisierele de la vertPath / fragPath.
        void loadShaders(const std::string& vertPath, const std::string& fragPath) {
            // Citim sursele
            std::string vertSrc = textFileRead(vertPath);
            std::string fragSrc = textFileRead(fragPath);
            const char* vertexShader = vertSrc.c_str();
            const char* fragmentShader = fragSrc.c_str();

            // Compilam vertex shader
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vs, 1, &vertexShader, nullptr);
            glCompileShader(vs);
            printShaderInfoLog(vs);

            // Compilam fragment shader
            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fs, 1, &fragmentShader, nullptr);
            glCompileShader(fs);
            printShaderInfoLog(fs);

            // Cream si legam programul
            shader_programme = glCreateProgram();
            glAttachShader(shader_programme, vs);
            glAttachShader(shader_programme, fs);
            glLinkProgram(shader_programme);
            printProgramInfoLog(shader_programme);

            // Shaderele individuale nu mai sunt necesare dupa link
            glDeleteShader(vs);
            glDeleteShader(fs);
        }

        /// Incarca geometria (vertices + indices) in VAO/VBO/EBO.
        /// Apelata din init() al claselor copil dupa construirea vertexilor.
        void setupMesh(const std::vector<TileVertex>& vertices,
                   const std::vector<GLuint>&    indices)
        {
            indexCount = static_cast<GLsizei>(indices.size());

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            glBindVertexArray(vao);

            // VBO - date vertices
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TileVertex),
                         vertices.data(), GL_STATIC_DRAW);

            // EBO - indici
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                         indices.data(),GL_STATIC_DRAW);

            // layout(location = 0) in vec3 v_pos
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(TileVertex), (void*)offsetof(TileVertex, position));
            glEnableVertexAttribArray(0);

            // layout(location = 1) in vec3 v_normal
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(TileVertex), (void*)offsetof(TileVertex, normal));
            glEnableVertexAttribArray(1);

            // layout(location = 2) in vec3 v_color
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(TileVertex), (void*)offsetof(TileVertex, color));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
        }

        /// Trimite uniform-urile catre shader inainte de draw call.
        /// mvpMatrix, modelMatrix, lightPos, viewPos
        void applyUniforms(const glm::mat4& mvp, const glm::vec3& lightPos,
                           const glm::vec3& viewPos) const
        {
            glUseProgram(shader_programme);

            GLuint locMVP   = glGetUniformLocation(shader_programme, "mvpMatrix");
            GLuint locModel = glGetUniformLocation(shader_programme, "modelMatrix");
            GLuint locLight = glGetUniformLocation(shader_programme, "lightPos");
            GLuint locView  = glGetUniformLocation(shader_programme, "viewPos");

            glUniformMatrix4fv(locMVP,   1, GL_FALSE, glm::value_ptr(mvp));
            glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniform3fv(locLight, 1, glm::value_ptr(lightPos));
            glUniform3fv(locView,  1, glm::value_ptr(viewPos));
        }

        /// Construieste geometria unui paralelipiped cu dimensiunile date si culoarea specificata.
        /// Returneaza vertices si indices gata de incarcat in GPU.
        /// Fiecare fata are 4 vertices proprii (pentru normale corecte per-fata).
        static void buildBox(float w, float h, float d, const glm::vec3& color,
                             std::vector<TileVertex>& outVertices, std::vector<GLuint>&     outIndices)
        {
            // Jumatati de dimensiuni, centrat in origine
            float hw = w / 2.0f;
            float hh = h / 2.0f;
            float hd = d / 2.0f;

            // Cele 6 fete: normal + 4 pozitii
            struct Face {
                glm::vec3 normal;
                glm::vec3 corners[4]; // quad in sens trigonometric
            };

            Face faces[6] = {
                // Sus  (+Y)
                { { 0, 1, 0}, { {-hw, hh,-hd}, { hw, hh,-hd}, { hw, hh, hd}, {-hw, hh, hd} } },
                // Jos  (-Y)
                { { 0,-1, 0}, { {-hw,-hh, hd}, { hw,-hh, hd}, { hw,-hh,-hd}, {-hw,-hh,-hd} } },
                // Fata (+Z)
                { { 0, 0, 1}, { {-hw,-hh, hd}, { hw,-hh, hd}, { hw, hh, hd}, {-hw, hh, hd} } },
                // Spate(-Z)
                { { 0, 0,-1}, { { hw,-hh,-hd}, {-hw,-hh,-hd}, {-hw, hh,-hd}, { hw, hh,-hd} } },
                // Dreapta(+X)
                { { 1, 0, 0}, { { hw,-hh, hd}, { hw,-hh,-hd}, { hw, hh,-hd}, { hw, hh, hd} } },
                // Stanga (-X)
                { {-1, 0, 0}, { {-hw,-hh,-hd}, {-hw,-hh, hd}, {-hw, hh, hd}, {-hw, hh,-hd} } },
            };

            outVertices.clear();
            outIndices.clear();

            for (int f = 0; f < 6; f++) {
                GLuint base = static_cast<GLuint>(outVertices.size());

                for (int v = 0; v < 4; v++) {
                    outVertices.push_back({ faces[f].corners[v], faces[f].normal, color });
                }

                // Doua triunghiuri per quad (CCW)
                outIndices.insert(outIndices.end(),
                    { base, base+1, base+2,
                      base, base+2, base+3 });
            }
        }

    public:
        /// Constructor: pozitie in grila, dimensiuni, culoare, cai shader
        Tile(int gx, int gz, float width, float height, float depth,
             const glm::vec3& color, const std::string& vertPath, const std::string& fragPath)
            : gridX(gx), gridZ(gz), tileWidth(width), tileHeight(height), tileDepth(depth)
            , baseColor(color), active(true), vertShaderPath(vertPath)
            , fragShaderPath(fragPath), indexCount(0)
        {
            // Pozitionam tile-ul in lume pe baza coordonatelor din grila
            modelMatrix = glm::translate(glm::mat4(1.0f),
                                         glm::vec3(gx * width, 0.0f, gz * depth));
        }

        /// Destructor
        virtual ~Tile() {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteProgram(shader_programme);
        }

        // ---------------------------------------------------------------
        // Getteri
        // ---------------------------------------------------------------
        int getGridX()           const { return gridX; }
        int getGridZ()           const { return gridZ; }
        bool isActive()          const { return active; }
        glm::mat4 getModelMatrix() const { return modelMatrix; }
        virtual TileType getType() const = 0;

        // ---------------------------------------------------------------
        // Metode pur virtuale mostenite din ObjectModelInterface
        // ---------------------------------------------------------------
        virtual void display() = 0;
        virtual void init()    = 0;
        virtual void update(float deltaTime) = 0;

        // ---------------------------------------------------------------
        // Metoda virtuala de randare cu parametri de camera/lumina
        // ---------------------------------------------------------------
        virtual void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                            const glm::vec3& viewPos) = 0;
    };



}

#endif //BLOXORZ_SPG_TILE_H
