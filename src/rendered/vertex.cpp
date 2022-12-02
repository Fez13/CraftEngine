#include "vertex.h"

namespace craft{

    VkVertexInputBindingDescription vertex::getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    std::vector<VkVertexInputAttributeDescription> vertex::getAttributeDescription() {
        std::vector<VkVertexInputAttributeDescription> AttributeDescriptions(2);

        AttributeDescriptions[0].binding = 0;
        AttributeDescriptions[0].location = 0;
        AttributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        AttributeDescriptions[0].offset = offsetof(vertex,position);

        AttributeDescriptions[1].binding = 0;
        AttributeDescriptions[1].location = 1;
        AttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        AttributeDescriptions[1].offset = offsetof(vertex, albedo);

        return AttributeDescriptions;
    }
}