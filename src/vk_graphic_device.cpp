#include "vk_graphic_device.h"

namespace craft{

    graphicProcessor::graphicProcessor(VkInstance &mainInstance,const std::vector<std::function<bool(VkPhysicalDeviceProperties&,VkPhysicalDeviceFeatures&)>>& checks = {}) {

        uint32_t devices = 0;

        vkEnumeratePhysicalDevices(mainInstance,&devices, nullptr);

        if(devices == 0)
            throw std::runtime_error("There is not vulkan gpu available...");

        m_availableDevices.resize(devices);
        vkEnumeratePhysicalDevices(mainInstance,&devices, m_availableDevices.data());

        bool pick = false;
        for(const VkPhysicalDevice &dv : m_availableDevices)
            if(deviceSuitable(dv,checks)){
                m_mainDevice = dv;
                pick = true;
            }

        if(!pick)
            throw std::runtime_error("There is not vulkan gpu available with the requested features");

        uint32_t queueFamilyIndex = getSuitableQueueFamily([](const VkQueueFamilyProperties & pts){
            if(pts.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                return true;
            return false;

        });

        //TODO: Currently it only has support for 1 queue, this is not permanent

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1;
        float priority = 1.0f;
        queueCreateInfo.pQueuePriorities = &priority;

        VkPhysicalDeviceFeatures thisIsBad{};

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &thisIsBad;


    }

    bool graphicProcessor::deviceSuitable(VkPhysicalDevice const &device,const std::vector<std::function<bool(VkPhysicalDeviceProperties&,VkPhysicalDeviceFeatures&)>> &checks){
        bool valid = true;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        for(const std::function<bool(VkPhysicalDeviceProperties&,VkPhysicalDeviceFeatures&)>& validator: checks)
            valid = validator(deviceProperties,deviceFeatures);
        return valid;
    }

    uint32_t graphicProcessor::getSuitableQueueFamily(const std::function<bool(const VkQueueFamilyProperties &)>& checks) {
        uint32_t families = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_mainDevice, &families, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(families);
        vkGetPhysicalDeviceQueueFamilyProperties(m_mainDevice, &families, queueFamilies.data());

        for(uint32_t i = 0; i < queueFamilies.size(); i++)
            if(checks(queueFamilies[i]))
                return i;
        throw std::runtime_error("There is not queue family with requested capabilities");
    }

    VkDevice &graphicProcessor::getDeviceAbstraction(std::string &name) {
        for (auto &m_mainDeviceAbstraction: m_mainDeviceAbstractions)
            if (m_mainDeviceAbstraction.second == name)
                return m_mainDeviceAbstraction.first;
        throw std::runtime_error("There is not deviceAbstraction with requested name");
    }

}