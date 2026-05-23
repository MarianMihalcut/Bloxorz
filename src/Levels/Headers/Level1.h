#ifndef BLOXORZ_SPG_LEVEL1_H
#define BLOXORZ_SPG_LEVEL1_H

#include "Level.h"

namespace Levels {
    class Level1 : public Level {
    public:
        Level1();

        void buildLevel() override;
        glm::ivec2 getStartGridPos() const override;
        glm::vec3 getCameraPos() const override;
        glm::vec3 getCameraTarget() const override;
    };
}

#endif // BLOXORZ_SPG_LEVEL1_H