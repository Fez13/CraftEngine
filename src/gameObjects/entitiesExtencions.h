#pragma once

#include "../glm/glm.hpp"
#include "../glm/gtx/quaternion.hpp"

namespace craft{
    //Should use quaternions
    struct Transform{
    public:
        glm::vec3 position;
        glm::vec3 rotation;
    };

    struct Update{
        virtual void update() = 0;
    };


}