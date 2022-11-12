#include "vk_instance.h"
#include "utils.hpp"
#include "debug/log.h"

namespace craft{

    craft::vk_instance::vk_instance(const char *appName, uint32_t appVersion, uint32_t apiVersion,
                                    const std::vector<std::string>& layers = {},const std::vector<std::string>& extensions = {}) : m_instance() {

        VkApplicationInfo AppInfo{};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = appName;
        AppInfo.applicationVersion = appVersion;
        AppInfo.apiVersion = apiVersion;
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.pEngineName = "Craft";


        VkInstanceCreateInfo InstanceCreateInfo{};
        InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.flags = VkInstanceCreateFlags();
        InstanceCreateInfo.pApplicationInfo = &AppInfo;


        //setValidationLayers({"VK_LAYER_KHRONOS_validation"},  InstanceCreateInfo);
        //setExtensions(extensions,&InstanceCreateInfo);

        ////////////////////
        //I made voids for setting extensions and layers, but for some reason, they don't want to work
        //TODO: And i should probably make it work

        //Extensions
        auto availableExtension = getAvailableExtensions();
        std::vector<const char*> pp_extensions;

        for(const auto& extension : extensions){
            if(!checkExtension(availableExtension,extension)){
                LOG(std::string("Couldn't find extension... \n\tExtension name: ") + extension,0,0)
                continue;
            }
            pp_extensions.push_back(extension.c_str());
        }

        InstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(pp_extensions.size());
        InstanceCreateInfo.ppEnabledExtensionNames = pp_extensions.data();

        //Layers
        auto availableLayers = getAvailableLayers();
        std::vector<const char*> pp_layers;

        for (const auto & layer : layers) {
            if (!checkLayer(availableLayers, layer)){
                LOG(std::string("Couldn't find layer... \n\tLayer name: ") + layer,0,0)
                continue;
            }
            pp_layers.push_back(layer.c_str());
        }
        InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(pp_layers.size());
        InstanceCreateInfo.ppEnabledLayerNames = pp_layers.data();

        ////////////////////

        if(vkCreateInstance(&InstanceCreateInfo, nullptr,&m_instance) != VK_SUCCESS)
            throw std::runtime_error("ERROR CREATING INSTANCE");
    }

    VkInstance &craft::vk_instance::getInstance() {
        return m_instance;
    }

    craft::vk_instance::~vk_instance() {
        vkDestroyInstance(m_instance, nullptr);
    }

    std::vector<VkExtensionProperties> craft::vk_instance::getAvailableExtensions() {

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount, extensions.data());

        return extensions;
    }

    std::vector<VkLayerProperties> craft::vk_instance::getAvailableLayers() {

        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> layers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

        return layers;
    }

    int vk_instance::setExtensions(const std::vector<std::string>& extensions, VkInstanceCreateInfo *appInfo) {

        auto availableExtension = getAvailableExtensions();
        std::vector<const char*> pp_extensions;

        for(const auto& extension : extensions){
            if(!checkExtension(availableExtension,extension)){
                LOG(std::string("Couldn't find extension... \n\tExtension name: ") + extension,0,0)
                continue;
            }
            pp_extensions.push_back(extension.c_str());
        }

        appInfo->enabledExtensionCount = static_cast<uint32_t>(pp_extensions.size());
        appInfo->ppEnabledExtensionNames = pp_extensions.data();
        return 0;
    }

    int craft::vk_instance::setValidationLayers(const std::vector<std::string>& layers,VkInstanceCreateInfo &appInfo) {

        auto availableLayers = getAvailableLayers();
        std::vector<const char*> pp_layers;

        for (const auto & layer : layers) {
            if (!checkLayer(availableLayers, layer)){
                LOG( std::string("Couldn't find layer... \n\tLayer name: ") + layer << '\n',0,0)
                continue;
            }
            pp_layers.push_back(layer.c_str());
        }
        appInfo.enabledLayerCount = static_cast<uint32_t>(pp_layers.size());
        appInfo.ppEnabledLayerNames = pp_layers.data();
        return 0;
    }

    bool craft::vk_instance::checkLayer(const std::vector<VkLayerProperties> &availableLayers , const std::string& layer) {

        for(const auto & availableLayer : availableLayers)
            if((std::string)availableLayer.layerName == layer)
                return true;
        return false;
    }

    bool vk_instance::checkExtension(const std::vector<VkExtensionProperties> &availableExtensions,
                                     const std::string &extension) {
        for(const auto & availableExtension : availableExtensions)
            if((std::string)availableExtension.extensionName == extension)
                return true;
        return false;
    }
}