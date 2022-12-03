#pragma once

#include <cstdlib>
#include <utility>
#include <vector>
#include <iostream>

#include <functional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../debug/log.h"
#include "vk_device_abstraction.h"
#include "../core/vk_instance.h"
#include "../core/vk_instance.h"

namespace craft{

    class vk_graphic_device{

    public:

        static vk_graphic_device &get();

        vk_graphic_device(const vk_graphic_device&) = delete;

        void initialize(const std::vector<std::function<bool(VkPhysicalDeviceProperties&, VkPhysicalDeviceFeatures&)>>& checks);

        uint32_t getSuitableQueueFamily(std::function<bool(const VkQueueFamilyProperties &)> checks);

        deviceAbstraction &getDeviceAbstraction(std::string name);

        VkQueue &getDeviceQueue(std::string &name,uint32_t queueIndex );

        const VkPhysicalDevice &getPhysicalDevice();

        std::vector<uint32_t> getAllUsedFamilies();

        std::vector<std::string> getAbstractionsData();

        std::vector<VkExtensionProperties> getDeviceExtensions();

        void createDeviceAbstraction(uint32_t queueIndex,const std::string& name,float& priority,std::vector<const char*> gpuExtensions = {},VkPhysicalDeviceFeatures features = VkPhysicalDeviceFeatures{});

        void free();

    private:

        vk_graphic_device() = default;

        VkPhysicalDevice m_mainDevice = nullptr;

        std::vector<deviceAbstraction> m_mainDeviceAbstractions;

        std::vector<VkPhysicalDevice> m_availableDevices;

        static bool deviceSuitable(VkPhysicalDevice const &device,const std::vector<std::function<bool(VkPhysicalDeviceProperties&,VkPhysicalDeviceFeatures&)>> &checks);

        static vk_graphic_device s_vk_graphic_device;

    };




}