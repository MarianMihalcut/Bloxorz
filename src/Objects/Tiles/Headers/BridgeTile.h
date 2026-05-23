//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_BRIDGETILE_H
#define BLOXORZ_SPG_BRIDGETILE_H

#include "Tile.h"

namespace ObjectModel {
    class BridgeTile : public Tile {
    public:
        enum class BridgeState {
            INACTIVE,    // invizibil, nu se randeaza
            FADING_IN,   // animatie de aparitie
            ACTIVE,      // vizibil si traversabil
            FADING_OUT   // animatie de disparitie
        };
    private:
        BridgeState state;

        // --- Parametri animatie ---
        float animDuration;  // durata tranzitiei in secunde
        float animTimer;     // timp scurs din animatie

        // Offset Y: in FADING_IN incepe de sus si coboara la 0
        // in FADING_OUT porneste de la 0 si se duce in jos
        static constexpr float ANIM_Y_OFFSET = 2.0f;

        // Culoarea podului (portocaliu aprins - distinctiv fata de tile-urile normale)
        static constexpr glm::vec3 COLOR_BRIDGE = glm::vec3(0.90f, 0.55f, 0.10f);

        /// Recalculeaza modelMatrix cu un offset Y curent bazat pe starea animatiei
        void updateModelMatrix(float yOffset = 0.0f);

    public:
        /// @param gx, gz        - pozitie in grila
        /// @param startActive   - daca true, tile-ul incepe vizibil (unele puzzle-uri au bridge-uri active initial)
        /// @param animSec       - durata animatiei de fade (secunde), 0 = instant
        /// @param vertPath/fragPath
        BridgeTile(int gx, int gz, bool startActive = false, float animSec = 0.4f,
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag");

        TileType getType() const override;
        BridgeState getState() const;

        /// Returneaza true daca blocul poate calca pe acest tile
        bool isTraversable() const;

        void init() override;

        /// Activeaza podul (apelat de ButtonTile prin callback)
        void activate();

        /// Dezactiveaza podul
        void deactivate();

        void update(float deltaTime) override;
        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;
        void display() override;
    };
}

#endif //BLOXORZ_SPG_BRIDGETILE_H