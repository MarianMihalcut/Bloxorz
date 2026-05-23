//
// Created by Marian on 17.04.2026.
//

#ifndef BLOXORZ_SPG_FINISHTILE_H
#define BLOXORZ_SPG_FINISHTILE_H

#include "Tile.h"
#include <functional>

namespace ObjectModel {

    /**
     * @brief Tile de finalizare a nivelului.
     *
     * Când cuboidul ajunge în picioare pe acest tile, se apelează un callback
     * care notifică trecerea la nivelul următor.
     */
    class FinishTile : public Tile {
    private:
        std::function<void()> onFinish; ///< Callback apelat la finish
        bool triggered;                 ///< Evită apelări multiple

        static constexpr glm::vec3 COLOR_FINISH = glm::vec3(0.10f, 0.85f, 0.30f); ///< Verde aprins

    public:
        /**
         * @brief Constructor.
         * @param gx       Coordonata X în grilă
         * @param gz       Coordonata Z în grilă
         * @param finishCb Callback apelat la finish
         * @param vertPath Calea vertex shader
         * @param fragPath Calea fragment shader
         */
        FinishTile(int gx, int gz,
                   std::function<void()> finishCb = nullptr,
                   const std::string& vertPath = "../src/Objects/Tiles/tile.vert",
                   const std::string& fragPath = "../src/Objects/Tiles/tile.frag");

        TileType getType() const override;

        /**
         * @brief Setează callback-ul de finish.
         * @param cb Funcția de callback
         */
        void setOnFinish(std::function<void()> cb);

        /**
         * @brief Declanșează finalizarea nivelului.
         * @details Apelată de Game când cuboidul aterizează pe acest tile în poziție verticală.
         */
        void triggerFinish();

        /**
         * @brief Resetează starea (util la restart).
         */
        void reset();

        void init() override;
        void render(const glm::mat4& vpMatrix, const glm::vec3& lightPos,
                    const glm::vec3& viewPos) override;
        void display() override;
        void update(float) override;
    };
}

#endif //BLOXORZ_SPG_FINISHTILE_H