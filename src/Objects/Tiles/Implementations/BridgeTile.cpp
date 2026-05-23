//
// Created by Marian on 14.04.2026.
//

#include "../Headers/BridgeTile.h"

namespace ObjectModel {
    void BridgeTile::updateModelMatrix(float yOffset) {
        glm::vec3 worldPos(gridX * tileWidth, yOffset, gridZ * tileDepth);
        modelMatrix = glm::translate(glm::mat4(1.0f), worldPos);
    }

    BridgeTile::BridgeTile(int gx, int gz, bool startActive, float animSec,
                           const std::string& vertPath, const std::string& fragPath)
        : Tile(gx, gz, 1.0f, 0.2f, 1.0f, COLOR_BRIDGE, vertPath, fragPath)
        , state(startActive ? BridgeState::ACTIVE : BridgeState::INACTIVE)
        , animDuration(animSec)
        , animTimer(0.0f) {
        active = startActive;
    }

    TileType BridgeTile::getType() const { return TileType::BRIDGE; }
    BridgeTile::BridgeState BridgeTile::getState() const { return state; }

    bool BridgeTile::isTraversable() const {
        return state == BridgeState::ACTIVE || state == BridgeState::FADING_IN;
    }

    void BridgeTile::init() {
        loadShaders(vertShaderPath, fragShaderPath);

        std::vector<TileVertex> verts;
        std::vector<GLuint> idxs;
        buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
        setupMesh(verts, idxs);

        updateModelMatrix(0.0f);
    }

    void BridgeTile::activate() {
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

    void BridgeTile::deactivate() {
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

    void BridgeTile::update(float deltaTime) {
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

    void BridgeTile::render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                            const glm::vec3& viewPos) {
        // Tile-ul complet inactiv (si fara animatie in curs) nu se deseneaza
        if (state == BridgeState::INACTIVE) return;

        glm::mat4 mvp = vpMatrix * modelMatrix;
        applyUniforms(mvp, lightPos, viewPos);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void BridgeTile::display() {}
}