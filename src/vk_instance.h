#pragma once
#include <iostream>
#include <vector>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace craft{

    struct vk_instance{

        vk_instance(const char* appName, uint32_t appVersion, uint32_t apiVersion, const std::vector<std::string>& layers);

        VkInstance& getInstance();

        static int setValidationLayers(const std::vector<std::string>& layers,VkInstanceCreateInfo &appInfo);

        static std::vector<VkLayerProperties> getAvailableLayers();

        static std::vector<VkExtensionProperties> getAvailableExtensions();

        ~vk_instance();

    private:

        static bool checkLayer(const std::vector<VkLayerProperties> &availableLayers , const std::string& layer);

        VkInstance m_instance;

    };


}
