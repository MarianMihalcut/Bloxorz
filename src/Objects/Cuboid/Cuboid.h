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

#include "../../Interfaces/ObjectModelInterface.h"

namespace ObjectModel {
    class Cuboid : public ObjectModelInterface {
    private:
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 color;
        float rotation;

        int nrFaces;
        int nrVerticesPerFace;
        int nrVertices;

        std::vector<float> vertices;

        glm::mat4 projectionMatrix, viewMatrix;
        glm::vec3 lightPos, viewPos;

        void generateVertices();

    public:
        Cuboid();
        Cuboid(glm::vec3 pos, glm::vec3 scl, glm::vec3 col);
        ~Cuboid();

        void init() override;
        void display() override;

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

        void move(glm::vec3 delta);
        void rotateY(float angle);
    };

}

#endif //BLOXORZ_SPG_CUBOID_H
