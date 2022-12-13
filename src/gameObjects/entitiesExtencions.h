#pragma once

#include "../glm/glm.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "../glm/gtc/quaternion.hpp"

namespace craft{
    //Should use quaternions
    struct Transform{
    public:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale{1,1,1};

        glm::mat4 mainMat;

        void updateTransformMatrix();
    };

    struct Update{
        virtual void update() = 0;
    };


}