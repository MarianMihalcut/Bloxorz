//
// Created by Marian on 14.04.2026.
//

#ifndef BLOXORZ_SPG_NORMALTILE_H
#define BLOXORZ_SPG_NORMALTILE_H

#include "Tile.h"

namespace ObjectModel {

    /**
     * @brief Tile standard, fără comportament special.
     *
     * Este traversabil și are o culoare albastră fixă.
     */
    class NormalTile : public Tile {
    public:
        /**
         * @brief Constructor standard.
         * @param gx       Coordonata X în grilă
         * @param gz       Coordonata Z în grilă
         * @param vertPath Calea vertex shader (opțional)
         * @param fragPath Calea fragment shader (opțional)
         */
        NormalTile(int gx, int gz,
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag");

        /**
         * @brief Returnează tipul TileType::NORMAL.
         */
        TileType getType() const override;

        /**
         * @brief Încarcă shaderele și construiește geometria în GPU.
         */
        void init() override;

        /**
         * @brief Randează tile-ul cu matricele de cameră și lumina date.
         */
        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;

        void display() override; ///< Neimplementat
        void update(float deltaTime) override; ///< Necesar (fără animație)
    };
}

#endif //BLOXORZ_SPG_NORMALTILE_H