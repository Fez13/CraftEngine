#include "vk_graphic_device.h"
#include "../debug/time.h"


namespace craft{

    vk_graphic_device::vk_graphic_device(VkInstance &mainInstance, const std::vector<std::function<bool(VkPhysicalDeviceProperties&, VkPhysicalDeviceFeatures&)>>& checks = {}) {

        uint32_t devices = 0;

        vkEnumeratePhysicalDevices(mainInstance,&devices, nullptr);

        if(devices == 0){
            LOG_TERMINAL("There is not vulkan gpu available...",999)
        }
        m_availableDevices.resize(devices);
        vkEnumeratePhysicalDevices(mainInstance,&devices, m_availableDevices.data());

        bool pick = false;
        for(const VkPhysicalDevice &dv : m_availableDevices)
            if(deviceSuitable(dv,checks)){
                m_mainDevice = dv;
                pick = true;
            }


        if(!pick){
            LOG_TERMINAL("There is not vulkan gpu available with the requested features",999)
        }

        //Creates the main abstract device
        uint32_t queueFamilyIndex = getSuitableQueueFamily(mainInstance, [](const VkQueueFamilyProperties & fp){
            if(!(fp.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                return false;
            return true;
        });
        float priority = 1.0f;
        createDeviceAbstraction(queueFamilyIndex,"main",priority);
    }

    bool vk_graphic_device::deviceSuitable(VkPhysicalDevice const &device, const std::vector<std::function<bool(VkPhysicalDeviceProperties&, VkPhysicalDeviceFeatures&)>> &checks){
        bool valid = true;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        for(const std::function<bool(VkPhysicalDeviceProperties&,VkPhysicalDeviceFeatures&)>& validator: checks)
            valid = validator(deviceProperties,deviceFeatures);
        return valid;
    }
    uint32_t vk_graphic_device::getSuitableQueueFamily(VkInstance &mainInstance,
                                                       std::function<bool(const VkQueueFamilyProperties & fp)> checks) {
        uint32_t families = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_mainDevice, &families, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(families);
        vkGetPhysicalDeviceQueueFamilyProperties(m_mainDevice, &families, queueFamilies.data());

        for(uint32_t i = 0; i < queueFamilies.size(); i++)
            if(checks(queueFamilies[i]))
                return i;
        LOG_TERMINAL("There is not queue family with requested capabilities",999)
    }

    deviceAbstraction &vk_graphic_device::getDeviceAbstraction(std::string name) {
        for (auto &m_mainDeviceAbstraction: m_mainDeviceAbstractions)
            if (m_mainDeviceAbstraction.name == name)
                return m_mainDeviceAbstraction;
        LOG_TERMINAL("There is not deviceAbstraction with requested name",5)
    }

    void vk_graphic_device::free() {
        for(const auto& device : m_mainDeviceAbstractions)
            device.free();
        m_mainDeviceAbstractions.clear();
    }

    VkQueue &vk_graphic_device::getDeviceQueue(std::string &name, uint32_t queueIndex = 0) {
        for (auto &m_mainDeviceAbstraction: m_mainDeviceAbstractions)
            if (m_mainDeviceAbstraction.name == name)
                return m_mainDeviceAbstraction.queue[queueIndex];
        LOG_TERMINAL("There is not deviceAbstraction with requested name",5);
    }

    std::vector<std::string> vk_graphic_device::getAbstractionsData() {
        std::vector<std::string> data_v;

        std::string data;
        for(const deviceAbstraction& abs : m_mainDeviceAbstractions){
            data = abs.name;
            data += '\n';
            data += std::to_string(abs.family);
            data += '\n';
            data_v.push_back(data);
        }

        return data_v;
    }

    void vk_graphic_device::createDeviceAbstraction(uint32_t queueIndex, const std::string& name, float &priority, std::vector<const char*> gpuExtensions, VkPhysicalDeviceFeatures features) {

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.queueFamilyIndex = queueIndex;
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &features;

        deviceCreateInfo.ppEnabledExtensionNames = gpuExtensions.data();
        deviceCreateInfo.enabledExtensionCount = gpuExtensions.size();


        VkDevice newDevice;
        if(vkCreateDevice(m_mainDevice,&deviceCreateInfo, nullptr,&newDevice) != VK_SUCCESS){
            LOG_TERMINAL("Error creating a virtual device",999)
        }

        m_mainDeviceAbstractions.emplace_back(name,newDevice,queueIndex);
    }

    const VkPhysicalDevice &vk_graphic_device::getPhysicalDevice() {
        return m_mainDevice;
    }

    std::vector<uint32_t> vk_graphic_device::getAllUsedFamilies() {
        std::vector<uint32_t> indices;
        for(const deviceAbstraction &abstraction : m_mainDeviceAbstractions)
            indices.push_back(abstraction.family);

        sort( indices.begin(), indices.end() );
        indices.erase( unique( indices.begin(), indices.end() ), indices.end() );
        return indices;
    }

    std::vector<VkExtensionProperties> vk_graphic_device::getDeviceExtensions() {

        std::vector<VkExtensionProperties> pts;
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_mainDevice, nullptr,&extensionCount, nullptr);
        pts.resize(extensionCount);

        vkEnumerateDeviceExtensionProperties(m_mainDevice, nullptr,&extensionCount, pts.data());
        return pts;
    }

    vk_graphic_device::vk_graphic_device() : m_mainDevice(nullptr){}

}