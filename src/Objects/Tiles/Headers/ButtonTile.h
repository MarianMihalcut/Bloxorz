//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_BUTTONTILE_H
#define BLOXORZ_SPG_BUTTONTILE_H

#include "Tile.h"
#include <functional>

#include "src/stb_image.h"

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
            /// Logica de HOLD nu e implementata in acest proiect
        };
    private:
        // --- Starea butonului ---
        bool pressed;
        ButtonMode mode;

        // --- Callback apelat la schimbarea starii (pressed/unpressed) ---
        std::function<void(bool)> onStateChange;

        // Textura
        GLuint textureId = 0;
        std::string texturePath;

        // Culori
        static constexpr glm::vec3 COLOR_BODY     = glm::vec3(0.25f, 0.25f, 0.30f); // gri inchis

        // -----------------------------------------------------------------------
        // Incarca textura PNG cu stb_image
        // -----------------------------------------------------------------------
        void loadTexture();

    public:
        /// @param gx, gz       - pozitie in grila
        /// @param mode         - TOGGLE sau HOLD
        /// @param callback     - functie apelata cu (true) la apasare, (false) la eliberare
        /// @param vertPath/fragPath - cai shadere
        ButtonTile(int gx, int gz, ButtonMode bMode = ButtonMode::TOGGLE,
                   std::function<void(bool)> callback = nullptr,
                   const std::string& texPath = "../src/Objects/Tiles/500px-Smiley.png",
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile_button.frag");

        ~ButtonTile() override;

        TileType getType() const override;
        bool isPressed() const;
        ButtonMode getMode() const;

        void init() override;

        /// Apelata de logica jocului cand blocul intra pe tile
        void press();

        /// Apelata cand blocul paraseste tile-ul
        void release();

        void render(const glm::mat4& vpMatrix,
                    const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;

        void display() override;
        void update(float deltaTime) override;
    };
}

#endif //BLOXORZ_SPG_BUTTONTILE_H