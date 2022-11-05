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
                    const std::vector<std::string> &layers,const std::vector<std::string> &extensions) : m_instance(appName,appVersion,apiVersion,layers,extensions), DEBUG(true),
                                                              m_window() {


        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = window({800,600},60);
        m_window.createSurface(m_instance.getInstance());


        m_gpu = std::make_unique<graphicProcessor>(graphicProcessor(m_instance.getInstance(),{IsGood}));
    }

    int App::mainLoop() {

        /*

        //Extensions
        std::cout<<"Available Extensions\n";
        auto extensions = m_instance.getAvailableExtensions();
        for(auto o : extensions)
            std::cout<<'\t'<<o.extensionName<<'\n';

        //Abstractions
        auto abstractions = m_gpu->getAbstractionsData();
        for(const auto& o : abstractions )
            std::cout<<o<<'\n';
        //Layers
        std::cout<<"\nAvailable layers\n";
        auto layers = m_instance.getAvailableLayers();
        for(auto o : layers)
            std::cout<<'\t'<<o.layerName<<'\n';

        */

        while (!glfwWindowShouldClose(m_window.mainWindow)) {



            glfwPollEvents();
        }
        return 0;
    }

    void App::clean() {
        m_gpu->free();
        m_window.free(m_instance.getInstance());
        glfwTerminate();
    }
}