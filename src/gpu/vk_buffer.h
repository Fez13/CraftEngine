#pragma once

#include <cstdlib>
#include <vector>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../debug/log.h"
#include "../gpu/vk_device_abstraction.h"

namespace craft{

    class vk_buffer {
    public:

        explicit vk_buffer(deviceAbstraction &mainDevice,uint32_t size,const VkBufferUsageFlags &usage, VkSharingMode sharingMode);

        void createBufferMemoryRequirements(const VkMemoryPropertyFlags &memoryProperties,const VkPhysicalDevice& device);

        void allocateMemory();

        template<class T>
        T *getMemoryLocation() {
            void* data;
            vkMapMemory(m_mainDevice->device,m_memory,0,m_size,0,&data);
            return static_cast<T*>(data);
        }

        void unMapMemory();

        VkBuffer getBuffer();

        friend void operator<<(vk_buffer& input, vk_buffer& output);

        void free();

        void bindBuffer();

    private:

        VkMemoryRequirements m_memoryRequirements;

        uint32_t m_size;

        uint32_t m_memoryIndex;

        deviceAbstraction* m_mainDevice;

        VkPhysicalDevice m_mainGpu;

        VkMemoryPropertyFlags m_memoryProperties;

        VkBuffer m_buffer;

        VkBufferUsageFlags m_localUsages;

        VkDeviceMemory m_memory;

    };

}
