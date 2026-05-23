//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_BUTTONTILE_H
#define BLOXORZ_SPG_BUTTONTILE_H

#include "Tile.h"
#include <functional>
#include "src/stb_image.h"

namespace ObjectModel {

    /**
     * @brief Tile cu buton care poate activa poduri.
     *
     * Când cuboidul stă pe acest tile, se apelează un callback (de obicei pentru activarea podurilor).
     * Are o textură pe fața superioară (smiley).
     */
    class ButtonTile : public Tile {
    public:
        /**
         * @brief Modul de operare al butonului.
         */
        enum class ButtonMode {
            TOGGLE,  ///< Fiecare apăsare comută starea (pornit/oprit)
            HOLD     ///< Activ doar cât timp blocul stă pe buton (neimplementat complet)
        };

    private:
        bool pressed;                       ///< Starea curentă (apăsat/eliberat)
        ButtonMode mode;                    ///< Modul de operare
        std::function<void(bool)> onStateChange; ///< Callback apelat la schimbarea stării
        GLuint textureId;                   ///< ID-ul texturii (smiley)
        std::string texturePath;            ///< Calea către fișierul texturii
        static constexpr glm::vec3 COLOR_BODY = glm::vec3(0.25f, 0.25f, 0.30f); ///< Gri închis

        /// Încarcă textura PNG cu stb_image
        void loadTexture();

    public:
        /**
         * @brief Constructor.
         * @param gx       Coordonata X în grilă
         * @param gz       Coordonata Z în grilă
         * @param bMode    Modul butonului (TOGGLE / HOLD)
         * @param callback Funcția apelată la schimbarea stării (true = apăsat, false = eliberat)
         * @param texPath  Calea către textură
         * @param vertPath Calea vertex shader
         * @param fragPath Calea fragment shader
         */
        ButtonTile(int gx, int gz, ButtonMode bMode = ButtonMode::TOGGLE,
                   std::function<void(bool)> callback = nullptr,
                   const std::string& texPath = "../src/Objects/Tiles/500px-Smiley.png",
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile_button.frag");

        ~ButtonTile() override;

        TileType getType() const override;
        bool isPressed() const;          ///< Returnează dacă butonul este apăsat
        ButtonMode getMode() const;      ///< Returnează modul butonului

        void init() override;

        /**
         * @brief Apelată când cuboidul intră pe acest tile.
         * @details Schimbă starea și apelează callback-ul.
         */
        void press();

        /**
         * @brief Apelată când cuboidul părăsește acest tile.
         */
        void release();

        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;

        void display() override;
        void update(float deltaTime) override;
    };
}

#endif //BLOXORZ_SPG_BUTTONTILE_H