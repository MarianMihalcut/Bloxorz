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
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag");

        TileType getType() const override;

        /// Incarca shadere si construieste geometria in GPU
        void init() override;

        /// Randeaza tile-ul cu matricile de camera si lumina date
        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;

        void display() override;
        void update(float deltaTime) override;
    };
}

#endif //BLOXORZ_SPG_NORMALTILE_H