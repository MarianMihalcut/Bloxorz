//
// Created by Marian on 14.04.2026.
//

#include "../Headers/NormalTile.h"

namespace ObjectModel {
    NormalTile::NormalTile(int gx, int gz, const std::string& vertPath, const std::string& fragPath)
        : Tile(gx, gz, 1.0f, 0.2f, 1.0f, glm::vec3(0.0f, 0.46f, 0.61f), vertPath, fragPath) {}

    TileType NormalTile::getType() const { return TileType::NORMAL; }

    void NormalTile::init() {
        loadShaders(vertShaderPath, fragShaderPath);

        std::vector<TileVertex> verts;
        std::vector<GLuint>     idxs;
        buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
        setupMesh(verts, idxs);
    }

    void NormalTile::render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                            const glm::vec3& viewPos) {
        if (!active) return;

        glm::mat4 mvp = vpMatrix * modelMatrix;
        applyUniforms(mvp, lightPos, viewPos);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void NormalTile::display() {}
    void NormalTile::update(float) {}
}