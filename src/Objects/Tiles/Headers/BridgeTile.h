//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_BRIDGETILE_H
#define BLOXORZ_SPG_BRIDGETILE_H

#include "Tile.h"

namespace ObjectModel {

    /**
     * @brief Pod ce poate fi activat/dezactivat de un buton.
     *
     * Are animații de fading in/out și poate fi traversabil doar când este activ.
     */
    class BridgeTile : public Tile {
    public:
        /**
         * @brief Stările posibile ale podului.
         */
        enum class BridgeState {
            INACTIVE,    ///< Invizibil, nu se randează
            FADING_IN,   ///< Animație de apariție
            ACTIVE,      ///< Vizibil și traversabil
            FADING_OUT   ///< Animație de dispariție
        };

    private:
        BridgeState state;      ///< Starea curentă
        float animDuration;     ///< Durata animației (secunde)
        float animTimer;        ///< Timer pentru animație

        static constexpr float ANIM_Y_OFFSET = 2.0f; ///< Deplasarea verticală în animație
        static constexpr glm::vec3 COLOR_BRIDGE = glm::vec3(0.90f, 0.55f, 0.10f); ///< Culoare portocalie

        /**
         * @brief Actualizează matricea de model cu un offset vertical.
         * @param yOffset Deplasarea pe axa Y
         */
        void updateModelMatrix(float yOffset = 0.0f);

    public:
        /**
         * @brief Constructor.
         * @param gx          Coordonata X în grilă
         * @param gz          Coordonata Z în grilă
         * @param startActive Dacă podul începe activ (vizibil)
         * @param animSec     Durata animației de fade (0 = instant)
         * @param vertPath    Calea vertex shader
         * @param fragPath    Calea fragment shader
         */
        BridgeTile(int gx, int gz, bool startActive = false, float animSec = 0.4f,
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag");

        TileType getType() const override;
        BridgeState getState() const; ///< Returnează starea curentă

        /**
         * @brief Verifică dacă blocul poate traversa acest tile.
         * @return true dacă este activ sau în animație de apariție
         */
        bool isTraversable() const;

        void init() override;

        /// Activează podul (pornind animația FADING_IN)
        void activate();

        /// Dezactivează podul (pornind animația FADING_OUT)
        void deactivate();

        void update(float deltaTime) override;
        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;
        void display() override;
    };
}

#endif //BLOXORZ_SPG_BRIDGETILE_H