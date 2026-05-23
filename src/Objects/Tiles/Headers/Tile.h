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

    /**
     * @brief Tipurile de tile-uri posibile în joc.
     */
    enum class TileType {
        NORMAL,   ///< Tile normal, traversabil
        BUTTON,   ///< Buton care activează poduri
        BRIDGE,   ///< Pod ce poate fi activat/dezactivat
        FINISH    ///< Tile de finalizare a nivelului
    };

    /**
     * @brief Structura unui vertex trimis către shader.
     *
     * Corespunde layout-ului:
     * - location = 0: poziție (vec3)
     * - location = 1: normală (vec3)
     * - location = 2: culoare (vec3)
     */
    struct TileVertex {
        glm::vec3 position; ///< Coordonatele spațiale
        glm::vec3 normal;   ///< Normala la suprafață
        glm::vec3 color;    ///< Culoarea asociată (ignorată dacă se folosește textură)
    };

    /**
     * @brief Clasă de bază pentru toate tile-urile din nivel.
     *
     * Gestionează geometria, shaderele, matricea de model și starea de activare.
     * Clasele derivate implementează comportamente specifice (buton, pod, final).
     */
    class Tile : public ObjectModelInterface {
    protected:
        // --- Pozitia in grila ---
        int gridX;          ///< Coordonata X în grila nivelului
        int gridZ;          ///< Coordonata Z în grila nivelului

        // --- Dimensiunile tile-ului (spatiu world) ---
        float tileWidth;    ///< Lățimea pe axa X
        float tileHeight;   ///< Înălțimea pe axa Y
        float tileDepth;    ///< Adâncimea pe axa Z

        // --- Culoarea de baza a tile-ului ---
        glm::vec3 baseColor; ///< Culoare folosită în lipsa texturii

        // --- Matricea de model (pozitionare in scena) ---
        glm::mat4 modelMatrix; ///< Matricea de transformare a obiectului

        // --- Starea tile-ului (activ/inactiv - relevant pt BridgeTile) ---
        bool active; ///< Dacă tile-ul este vizibil și/sau activ

        // --- Pozitia luminii pentru rendering ---
        glm::vec3 lightPos; ///< Poziția sursei de lumină în spațiul lumii

        // --- Cai catre fisierele shader ---
        std::string vertShaderPath; ///< Calea către fișierul vertex shader
        std::string fragShaderPath; ///< Calea către fișierul fragment shader

        // --- Numarul de indici pentru draw call ---
        GLsizei indexCount; ///< Numărul de indecși în EBO

        // ---------------------------------------------------------------
        // Metode ajutatoare protejate
        // ---------------------------------------------------------------

        /**
         * @brief Încarcă, compilează și leagă vertex + fragment shader.
         * @param vertPath Calea către fișierul vertex shader
         * @param fragPath Calea către fișierul fragment shader
         */
        void loadShaders(const std::string& vertPath, const std::string& fragPath);

        /**
         * @brief Încarcă geometria (vertices + indices) în VAO/VBO/EBO.
         * @param vertices Vectorul de vertexuri
         * @param indices  Vectorul de indecși
         */
        void setupMesh(const std::vector<TileVertex>& vertices,
                       const std::vector<GLuint>&    indices);

        /**
         * @brief Trimite uniform-urile către shader înainte de draw call.
         * @param mvp      Matricea MVP (proiecție * vizualizare * model)
         * @param lightPos Poziția luminii
         * @param viewPos  Poziția observatorului
         */
        void applyUniforms(const glm::mat4& mvp, const glm::vec3& lightPos,
                           const glm::vec3& viewPos) const;

        /**
         * @brief Construiește geometria unui paralelipiped cu dimensiunile date.
         * @param w            Lățimea (axa X)
         * @param h            Înălțimea (axa Y)
         * @param d            Adâncimea (axa Z)
         * @param color        Culoarea asociată
         * @param outVertices  Vectorul de ieșire pentru vertexuri
         * @param outIndices   Vectorul de ieșire pentru indecși
         */
        static void buildBox(float w, float h, float d, const glm::vec3& color,
                             std::vector<TileVertex>& outVertices, std::vector<GLuint>& outIndices);

    public:
        /**
         * @brief Constructor.
         * @param gx       Coordonata X în grilă
         * @param gz       Coordonata Z în grilă
         * @param width    Lățimea în lume
         * @param height   Înălțimea în lume
         * @param depth    Adâncimea în lume
         * @param color    Culoarea de bază
         * @param vertPath Calea vertex shader
         * @param fragPath Calea fragment shader
         */
        Tile(int gx, int gz, float width, float height, float depth,
             const glm::vec3& color, const std::string& vertPath, const std::string& fragPath);

        /// Destructor virtual
        virtual ~Tile();

        // ---------------------------------------------------------------
        // Getteri
        // ---------------------------------------------------------------
        int getGridX()           const; ///< Returnează coordonata X în grilă
        int getGridZ()           const; ///< Returnează coordonata Z în grilă
        bool isActive()          const; ///< Returnează starea de activare
        glm::mat4 getModelMatrix() const; ///< Returnează matricea de model
        virtual TileType getType() const = 0; ///< Returnează tipul tile-ului

        // ---------------------------------------------------------------
        // Setteri
        // ---------------------------------------------------------------
        void setLightPos(glm::vec3 light); ///< Setează poziția luminii

        // ---------------------------------------------------------------
        // Metode pur virtuale mostenite din ObjectModelInterface
        // ---------------------------------------------------------------
        virtual void display() = 0;      ///< Afișează obiectul (nefolosit la tile-uri)
        virtual void init()    = 0;      ///< Inițializează resursele OpenGL
        virtual void update(float deltaTime) = 0; ///< Actualizează logica (animatii)

        /**
         * @brief Randare tile cu parametrii camerei și luminii.
         * @param vpMatrix Matricea VP (View * Projection)
         * @param lightPos Poziția luminii
         * @param viewPos  Poziția camerei
         */
        virtual void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                            const glm::vec3& viewPos) = 0;
    };
}

#endif //BLOXORZ_SPG_TILE_H