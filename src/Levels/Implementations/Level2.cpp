#include "../Headers/Level2.h"
#include "../../Objects/Tiles/Headers/NormalTile.h"
#include "../../Objects/Tiles/Headers/BridgeTile.h"
#include "../../Objects/Tiles/Headers/ButtonTile.h"
#include "src/Objects/Tiles/Headers/FinishTile.h"

namespace Levels {
    Level2::Level2() {}

    // ---------------------------------------------------------------------------
    // Construirea nivelului 2 (mai complex decat primul)
    //
    //  Grila (X→, Z↓):
    //
    //     0   1   2   3   4   5   6   7   8   9  10  11   12  13  14
    //  0 [ ] [ ] [ ] [ ] [ ] [ ] [N] [N] [N] [N] [ ] [ ]  [N] [F] [N]
    //  1 [ ] [ ] [ ] [ ] [ ] [ ] [N] [N] [B] [N] [ ] [ ]  [N] [N] [N]         B = ButtonTile
    //  2 [N] [N] [B] [N] [ ] [ ] [N] [N] [N] [N] [ ] [ ]  [N] [N] [N]         P = BridgeTile (pod)
    //  3 [N] [N] [N] [N] [ ] [ ] [N] [N] [N] [N] [P] [P]  [N] [N] [N]          F = finish
    //  4 [N] [N] [N] [N] [P] [P] [N] [N] [N] [N] [ ] [ ]  [ ] [ ] [ ]          B→P(10,3)+P(11,3)
    //  5 [N] [N] [N] [N] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]  [ ] [ ] [ ]          B→P(4,4)+P(5,4)
    //
    //  Blocul porneste la (0,2).
    //  Buton (8,1)  → activeaza podul (10,3)-(11,3)
    //  Buton (2,2)  → activeaza podul  (4,4)-(5,4)
    // ---------------------------------------------------------------------------
    void Level2::buildLevel() {
        // Bridge-urile sunt create INAINTE de butoanele lor, astfel incat
        // pointerii capturati in lambda sa fie valizi la momentul apelului.

        // Pod 1: activat de butonul (8,1)
        auto bridge_10_3 = std::make_unique<ObjectModel::BridgeTile>(10, 3, false, 0.4f);
        auto bridge_11_3 = std::make_unique<ObjectModel::BridgeTile>(11, 3, false, 0.4f);
        ObjectModel::BridgeTile* pb1 = bridge_10_3.get();
        ObjectModel::BridgeTile* pb2 = bridge_11_3.get();
        tiles.emplace_back(std::move(bridge_10_3));
        tiles.emplace_back(std::move(bridge_11_3));

        // Pod 2: activat de butonul (2,2)
        auto bridge_4_4 = std::make_unique<ObjectModel::BridgeTile>(4, 4, false, 0.4f);
        auto bridge_5_4 = std::make_unique<ObjectModel::BridgeTile>(5, 4, false, 0.4f);
        ObjectModel::BridgeTile* pb3 = bridge_4_4.get();
        ObjectModel::BridgeTile* pb4 = bridge_5_4.get();
        tiles.emplace_back(std::move(bridge_4_4));
        tiles.emplace_back(std::move(bridge_5_4));

        // Rand Z=0: N la X = 6,7,8,9,12,13,14
        for (int x : {6, 7, 8, 9, 12, 14})
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(x, 0));
        tiles.emplace_back(std::make_unique<ObjectModel::FinishTile>(13,0));

        // Rand Z=1: N la X = 6,7,9,12,13, 14 | B la X=8
        for (int x : {6, 7, 9, 12, 13, 14})
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(x, 1));

        tiles.emplace_back(std::make_unique<ObjectModel::ButtonTile>(
            8, 1,
            ObjectModel::ButtonTile::ButtonMode::TOGGLE,
            [pb1, pb2](bool pressed) {
                if (pressed) {
                    pb1->activate();
                    pb2->activate();
                }
                else {
                    pb1->deactivate();
                    pb2->deactivate();
                }
            }
        ));

        // Rand Z=2: N la X = 0,1,3,6,7,8,9,12,13,14 | B la X=2
        for (int x : {0, 1, 3, 6, 7, 8, 9, 12, 13, 14})
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(x, 2));

        tiles.emplace_back(std::make_unique<ObjectModel::ButtonTile>(
            2, 2,
            ObjectModel::ButtonTile::ButtonMode::TOGGLE,
            [pb3, pb4](bool pressed) {
                if (pressed) {
                    pb3->activate();
                    pb4->activate();
                }
                else {
                    pb3->deactivate();
                    pb4->deactivate();
                }
            }
        ));

        // Rand Z=3: N la X = 0,1,2,3,6,7,8,9,12,13,14 | P la X=10,11 (deja adaugat)
        for (int x : {0, 1, 2, 3, 6, 7, 8, 9, 12, 13, 14})
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(x, 3));

        // Rand Z=4: N la X = 0,1,2,3,6,7,8,9 | P la X=4,5 (deja adaugat)
        for (int x : {0, 1, 2, 3, 6, 7, 8, 9})
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(x, 4));

        // Rand Z=5: N la X = 0,1,2,3
        for (int x : {0, 1, 2, 3})
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(x, 5));

        // Initializam toate tile-urile (shaders + GPU buffers)
        for (auto& t : tiles)
            t->init();
    }

    glm::ivec2 Level2::getStartGridPos() const {
        return {0, 2};
    }

    glm::vec3 Level2::getCameraPos() const {
        return {7.0f, 20.0f, 26.0f};
    }

    glm::vec3 Level2::getCameraTarget() const {
        return {7.0f, 0.0f, 2.5f};
    }
}