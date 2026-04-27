//
// Created by Marian on 17.04.2026.
//

#ifndef BLOXORZ_SPG_FINISHTILE_H
#define BLOXORZ_SPG_FINISHTILE_H
#include "Tile.h"

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
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag")
            : Tile(gx, gz, 1.0f, 0.2f, 1.0f, COLOR_FINISH, vertPath, fragPath)
            , onFinish(std::move(finishCb))
            , triggered(false)
        {}

        TileType getType() const override { return TileType::FINISH; }

        ///@param cb - functia de callback
        void setOnFinish(std::function<void()> cb) { onFinish = std::move(cb); }

        /// Apelata din Game::checkLanding() — cuboidul e in picioare pe acest tile
        void triggerFinish() {
            if (triggered) return;
            triggered = true;
            if (onFinish) onFinish();
        }

        /// Reseteaza starea (util la restart)
        void reset() { triggered = false; }

        void init() override {
            loadShaders(vertShaderPath, fragShaderPath);
            std::vector<TileVertex> verts; std::vector<GLuint> idxs;
            buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
            setupMesh(verts, idxs);
        }

        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                const glm::vec3& viewPos) override
        {
            if (!active) return;
            applyUniforms(vpMatrix * modelMatrix, lightPos, viewPos);
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        void display() override {}
        void update(float) override {}
    };
}

#endif //BLOXORZ_SPG_FINISHTILE_H
