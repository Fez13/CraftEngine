#pragma once

#include <cstdlib>
#include <vector>
#include <algorithm>
#include <memory>

#include "glm.hpp"
#include "vertex.h"
#include "../debug/log.h"
#include "../rendered/vao.h"
#include "../rendered/ebo.h"
#include "../gpu/vk_graphic_device.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace craft{

    struct geometry{
        std::vector<vertex> vertices;
        std::vector<uint32_t> indices;

        geometry(const std::vector<vertex>&,const std::vector<uint32_t>&);
    };

    class Mesh {
    public:

        explicit Mesh(const char* deviceName,geometry&);

        void free();

        size_t getIndicesCount();

        std::array<VkBuffer,1> &getVaoArray();

        ~Mesh();

        vao& getVao();
        ebo& getEbo();

    private:
        std::unique_ptr<geometry> m_geometry;

        std::array<VkBuffer,1> m_vao_data{};

        vao m_vao{};
        ebo m_ebo{};
    };
}