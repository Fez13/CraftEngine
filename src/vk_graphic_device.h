#pragma once

#include <cstdlib>
#include <vector>
#include <iostream>

#include <functional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace craft{

    class graphicProcessor{

    public:

        graphicProcessor(VkInstance &mainInstance,const std::vector<std::function<bool(VkPhysicalDeviceProperties&,VkPhysicalDeviceFeatures&)>>& checks);

        uint32_t getSuitableQueueFamily(const std::function<bool(const VkQueueFamilyProperties&)>& checks);

        VkDevice &getDeviceAbstraction(std::string &name);

    private:

        VkPhysicalDevice m_mainDevice;

        std::vector<std::pair<VkDevice,std::string>> m_mainDeviceAbstractions;

        std::vector<VkPhysicalDevice> m_availableDevices;

        static bool deviceSuitable(VkPhysicalDevice const &device,const std::vector<std::function<bool(VkPhysicalDeviceProperties&,VkPhysicalDeviceFeatures&)>> &checks);

    };




}