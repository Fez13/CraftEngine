#include "vk_graphic_device.h"
#include "debug/time.h"


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

        //Creates the main abstract device
        uint32_t queueFamilyIndex = getSuitableQueueFamily(mainInstance, [](const VkQueueFamilyProperties & fp){
            if(!(fp.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                return false;
            return true;
        });
        float priority = 1.0f;
        createDeviceAbstraction(queueFamilyIndex,"main",priority);
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
    uint32_t graphicProcessor::getSuitableQueueFamily(VkInstance &mainInstance,
                                                      std::function<bool(const VkQueueFamilyProperties & fp)> checks) {
        uint32_t families = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_mainDevice, &families, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(families);
        vkGetPhysicalDeviceQueueFamilyProperties(m_mainDevice, &families, queueFamilies.data());

        for(uint32_t i = 0; i < queueFamilies.size(); i++)
            if(checks(queueFamilies[i]))
                return i;
        throw std::runtime_error("There is not queue family with requested capabilities");
    }

    deviceAbstraction &graphicProcessor::getDeviceAbstraction(std::string name) {
        for (auto &m_mainDeviceAbstraction: m_mainDeviceAbstractions)
            if (m_mainDeviceAbstraction.name == name)
                return m_mainDeviceAbstraction;
        throw std::runtime_error("There is not deviceAbstraction with requested name");
    }

    void graphicProcessor::free() {
        for(const auto& device : m_mainDeviceAbstractions)
            device.free();
        m_mainDeviceAbstractions.clear();
    }

    VkQueue &graphicProcessor::getDeviceQueue(std::string &name) {
        for (auto &m_mainDeviceAbstraction: m_mainDeviceAbstractions)
            if (m_mainDeviceAbstraction.name == name)
                return m_mainDeviceAbstraction.queue;
        throw std::runtime_error("There is not deviceAbstraction with requested name");
    }

    std::vector<std::string> graphicProcessor::getAbstractionsData() {
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

    void graphicProcessor::createDeviceAbstraction(uint32_t queueIndex, const std::string& name, float &priority,std::vector<const char*> gpuExtensions,VkPhysicalDeviceFeatures features) {

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

        deviceAbstraction newDevice(name,VkDevice{},VkQueue{},queueIndex);

        if(vkCreateDevice(m_mainDevice,&deviceCreateInfo, nullptr,&newDevice.device) != VK_SUCCESS) {
            LOG("Error creating a virtual device",999,-1)
            exit(1);
        }
        m_mainDeviceAbstractions.push_back(newDevice);
    }

    const VkPhysicalDevice &graphicProcessor::getPhysicalDevice() {
        return m_mainDevice;
    }

    std::vector<uint32_t> graphicProcessor::getAllUsedFamilies() {
        std::vector<uint32_t> indices;
        for(const deviceAbstraction &abstraction : m_mainDeviceAbstractions)
            indices.push_back(abstraction.family);

        sort( indices.begin(), indices.end() );
        indices.erase( unique( indices.begin(), indices.end() ), indices.end() );
        return indices;
    }

    std::vector<VkExtensionProperties> graphicProcessor::getDeviceExtensions() {

        std::vector<VkExtensionProperties> pts;
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(m_mainDevice, nullptr,&extensionCount, nullptr);
        pts.resize(extensionCount);

        vkEnumerateDeviceExtensionProperties(m_mainDevice, nullptr,&extensionCount, pts.data());
        return pts;
    }

}