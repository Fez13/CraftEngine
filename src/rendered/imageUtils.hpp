#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "../debug/log.h"
#include "../vendor/glm/glm.hpp"
#include "../gpu/vk_device_abstraction.h"

namespace craft
{
    inline uint32_t findMemory(const VkMemoryPropertyFlags memoryProperties,VkPhysicalDeviceMemoryProperties deviceMemoryProperties,uint32_t memoryType){
        for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount;i++){
            if(!(memoryType & (1 < i)))
                continue;


            if((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
                return i;
        }
        LOG_TERMINAL("Couldn't find memory properties",1)
    }       

    inline void createImageView(VkImageViewType viewType, VkFormat format,VkDevice device, VkImageView& imageView, VkImage& image, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT
                , uint32_t baseMinp = 0, uint32_t baseArray = 0, uint32_t levelCount = 1, uint32_t layerCount = 1){
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.viewType = viewType;
        info.image = image;
        info.format = format;

        //TODO: Maybe this could be customizable in the future
        info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        info.subresourceRange.aspectMask = aspectFlags;
        info.subresourceRange.baseMipLevel = baseMinp;
        info.subresourceRange.levelCount = levelCount;
        info.subresourceRange.baseArrayLayer = baseArray;
        info.subresourceRange.layerCount = layerCount;


        if (vkCreateImageView(device, &info, nullptr, &imageView) != VK_SUCCESS){
            LOG_TERMINAL("Fail creating a image view...",999)
        }
    }

    inline void transitionImageLayout(deviceAbstraction *device,VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout){
        
        VkCommandBuffer cmd = device->createOneTimeUseCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            cmd,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
        vkEndCommandBuffer(cmd);
        device->SubmitWork(&cmd,1);
    }

    //I think this should be a type of buffer, but I am not sure if it fits the definition
    inline void createImage(glm::ivec2 size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image
                     , VkDeviceMemory& imageMemory, VkDevice &device,VkPhysicalDevice gpu, VkImageLayout imagelayout = VK_IMAGE_LAYOUT_UNDEFINED){
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.x;
        imageInfo.extent.height = size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = imagelayout;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            LOG_TERMINAL("Fail creating a image...",999)
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(gpu,&deviceMemoryProperties);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemory(properties,deviceMemoryProperties, memRequirements.memoryTypeBits);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            LOG_TERMINAL("Fail creating an image memory...",999)
        }
        vkBindImageMemory(device, image, imageMemory, 0);
    }

    inline void createImageSampler(VkSampler &sampler, VkDevice &device,const VkPhysicalDevice &gpu,VkFilter magFilter,VkFilter minFilter, VkBool32 normalizeCordinates,VkBool32 compareOp){
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_FALSE; //TODO:
        samplerInfo.maxAnisotropy = 1.0f;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(gpu, &properties);

        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = normalizeCordinates;

        samplerInfo.compareEnable = compareOp;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            LOG_TERMINAL("Fail creating an image sampler...",999)
        }
    }

};
