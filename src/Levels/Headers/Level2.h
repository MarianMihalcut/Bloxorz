#ifndef BLOXORZ_SPG_LEVEL2_H
#define BLOXORZ_SPG_LEVEL2_H

#include "Level.h"

namespace Levels {
    class Level2 : public Level {
    public:
        Level2();

        void buildLevel() override;
        glm::ivec2 getStartGridPos() const override;
        glm::vec3 getCameraPos() const override;
        glm::vec3 getCameraTarget() const override;
    };
}

#endif // BLOXORZ_SPG_LEVEL2_H