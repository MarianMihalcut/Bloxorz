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
        void updateModelMatrix(float yOffset = 0.0f) {
            glm::vec3 worldPos(gridX * tileWidth, yOffset, gridZ * tileDepth);
            modelMatrix = glm::translate(glm::mat4(1.0f), worldPos);
        }

    public:
        /// @param gx, gz        - pozitie in grila
        /// @param startActive   - daca true, tile-ul incepe vizibil (unele puzzle-uri au bridge-uri active initial)
        /// @param animSec       - durata animatiei de fade (secunde), 0 = instant
        /// @param vertPath/fragPath
        BridgeTile(int gx, int gz, bool startActive = false, float animSec = 0.4f,
                   const std::string& vertPath = "../src/Objects/Tiles/tile.frag",
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag")
            : Tile(gx, gz,
                   1.0f, 0.2f, 1.0f,
                   COLOR_BRIDGE, vertPath, fragPath)
            , state(startActive ? BridgeState::ACTIVE : BridgeState::INACTIVE)
            , animDuration(animSec)
            , animTimer(0.0f)
        {
            active = startActive;
        }

        TileType getType()   const override { return TileType::BRIDGE; }
        BridgeState getState() const        { return state; }

        /// Returneaza true daca blocul poate calca pe acest tile
        bool isTraversable() const {
            return state == BridgeState::ACTIVE || state == BridgeState::FADING_IN;
        }

        void init() override {
            loadShaders(vertShaderPath, fragShaderPath);

            std::vector<TileVertex> verts;
            std::vector<GLuint> idxs;
            buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
            setupMesh(verts, idxs);

            updateModelMatrix(0.0f);
        }

        /// Activeaza podul (apelat de ButtonTile prin callback)
        void activate() {
            if (state == BridgeState::ACTIVE || state == BridgeState::FADING_IN) return;

            active = true;
            animTimer = 0.0f;

            if (animDuration > 0.0f) {
                state = BridgeState::FADING_IN;
                updateModelMatrix(ANIM_Y_OFFSET); // incepe de sus
            } else {
                state = BridgeState::ACTIVE;
                updateModelMatrix(0.0f);
            }
        }

        /// Dezactiveaza podul
        void deactivate() {
            if (state == BridgeState::INACTIVE || state == BridgeState::FADING_OUT) return;

            animTimer = 0.0f;

            if (animDuration > 0.0f) {
                state = BridgeState::FADING_OUT;
            } else {
                state = BridgeState::INACTIVE;
                active = false;
                updateModelMatrix(0.0f);
            }
        }

        void update(float deltaTime) override {
            switch (state) {
                case BridgeState::FADING_IN: {
                    animTimer += deltaTime;
                    float t = glm::clamp(animTimer / animDuration, 0.0f, 1.0f);
                    // Interpolare liniara: Y coboara de la ANIM_Y_OFFSET la 0
                    float yOff = ANIM_Y_OFFSET * (1.0f - t);
                    updateModelMatrix(yOff);
                    if (t >= 1.0f) {
                        state = BridgeState::ACTIVE;
                        updateModelMatrix(0.0f);
                    }
                    break;
                }

                case BridgeState::FADING_OUT: {
                    animTimer += deltaTime;
                    float t = glm::clamp(animTimer / animDuration, 0.0f, 1.0f);
                    // Y coboara sub podea
                    float yOff = -ANIM_Y_OFFSET * t;
                    updateModelMatrix(yOff);
                    if (t >= 1.0f) {
                        state = BridgeState::INACTIVE;
                        active = false;
                        updateModelMatrix(0.0f);
                    }
                    break;
                }

                default:
                    break;
            }
        }

        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                const glm::vec3& viewPos) override
        {
            // Tile-ul complet inactiv (si fara animatie in curs) nu se deseneaza
            if (state == BridgeState::INACTIVE) return;

            glm::mat4 mvp = vpMatrix * modelMatrix;
            applyUniforms(mvp, lightPos, viewPos);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        void display() override {}
    };
}

#endif //BLOXORZ_SPG_BRIDGETILE_H
