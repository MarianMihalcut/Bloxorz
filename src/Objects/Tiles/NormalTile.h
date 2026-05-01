//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_NORMALTILE_H
#define BLOXORZ_SPG_NORMALTILE_H

#include "Tile.h"

namespace ObjectModel {
    class NormalTile : public Tile {
    public:
        /// Constructor standard
        /// @param gx, gz  - pozitia in grila
        /// @param vertPath, fragPath - cai catre fisierele shader
        NormalTile(int gx, int gz,
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag")
            : Tile(gx, gz,
                   1.0f, 0.2f, 1.0f,          // dimensiuni standard tile
                   glm::vec3(0.0f, 0.46f, 0.61f), // albastru
                   vertPath, fragPath)
        {}

        TileType getType() const override { return TileType::NORMAL; }

        /// Incarca shadere si construieste geometria in GPU
        void init() override {
            loadShaders(vertShaderPath, fragShaderPath);

            std::vector<TileVertex> verts;
            std::vector<GLuint>     idxs;
            buildBox(tileWidth, tileHeight, tileDepth, baseColor, verts, idxs);
            setupMesh(verts, idxs);
        }

        /// Randeaza tile-ul cu matricile de camera si lumina date
        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override
        {
            if (!active) return;

            glm::mat4 mvp = vpMatrix * modelMatrix;
            applyUniforms(mvp, lightPos, viewPos);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        void display() override {

        }

        void update(float deltaTime) override {
            // Tile-ul normal nu are logica de update
        }
    };
}



#endif //BLOXORZ_SPG_NORMALTILE_H
