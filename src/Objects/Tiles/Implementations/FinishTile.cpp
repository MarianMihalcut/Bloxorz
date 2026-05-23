//
// Created by Marian on 17.04.2026.
//

#include "../Headers/FinishTile.h"

namespace ObjectModel {
    FinishTile::FinishTile(int gx, int gz,
                           std::function<void()> finishCb,
                           const std::string& vertPath,
                           const std::string& fragPath)
        : Tile(gx, gz, 1.0f, 0.2f, 1.0f, COLOR_FINISH, vertPath, fragPath)
        , onFinish(std::move(finishCb))
        , triggered(false) {}

    TileType FinishTile::getType() const { return TileType::FINISH; }

    void FinishTile::setOnFinish(std::function<void()> cb) {
        onFinish = std::move(cb);
    }

    void FinishTile::triggerFinish() {
        if (triggered) return;
        triggered = true;
        if (onFinish) onFinish();
    }

    void FinishTile::reset() { triggered = false; }

    void FinishTile::init() {
        loadShaders(vertShaderPath, fragShaderPath);
        std::vector<TileVertex> verts;
        std::vector<GLuint> idxs;
        buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
        setupMesh(verts, idxs);
    }

    void FinishTile::render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                            const glm::vec3& viewPos) {
        if (!active) return;
        applyUniforms(vpMatrix * modelMatrix, lightPos, viewPos);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void FinishTile::display() {}
    void FinishTile::update(float) {}
}