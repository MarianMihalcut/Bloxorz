#include "../Headers/Level.h"

namespace Levels {
    Level::~Level() = default;

    std::vector<std::unique_ptr<ObjectModel::Tile>>& Level::getTiles() {
        return tiles;
    }
}