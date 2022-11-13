#include "App.h"

#include "debug/time.h"

namespace craft{

    bool IsGood(VkPhysicalDeviceProperties& pt,VkPhysicalDeviceFeatures& ft){

        if(pt.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return false;
        if(!ft.geometryShader)
            return false;


        return true;
    }

    App::App(const char *appName, uint32_t appVersion, uint32_t apiVersion,
                    const std::vector<std::string> &layers,const std::vector<std::string> &extensions) : m_instance(appName,appVersion,apiVersion,layers,extensions){


        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = vk_window({800, 600}, 60);
        m_window.createSurface(m_instance.getInstance());


        m_gpu = std::make_unique<graphicProcessor>(graphicProcessor(m_instance.getInstance(),{IsGood}));

        m_window.createSwapChainProperties(m_gpu->getPhysicalDevice());

        float priority = 1.0f;
        m_gpu->createDeviceAbstraction(m_window.findQueueFamily(m_gpu->getPhysicalDevice()), "QUEUE_KHR", priority,{VK_KHR_SWAPCHAIN_EXTENSION_NAME});

        m_window.createSwapChain(m_gpu->getDeviceAbstraction("QUEUE_KHR").device,m_gpu->getAllUsedFamilies());

        std::string vert = "../src/shaders/compiled/vert.spv";
        std::string frag = "../src/shaders/compiled/frag.spv";

        compile("../src/shaders/verts/basic_vert.vert", EMPTY, vert, true);
        compile("../src/shaders/frags/basic_frag.frag", EMPTY, frag, true);

        m_rendered = vk_renderer(&m_gpu->getDeviceAbstraction("QUEUE_KHR"));
        m_rendered.loadShaders(vert.c_str(),frag.c_str());
        m_rendered.setMainWindow(&m_window);
        m_rendered.setDynamicStates({
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        });
        m_rendered.createShaderPipeline();
        m_window.createFrameBuffers(m_gpu->getDeviceAbstraction("QUEUE_KHR").device,m_rendered.getRenderPass());


    }

    int App::mainLoop() {

        LOG("Main loop has started",0,1)
        /*

        //Extensions
        LOG("Available Extensions",0,1)
        auto extensions = m_instance.getAvailableExtensions();
        for(auto o : extensions)
            std::cout<<'\t'<<o.extensionName<<'\n';

        //Layers
        LOG("Available layers\n",0,1)
        auto layers = m_instance.getAvailableLayers();
        for(auto o : layers)
            std::cout<<'\t'<<o.layerName<<'\n';

        //Device Extensions

        LOG("Device Extensions\n",0,1)
        auto deviceExtensions = m_gpu->getDeviceExtensions();
        for(VkExtensionProperties pt : deviceExtensions)
            std::cout<<"\t"<<pt.extensionName<<" Version: " <<pt.specVersion<<'\n';
        m_gpu->getDeviceExtensions();
        */

        while (!glfwWindowShouldClose(m_window.mainWindow)) {


            m_window.update(m_gpu->getDeviceAbstraction("QUEUE_KHR").device);
            glfwPollEvents();
        }
        return 0;
    }

    void App::clean() {
        m_rendered.free();
        m_window.free(m_instance.getInstance(),m_gpu->getDeviceAbstraction("QUEUE_KHR").device);
        m_gpu->free();
        glfwTerminate();
    }
}