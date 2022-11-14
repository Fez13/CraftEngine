#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "debug/log.h"

namespace craft{

    struct deviceAbstraction{
    public:
        std::string name;
        VkDevice device;
        VkQueue queue;
        uint32_t family;
        VkCommandPool commandPool;
        VkFence fence;

        void findQueue();

        VkCommandBuffer createCommandBuffer(VkCommandBufferLevel bufferLevel, uint32_t bufferCount) const;

        void createFence(VkFenceCreateFlags flags);

        //TODO
        void SubmitWork();
        //TODO
        void SubmitWork(VkSemaphore toWait);

        void free() const;

        deviceAbstraction(std::string name, VkDevice device,VkQueue queue,uint32_t family){
            this->name = std::move(name);
            this->device = device;
            this->queue = queue;
            this->family = family;
            commandPool = nullptr;
            fence = nullptr;

            createCommandPool();

        };
    private:
        void createCommandPool();


    };
}
