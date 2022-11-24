#pragma once

#include <cstdlib>
#include <vector>

#include "../glm/glm.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>



namespace craft{
    struct vertex{
        glm::vec3 position;
        glm::vec4 albedo;

        static VkVertexInputBindingDescription getBindingDescription();

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();

    };

    class mesh {

    };
}