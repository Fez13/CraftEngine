#include "App.h"

namespace craft{

    bool IsGood(VkPhysicalDeviceProperties& pt,VkPhysicalDeviceFeatures& ft){

        if(pt.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return false;
        if(!ft.geometryShader)
            return false;

        return true;
    }

    App::App(const char *appName, uint32_t appVersion, uint32_t apiVersion,
                    const std::vector<std::string> &layers) : m_instance(appName,appVersion,apiVersion,layers), m_windowSize({800,600}), DEBUG(true) {


        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(m_windowSize.x, m_windowSize.y, "Craft", nullptr, nullptr);

        m_gpu = std::make_unique<graphicProcessor>(graphicProcessor(m_instance.getInstance(),{IsGood}));

        uint32_t queueFamily = m_gpu->getSuitableQueueFamily([](const VkQueueFamilyProperties & pts){

            if(pts.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                return true;
            return false;

        });

        std::cout<<queueFamily;

    }

    glm::ivec2 App::GetWindowSize() const {
        return m_windowSize;
    }

    void App::SetWindowSize(glm::ivec2 newSize) {
        m_windowSize = newSize;
    }

    int App::mainLoop() {

        /*

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
        */

        while (!glfwWindowShouldClose(m_window)) {



            glfwPollEvents();
        }
        return 0;
    }

    void App::clean() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}