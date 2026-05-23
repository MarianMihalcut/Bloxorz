//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_TILE_H
#define BLOXORZ_SPG_TILE_H

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "src/Objects/ObjectModelInterface.h"

namespace ObjectModel {
    /// Tipurile de tile-uri posibile
    enum class TileType {
        NORMAL,
        BUTTON,
        BRIDGE,
        FINISH
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

        // --- Pozitia luminii pentru rendering ---
        glm::vec3 lightPos;

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
        void loadShaders(const std::string& vertPath, const std::string& fragPath);

        /// Incarca geometria (vertices + indices) in VAO/VBO/EBO.
        /// Apelata din init() al claselor copil dupa construirea vertexilor.
        void setupMesh(const std::vector<TileVertex>& vertices,
                       const std::vector<GLuint>&    indices);

        /// Trimite uniform-urile catre shader inainte de draw call.
        /// mvpMatrix, modelMatrix, lightPos, viewPos
        void applyUniforms(const glm::mat4& mvp, const glm::vec3& lightPos,
                           const glm::vec3& viewPos) const;

        /// Construieste geometria unui paralelipiped cu dimensiunile date si culoarea specificata.
        /// Returneaza vertices si indices gata de incarcat in GPU.
        /// Fiecare fata are 4 vertices proprii (pentru normale corecte per-fata).
        static void buildBox(float w, float h, float d, const glm::vec3& color,
                             std::vector<TileVertex>& outVertices, std::vector<GLuint>&     outIndices);

    public:
        /// Constructor: pozitie in grila, dimensiuni, culoare, cai shader
        Tile(int gx, int gz, float width, float height, float depth,
             const glm::vec3& color, const std::string& vertPath, const std::string& fragPath);

        /// Destructor
        virtual ~Tile();

        // ---------------------------------------------------------------
        // Getteri
        // ---------------------------------------------------------------
        int getGridX()           const;
        int getGridZ()           const;
        bool isActive()          const;
        glm::mat4 getModelMatrix() const;
        virtual TileType getType() const = 0;

        // ---------------------------------------------------------------
        // Setteri
        // ---------------------------------------------------------------
        void setLightPos(glm::vec3 light);

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