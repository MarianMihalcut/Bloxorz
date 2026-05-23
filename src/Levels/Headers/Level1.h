#ifndef BLOXORZ_SPG_LEVEL1_H
#define BLOXORZ_SPG_LEVEL1_H

#include "Level.h"

namespace Levels {

    /**
     * @brief Primul nivel al jocului (simplu, fără butoane sau poduri).
     *
     * Grila 6x4, blocul pornește la (0,0), finalul la (5,3).
     * Nu există elemente interactive.
     */
    class Level1 : public Level {
    public:
        /**
         * @brief Constructor.
         */
        Level1();

        /**
         * @brief Construiește nivelul 1.
         * @details Adaugă tile-uri normale și un FinishTile la (5,3).
         *          Inițializează toate tile-urile după creare.
         */
        void buildLevel() override;

        /**
         * @brief Poziția de start a cuboidului.
         * @return (0, 0)
         */
        glm::ivec2 getStartGridPos() const override;

        /**
         * @brief Poziția camerei pentru nivelul 1.
         * @return (2.5, 10.0, 12.0)
         */
        glm::vec3 getCameraPos() const override;

        /**
         * @brief Punctul țintă al camerei.
         * @return (2.5, 0.0, 1.5)
         */
        glm::vec3 getCameraTarget() const override;
    };
}

#endif // BLOXORZ_SPG_LEVEL1_H