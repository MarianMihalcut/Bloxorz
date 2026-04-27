//
// Created by Marian on 16.04.2026.
//

#ifndef BLOXORZ_SPG_LEVEL_H
#define BLOXORZ_SPG_LEVEL_H
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "src/Objects/Tiles/Tile.h"

namespace Levels {
    class Level {
    protected:
        std::vector<std::unique_ptr<ObjectModel::Tile>> tiles;

    public:
        virtual ~Level() = default;

        /// Construieste geometria nivelului (tile-uri + init GPU)
        virtual void buildLevel() = 0;

        /// Pozitia de start a cuboidului in grila (X, Z)
        virtual glm::ivec2 getStartGridPos() const = 0;

        /// Pozitia camerei pentru ca nivelul sa incapa pe ecran
        virtual glm::vec3 getCameraPos()    const = 0;
        virtual glm::vec3 getCameraTarget() const = 0;

        std::vector<std::unique_ptr<ObjectModel::Tile>>& getTiles() { return tiles; }
    };
}

#endif //BLOXORZ_SPG_LEVEL_H
