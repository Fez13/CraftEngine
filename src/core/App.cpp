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
                    const std::vector<std::string> &layers,const std::vector<std::string> &extensions) : m_instance(appName,appVersion,apiVersion,layers,extensions){


        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = vk_window({1200, 675}, 60);
        m_window.createSurface(m_instance.getInstance());


        m_gpu = vk_graphic_device(m_instance.getInstance(), {IsGood});

        m_window.createSwapChainProperties(m_gpu.getPhysicalDevice());

        float priority = 1.0f;
        m_gpu.createDeviceAbstraction(m_window.findQueueFamily(m_gpu.getPhysicalDevice()), "QUEUE_KHR", priority,{VK_KHR_SWAPCHAIN_EXTENSION_NAME});

        std::string vert = "../src/shaders/compiled/vert.spv";

        std::string frag = "../src/shaders/compiled/frag.spv";
        compile("../src/shaders/verts/basic_vert.vert", EMPTY, vert, true);
        compile("../src/shaders/frags/basic_frag.frag", EMPTY, frag, true);

        m_rendered = vk_renderer(&m_gpu.getDeviceAbstraction("QUEUE_KHR"));
        m_rendered.loadShaders(vert.c_str(),frag.c_str());
        m_rendered.setMainWindow(&m_window);
        m_rendered.setMainGpu(&m_gpu);
        m_rendered.setDynamicStates({
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        });
        m_rendered.createShaderPipeline();
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
        auto deviceExtensions = m_gpu.getDeviceExtensions();
        for(VkExtensionProperties pt : deviceExtensions)
            std::cout<<"\t"<<pt.extensionName<<" Version: " <<pt.specVersion<<'\n';
        m_gpu.getDeviceExtensions();
        */


        std::vector<vertex> vertices{
                {{0,-0.5f,0},{1.0f,0,0,1}},
                {{0.5f,0.5f,0},{0,1.0f,0,1}},
                {{-0.5f,0.5f,0},{0,0,1.0f,1}}
        };

        vk_buffer buffer(m_gpu.getDeviceAbstraction("QUEUE_KHR"),sizeof(vertex) * vertices.size(),VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_SHARING_MODE_EXCLUSIVE);
        buffer.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,m_gpu.getPhysicalDevice());
        buffer.allocateMemory();

        auto* gpu_vertices = buffer.getMemoryLocation<vertex>();

        for(uint32_t i = 0; i < vertices.size(); i++)
            gpu_vertices[i] = vertices[i];

        buffer.unMapMemory();
        buffer.bindBuffer();

        vk_buffer buffer_gpu(m_gpu.getDeviceAbstraction("QUEUE_KHR"),sizeof(vertex) * vertices.size(),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ,VK_SHARING_MODE_EXCLUSIVE);
        buffer_gpu.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,m_gpu.getPhysicalDevice());
        buffer_gpu.allocateMemory();
        buffer_gpu.bindBuffer();

        buffer_gpu<<buffer;

        m_rendered.draw_temporal(buffer_gpu);


        while (!glfwWindowShouldClose(m_window.mainWindow)) {
            static uint32_t i = 0;
            i++;

            m_rendered.setClearColor({std::sin(i / 1000.0f),std::sin((i/ 1000.0f) * 2),std::sin((i/ 1000.0f) * 3),1});


            m_window.update(m_gpu.getDeviceAbstraction("QUEUE_KHR").device);


            m_rendered.updateFrame();
            glfwPollEvents();
        }
        m_rendered.waitToFinish();

        //Temporal
        buffer.free();
        buffer_gpu.free();


        LOG("Main loop has ended",0,1)
        return 0;
    }

    void App::clean() {
        m_window.free(m_instance.getInstance(),m_gpu.getDeviceAbstraction("QUEUE_KHR").device);
        m_rendered.free();
        m_gpu.free();
        glfwTerminate();
    }
}