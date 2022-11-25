#pragma once

#include <cstring>

#include "../gpu/vk_buffer.h"
#include "vertex.h"

namespace craft{

    class vao{
    public:
        vao() = default;
        vao(const std::vector<vertex>&,deviceAbstraction& deviceAbstraction,const VkPhysicalDevice &device);

        void free();

        VkBuffer getBuffer();

    private:
        vk_buffer m_buffer;
    };

}


