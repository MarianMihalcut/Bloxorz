//
// Created by Marian on 17.04.2026.
//

#ifndef BLOXORZ_SPG_FINISHTILE_H
#define BLOXORZ_SPG_FINISHTILE_H

#include "Tile.h"
#include <functional>

namespace ObjectModel {
    class FinishTile : public Tile {
    private:
        // Callback apelat de Game cand conditia de finish e indeplinita
        std::function<void()> onFinish;
        bool triggered; // evitam apeluri multiple

        static constexpr glm::vec3 COLOR_FINISH = glm::vec3(0.10f, 0.85f, 0.30f); // verde aprins

    public:
        /// @param gx, gz      - pozitie in grila
        /// @param finishCb    - callback apelat la finish (setat de Game)
        FinishTile(int gx, int gz,
                   std::function<void()> finishCb = nullptr,
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag");

        TileType getType() const override;

        /// @param cb - functia de callback
        void setOnFinish(std::function<void()> cb);

        /// Apelata din Game::checkLanding() — cuboidul e in picioare pe acest tile
        void triggerFinish();

        /// Reseteaza starea (util la restart)
        void reset();

        void init() override;
        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;
        void display() override;
        void update(float) override;
    };
}

#endif //BLOXORZ_SPG_FINISHTILE_H