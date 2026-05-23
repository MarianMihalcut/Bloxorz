#ifndef BLOXORZ_SPG_LEVEL_H
#define BLOXORZ_SPG_LEVEL_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "../../Objects/Tiles/Headers/Tile.h"

namespace Levels {

    /**
     * @brief Clasă abstractă de bază pentru toate nivelurile jocului.
     *
     * Gestionează colecția de tile-uri și oferă interfața pentru construcția nivelului,
     * poziția de start a cuboidului și poziția camerei.
     */
    class Level {
    protected:
        std::vector<std::unique_ptr<ObjectModel::Tile>> tiles; ///< Toate tile-urile nivelului

    public:
        /**
         * @brief Constructor implicit.
         */
        Level() = default;

        /**
         * @brief Destructor virtual.
         */
        virtual ~Level();

        /**
         * @brief Construiește geometria și logica nivelului.
         * @details Creează tile-urile (NormalTile, BridgeTile, ButtonTile, FinishTile)
         *          și le inițializează. Apelat o dată la încărcarea nivelului.
         */
        virtual void buildLevel() = 0;

        /**
         * @brief Returnează coordonatele (X, Z) în grilă de unde pornește cuboidul.
         * @return Perechea (gridX, gridZ) a poziției de start.
         */
        virtual glm::ivec2 getStartGridPos() const = 0;

        /**
         * @brief Returnează poziția camerei în spațiul lumii.
         * @return Coordonatele camerei (X, Y, Z).
         */
        virtual glm::vec3 getCameraPos() const = 0;

        /**
         * @brief Returnează punctul spre care este orientată camera.
         * @return Coordonatele punctului țintă.
         */
        virtual glm::vec3 getCameraTarget() const = 0;

        /**
         * @brief Acces la vectorul de tile-uri al nivelului.
         * @return Referință la vectorul de unique_ptr-uri către Tile.
         */
        std::vector<std::unique_ptr<ObjectModel::Tile>>& getTiles();
    };
}

#endif // BLOXORZ_SPG_LEVEL_H