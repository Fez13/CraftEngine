#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "../debug/log.h"

namespace craft{

    struct deviceAbstraction{
    public:
        std::string name;
        VkDevice device;
        std::vector<VkQueue> queue;
        uint32_t family;
        VkCommandPool commandPool;
        VkFence fence;

        void findNewQueue();

        VkCommandBuffer createCommandBuffer(VkCommandBufferLevel bufferLevel, uint32_t bufferCount) const;

        void createFence(VkFenceCreateFlags flags);

        void SubmitWork(VkCommandBuffer *commandBuffers,uint32_t count,uint32_t queueIndex = 0);
        //TODO
        void SubmitWork(VkSemaphore toWait);

        void waitToFinish(uint32_t queueIndex = 0);

        void free() const;

        deviceAbstraction(std::string name, VkDevice device,uint32_t family){
            this->name = std::move(name);
            this->device = device;
            this->family = family;
            commandPool = nullptr;
            fence = nullptr;

            createCommandPool();
            findNewQueue();
        };
    private:
        void createCommandPool();


    };
}
