#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include "utils.hpp"
#include "glm.hpp"

#include "debug/log.h"

namespace craft{

    struct vk_window{

        GLFWwindow *mainWindow;

        VkSurfaceKHR surface;

        explicit vk_window(glm::ivec2 windowSize, uint32_t refreshRate);

        explicit vk_window();

        void createSurface(VkInstance& instance);

        //Use AFTER creating properties
        void createSwapChain(const VkDevice &device,const std::vector<uint32_t>& queueFamilies,uint32_t swapChainImageCount = 0);

        void createSwapChainProperties(const VkPhysicalDevice& device) const;

        void setWindowName(const std::string &newName) const;

        void setWindowSize(const glm::ivec2 &newSize);

        glm::ivec2 getWindowSize() const;

        uint32_t getRefreshRate() const;

        void free(const VkInstance& instance,const VkDevice &KHRdevice) const;

        uint32_t findQueueFamily(VkPhysicalDevice mainDevice) const;

        void update(const VkDevice& device);

        VkExtent2D getExtent() const;

    private:

        struct SwapChainData{
            bool populated = false;
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
            uint32_t imagesInSwapChain = 0;
        };

        struct SwapChain{
            bool populated = false;
            VkSurfaceFormatKHR format;
            VkPresentModeKHR presentMode;
            VkExtent2D extent;
            VkSwapchainKHR swapChainKhr;

            void UpdateImages(const VkDevice& device);

            void UpdateImageViews(const VkDevice& device);

            std::vector<VkImage> images;
            std::vector<VkImageView> imagesViews;
        };

        mutable uint32_t m_queueFamily;

        mutable SwapChainData m_swapChainData;

        SwapChain m_swapChain;

        uint32_t m_refreshRate;

        glm::ivec2 m_windowSize;
    };

}
