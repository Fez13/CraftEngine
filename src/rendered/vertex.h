#pragma once

#include <cstdlib>
#include <array>

#include "../vendor/glm/glm.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define ATRIBUTE_COUNT 3
namespace craft{
    struct vertex{
        glm::vec3 position;
        glm::vec4 albedo;//!                        <------ TODO: This will be relpaced by the material system
        glm::vec2 textureCordinates;
        //glm::vec3 normal;

        static VkVertexInputBindingDescription getBindingDescription();

        static std::array<VkVertexInputAttributeDescription,ATRIBUTE_COUNT> getAttributeDescription();

    };
}


