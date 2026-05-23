#ifndef BLOXORZ_SPG_LEVEL2_H
#define BLOXORZ_SPG_LEVEL2_H

#include "Level.h"

namespace Levels {

    /**
     * @brief Al doilea nivel (complex, cu butoane și poduri).
     *
     * Grila 15x6, blocul pornește la (0,2).
     * Conține două butoane tip TOGGLE care activează/dezactivează poduri:
     * - Butonul la (8,1) → activează podul la (10,3) și (11,3)
     * - Butonul la (2,2) → activează podul la (4,4) și (5,4)
     */
    class Level2 : public Level {
    public:
        /**
         * @brief Constructor.
         */
        Level2();

        /**
         * @brief Construiește nivelul 2.
         * @details Creează podurile (BridgeTile) înaintea butoanelor pentru a putea captura
         *          pointerii în lambda-urile callback. Adaugă toate tile-urile normale,
         *          butoanele și finish-ul (13,0). Inițializează toate tile-urile.
         */
        void buildLevel() override;

        /**
         * @brief Poziția de start a cuboidului.
         * @return (0, 2)
         */
        glm::ivec2 getStartGridPos() const override;

        /**
         * @brief Poziția camerei pentru nivelul 2.
         * @return (7.0, 20.0, 26.0)
         */
        glm::vec3 getCameraPos() const override;

        /**
         * @brief Punctul țintă al camerei.
         * @return (7.0, 0.0, 2.5)
         */
        glm::vec3 getCameraTarget() const override;
    };
}

#endif // BLOXORZ_SPG_LEVEL2_H