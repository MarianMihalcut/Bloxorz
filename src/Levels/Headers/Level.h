#ifndef BLOXORZ_SPG_LEVEL_H
#define BLOXORZ_SPG_LEVEL_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../../Objects/Tiles/Headers/Tile.h"

namespace Levels {
    class Level {
    protected:
        std::vector<std::unique_ptr<ObjectModel::Tile>> tiles;

    public:
        Level() = default;
        virtual ~Level();

        virtual void buildLevel() = 0;
        virtual glm::ivec2 getStartGridPos() const = 0;
        virtual glm::vec3 getCameraPos() const = 0;
        virtual glm::vec3 getCameraTarget() const = 0;

        std::vector<std::unique_ptr<ObjectModel::Tile>>& getTiles();
    };
}

#endif // BLOXORZ_SPG_LEVEL_H