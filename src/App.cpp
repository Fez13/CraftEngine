#include <cstring>
#include "App.h"


craft::vk_instance::vk_instance(const char *appName, uint32_t appVersion, uint32_t apiVersion,
                                const std::vector<std::string>& layers) : m_instance() {

    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = appName;
    AppInfo.applicationVersion = appVersion;
    AppInfo.apiVersion = appVersion;
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "Craft";


    VkInstanceCreateInfo InstanceCreateInfo{};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.flags = VkInstanceCreateFlags();
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
    setValidationLayers(layers, InstanceCreateInfo);


    uint32_t extensionCount = 0;

    const char **extensions = nullptr;

    glfwGetRequiredInstanceExtensions(&extensionCount);

    InstanceCreateInfo.enabledExtensionCount = extensionCount;
    InstanceCreateInfo.ppEnabledExtensionNames = extensions;

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

int craft::vk_instance::setValidationLayers(const std::vector<std::string>& layers,VkInstanceCreateInfo &appInfo) {
    int result = 0;
    auto availableLayers = getAvailableLayers();
    std::vector<const char*> pp_layers;


    for (const auto & layer : layers) {
        if (!checkLayer(availableLayers, layer)){
            std::cout << std::string("Couldn't find layer... \n\tLayer name: ") + layer << '\n';
            result = -1;
        }
        pp_layers.push_back(layer.c_str());
    }

    appInfo.enabledExtensionCount = static_cast<uint32_t>(pp_layers.size());
    appInfo.ppEnabledExtensionNames = pp_layers.data();
    return 0;
}

bool craft::vk_instance::checkLayer(const std::vector<VkLayerProperties> &availableLayers , const std::string& layer) {

    for(const auto & availableLayer : availableLayers)
        if((std::string)availableLayer.layerName == layer)
            return true;
    return false;
}

craft::App::App(const char *appName, uint32_t appVersion, uint32_t apiVersion,
                const std::vector<std::string> &layers) : m_instance(appName,appVersion,apiVersion,layers), m_windowSize({800,600}) {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_windowSize.x, m_windowSize.y, "Craft", nullptr, nullptr);
}

const glm::ivec2 craft::App::GetWindowSize() const {
    return m_windowSize;
}

void craft::App::SetWindowSize(glm::ivec2 newSize) {
    m_windowSize = newSize;
}

int craft::App::mainLoop() {

    //Extensions
    std::cout<<"Available Extensions\n";
    auto extensions = m_instance.getAvailableExtensions();
    for(auto o : extensions)
        std::cout<<'\t'<<o.extensionName<<'\n';


    //Layers
    std::cout<<"\nAvailable layers\n";
    auto layers = m_instance.getAvailableLayers();
    for(auto o : layers)
        std::cout<<'\t'<<o.layerName<<'\n';


    while (!glfwWindowShouldClose(m_window)) {



        glfwPollEvents();
    }
    return 0;
}

void craft::App::clean() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
