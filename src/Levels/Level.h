//
// Created by Marian on 16.04.2026.
//

#ifndef BLOXORZ_SPG_LEVEL_H
#define BLOXORZ_SPG_LEVEL_H
#include <memory>

#include "src/Objects/Tiles/Tile.h"

namespace Levels {
    class Level {
    protected:
        std::vector<std::unique_ptr<ObjectModel::Tile>> tiles;

    public:
        virtual ~Level() = default;

        virtual void buildLevel() = 0;

        std::vector<std::unique_ptr<ObjectModel::Tile>>& getTiles() {
            return tiles;
        }
    };
}

#endif //BLOXORZ_SPG_LEVEL_H
