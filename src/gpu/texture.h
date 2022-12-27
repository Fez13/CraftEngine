#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../vendor/stb/stb_image.h"
#include "../vendor/glm/glm.hpp"

#include "vk_buffer.h"
#include "../debug/log.h"
#include "../gpu/vk_graphic_device.h"
#include  "../rendered/imageUtils.hpp"

#include<cstring>
#include<iostream>
namespace craft{
    

    class Texture
    {
    public:

        Texture() = default;

        Texture(std::string path, const std::string& deviceName,VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);
    
        void free();

        static void createDefaultTexture(const std::string& deviceName,VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

        static std::pair<VkSampler,VkImageView> getDefaultBindData();

        static void freeDefault();
        
        std::pair<VkSampler,VkImageView> bindData();

    private:

        glm::ivec2 texture_size;
        
        int textureChannels;
        
        VkImageTiling m_tiling;

        uint32_t m_memoryIndex;

        void createImages(vk_buffer& stageBuffer);

        VkImage m_image;
        VkImageView m_imageView;
        VkDeviceMemory m_memory;
        VkSampler m_imageSampler;
        deviceAbstraction *m_mainDeviceAbstraction;

        static Texture s_default;

    };

};