#pragma once

#include <cstring>

#include "../gpu/vk_buffer.h"
#include "vertex.h"


namespace craft{
    class ebo {
    public:
        ebo() = default;
        ebo(const std::vector<uint32_t>&,deviceAbstraction& deviceAbstraction,const VkPhysicalDevice &device);

        void free();

        VkBuffer getBuffer();

    private:
        vk_buffer m_buffer;
    };
}

