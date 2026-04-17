//
// Created by Marian on 16.04.2026.
//

#ifndef BLOXORZ_SPG_LEVEL1_H
#define BLOXORZ_SPG_LEVEL1_H
#include "Level.h"
#include "src/Objects/Tiles/NormalTile.h"

namespace Levels {
    class Level1 : public Level {
    public:
        // ---------------------------------------------------------------------------
        // Construirea nivelului 1
        //
        //  Grila (X→, Z↓):
        //
        //    0   1   2   3   4   5
        //  0 [N] [ ] [N] [N] [ ] [ ]
        //  1 [N] [N] [N] [N] [ ] [ ]    B = ButtonTile
        //  2 [N] [N] [N] [N] [ ] [ ]    P = BridgeTile (pod)
        //  3 [ ] [ ] [N] [N] [N] [F]    F = finish
        //
        //  Blocul porneste la (0,0).
        // ---------------------------------------------------------------------------
        void buildLevel() override {
            // -- Rand 0 --
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(0,0));
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(2,0));
            tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(3,0));

            // -- Rand 1 & rand 2 --
            for (int i=0;i<4;i++) {
                tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(i,1));
                tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(i,2));
            }

            // -- Rand 3 --
            for (int i=2;i<6;i++)
                tiles.emplace_back(std::make_unique<ObjectModel::NormalTile>(i,3));

            // Initializam toate tile-urile (shaders + GPU buffers)
            for (auto& t : tiles) {
                t->init();
            }
        }

        Level1() {}
    };
}

#endif //BLOXORZ_SPG_LEVEL1_H
