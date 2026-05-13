//
// Created by Marian on 11.04.2026.
//

#ifndef BLOXORZ_SPG_CUBOID_H
#define BLOXORZ_SPG_CUBOID_H

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "../ObjectModelInterface.h"

namespace ObjectModel {
    enum AnimationState {
        IDLE,
        MOVING_UP,
        MOVING_DOWN,
        MOVING_LEFT,
        MOVING_RIGHT,
        STANDING_UP,
        LAYING_DOWN
    };

    class Cuboid : public ObjectModelInterface {
    private:
        glm::vec3 position;
        glm::vec3 targetPosition;
        glm::vec3 scale;
        glm::vec3 targetScale;
        glm::vec3 color;
        float rotation;
        float targetRotation;
        glm::vec3 rotationAxis;

        int nrFaces;
        int nrVerticesPerFace;
        int nrVertices;

        std::vector<float> vertices;

        glm::mat4 projectionMatrix, viewMatrix;
        glm::vec3 lightPos, viewPos;

        bool isStanding;
        AnimationState animState;
        float animProgress; //de la 0% la 100%
        float animSpeed; //viteza animatiei

        GLuint textureID;           // Texture for cuboid_wall.jpg
        GLuint shadowMapFBO;        // Framebuffer for shadow map
        GLuint shadowMapTexture;    // Depth texture for shadows
        GLint shadowMapSize = 2048; // Resolution of shadow map

        void generateVertices();
        void startAnimation(AnimationState state, glm::vec3 newPos, glm::vec3 newScale,
                           float newRot, glm::vec3 rotAxis);

    public:
        Cuboid();
        Cuboid(glm::vec3 pos, glm::vec3 scl, glm::vec3 col);
        ~Cuboid();

        void init() override;
        void display() override;
        void update(float deltaTime) override;

        void setProjectionMatrix(glm::mat4 proj);
        void setViewMatrix(glm::mat4 view);
        void setLightPos(glm::vec3 light);
        void setViewPos(glm::vec3 vPos);

        void setPosition(glm::vec3 pos);
        void setScale(glm::vec3 scl);
        void setColor(glm::vec3 col);
        void setRotation(float rot);

        glm::vec3 getPosition();
        glm::vec3 getScale();
        bool getIsStanding();
        bool isAnimating();  // Verifică dacă e în animație

        void moveUp();
        void moveDown();
        void moveLeft();
        void moveRight();
        void updateOrientation();
    };

}

#endif //BLOXORZ_SPG_CUBOID_H
