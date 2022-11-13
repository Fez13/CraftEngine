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

        void findQueue();

        void createCommandPool();

        void free() const;

        deviceAbstraction(std::string name, VkDevice device,VkQueue queue,uint32_t family){
            this->name = std::move(name);
            this->device = device;
            this->queue = queue;
            this->family = family;
            commandPool = nullptr;
        };

    };
}