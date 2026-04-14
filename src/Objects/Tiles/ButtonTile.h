//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_BUTTONTILE_H
#define BLOXORZ_SPG_BUTTONTILE_H

#include "Tile.h"
#include <functional>

namespace ObjectModel {
    /// Un tile cu buton vizibil pe suprafata superioara.
    /// Cand blocul sta pe el, se apasa (coborat vizual) si cheama un callback.
    /// Geometria: tile-ul de baza + un indicator (disc/prisma) pe fata superioara.
    ///
    /// Redare in doua draw call-uri:
    ///   1. Corpul tile-ului  (culoare: gri inchis)
    ///   2. Indicatorul       (culoare: rosu nepasit / verde pasat)
    class ButtonTile : public Tile {
    public:
        /// Tipuri de buton (determina daca bridgeul se mentine sau nu)
        enum class ButtonMode {
            TOGGLE,  // fiecare apasare schimba starea bridge-ului
            HOLD     // bridge-ul e activ DOAR cat blocul sta pe buton
        };
    private:
        // --- Starea butonului ---
        bool pressed;
        ButtonMode mode;

        // --- Callback apelat la schimbarea starii (pressed/unpressed) ---
        std::function<void(bool)> onStateChange;

        // --- GPU objects pentru indicatorul de pe suprafata ---
        GLuint indicatorVAO, indicatorVBO, indicatorEBO;
        GLsizei indicatorIndexCount;

        // Culori
        static constexpr glm::vec3 COLOR_BODY     = glm::vec3(0.25f, 0.25f, 0.30f); // gri inchis
        static constexpr glm::vec3 COLOR_UNPRESSED = glm::vec3(0.85f, 0.20f, 0.15f); // rosu
        static constexpr glm::vec3 COLOR_PRESSED   = glm::vec3(0.15f, 0.75f, 0.25f); // verde

        /// Construieste geometria indicatorului (o prisma mica deasupra tile-ului)
        void buildIndicator() {
            glm::vec3 color = pressed ? COLOR_PRESSED : COLOR_UNPRESSED;

            // Indicator: un cub mic centrat pe fata superioara a tile-ului
            float iw = tileWidth  * 0.3f;
            float ih = tileHeight * 0.6f; // inainte de apasare
            float id = tileDepth  * 0.3f;

            // Il ridicam cu jumatatea inaltimii tile-ului + jumatatea inaltimii proprii
            float yOffset = tileHeight / 2.0f + ih / 2.0f;

            std::vector<TileVertex> verts;
            std::vector<GLuint>     idxs;
            buildBox(iw, ih, id, color, verts, idxs);

            // Translatem vertexii manual cu offset-ul Y
            for (auto& v : verts) {
                v.position.y += yOffset;
            }

            indicatorIndexCount = static_cast<GLsizei>(idxs.size());

            if (indicatorVAO == 0) {
                glGenVertexArrays(1, &indicatorVAO);
                glGenBuffers(1, &indicatorVBO);
                glGenBuffers(1, &indicatorEBO);
            }

            glBindVertexArray(indicatorVAO);

            glBindBuffer(GL_ARRAY_BUFFER, indicatorVBO);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(TileVertex),
                         verts.data(), GL_DYNAMIC_DRAW); // DYNAMIC - culoarea se schimba

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicatorEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size() * sizeof(GLuint),
                         idxs.data(), GL_DYNAMIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TileVertex),
                                  (void*)offsetof(TileVertex, position));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TileVertex),
                                  (void*)offsetof(TileVertex, normal));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TileVertex),
                                  (void*)offsetof(TileVertex, color));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
        }

    public:
        /// @param gx, gz       - pozitie in grila
        /// @param mode         - TOGGLE sau HOLD
        /// @param callback     - functie apelata cu (true) la apasare, (false) la eliberare
        /// @param vertPath/fragPath - cai shadere
        ButtonTile(int gx, int gz, ButtonMode bMode = ButtonMode::TOGGLE,
                   std::function<void(bool)> callback = nullptr,
                   const std::string& vertPath = "src/Objects/Tile/tile.vert",
                   const std::string& fragPath = "src/Objects/Tile/tile.frag")
            : Tile(gx, gz,
                   1.0f, 0.2f, 1.0f,
                   COLOR_BODY, vertPath, fragPath)
            , pressed(false)
            , mode(bMode)
            , onStateChange(std::move(callback))
            , indicatorVAO(0), indicatorVBO(0), indicatorEBO(0)
            , indicatorIndexCount(0)
        {}

        ~ButtonTile() override {
            glDeleteVertexArrays(1, &indicatorVAO);
            glDeleteBuffers(1, &indicatorVBO);
            glDeleteBuffers(1, &indicatorEBO);
        }

        TileType getType() const override { return TileType::BUTTON; }
        bool isPressed()   const          { return pressed; }
        ButtonMode getMode() const        { return mode; }

        void init() override {
            loadShaders(vertShaderPath, fragShaderPath);

            // Corpul tile-ului
            std::vector<TileVertex> verts;
            std::vector<GLuint>     idxs;
            buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
            setupMesh(verts, idxs);

            // Indicatorul de deasupra
            buildIndicator();
        }

        /// Apelata de logica jocului cand blocul intra pe tile
        void press() {
            if (pressed) return;
            pressed = true;
            buildIndicator(); // recoloreaza in verde
            if (onStateChange) onStateChange(true);
        }

        /// Apelata cand blocul paraseste tile-ul
        void release() {
            if (!pressed) return;
            pressed = false;
            buildIndicator(); // recoloreaza in rosu
            if (onStateChange) onStateChange(false);
        }

        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                const glm::vec3& viewPos) override
        {
            if (!active) return;

            glm::mat4 mvp = vpMatrix * modelMatrix;
            applyUniforms(mvp, lightPos, viewPos);

            // 1. Corpul tile-ului
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);

            // 2. Indicatorul (acelasi shader, alta geometrie)
            glBindVertexArray(indicatorVAO);
            glDrawElements(GL_TRIANGLES, indicatorIndexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    };
}

#endif //BLOXORZ_SPG_BUTTONTILE_H
