#include "App.h"

namespace craft{

    bool IsGood(VkPhysicalDeviceProperties& pt,VkPhysicalDeviceFeatures& ft){

        if(pt.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return false;
        if(!ft.geometryShader)
            return false;
        return true;
    }

    void mouse_callback(GLFWwindow* window, double xpos, double ypos){

    }

    App::App(const char *appName, uint32_t appVersion, uint32_t apiVersion,
                    const std::vector<std::string> &layers,const std::vector<std::string> &extensions){

        vk_instance::get().initialize(appName,appVersion,apiVersion,layers,extensions);

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


        m_window = vk_window({1200, 675}, 60);
        input::get().setMainWindow(m_window.mainWindow);

        glfwSetCursorPosCallback(m_window.mainWindow, mouse_callback);

        m_window.createSurface();

        vk_graphic_device::get().initialize({IsGood});

        m_window.createSwapChainProperties(vk_graphic_device::get().getPhysicalDevice());

        float priority = 1.0f;
        vk_graphic_device::get().createDeviceAbstraction(m_window.findQueueFamily(vk_graphic_device::get().getPhysicalDevice()), "QUEUE_KHR", priority,{VK_KHR_SWAPCHAIN_EXTENSION_NAME});

        std::string vert = "../src/shaders/compiled/vert.spv";

        std::string frag = "../src/shaders/compiled/frag.spv";
        compile("../src/shaders/verts/basic_vert.vert", EMPTY, vert, true);
        compile("../src/shaders/frags/basic_frag.frag", EMPTY, frag, true);

        vk_renderer::get().initialize(&vk_graphic_device::get().getDeviceAbstraction("QUEUE_KHR"));
        vk_renderer::get().loadShaders(vert.c_str(),frag.c_str());
        vk_renderer::get().setMainWindow(&m_window);
        vk_renderer::get().setDynamicStates({
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        });
        vk_renderer::get().createShaderPipeline();
        m_mainLoopFrameTime = frameRate();
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


        /*
        std::vector<vertex> vertices{
                {{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f,1}},
                {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f,1}},
                {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f,1}},
                {{-0.5f,  0.5f, 0.0}, {1.0f, 1.0f, 1.0f,1}}
        };

        std::vector<uint32_t> indices{
                0, 1, 3,
                1, 2, 3
        };
        */


        std::vector<vertex> vertices{
                {{-1, -1, 0.5f}, {1.0f, 0.0f, 0.0f,1}}, //0
                {{1, -1, 0.5f}, {0.0f, 1.0f, 0.0f,1}}, //1
                {{-1, 1, 0.5f}, {0.0f, 0.0f, 1.0f,1}}, //2
                {{1, 1, 0.5f},{1.0f, 1.0f, 1.0f,1}}, //3
                {{-1, -1, -0.5f},{1.0f, 0.0f, 0.0f,1}}, //4input::GetKeyDown(
                {{1, -1, -0.5f},{0.0f, 1.0f, 0.0f,1}}, //5
                {{-1, 1, -0.5f},{1.0f, 0.0f, 0.0f,1}}, //6
                {{ 1, 1, -0.5f }, {1.0f, 0.0f, 0.0f,1}}  //7

        };

        std::vector<uint32_t> indices{
                //Top
                2, 6, 7,
                2, 3, 7,

                //Bottom
                0, 4, 5,
                0, 1, 5,

                //Left
                0, 2, 6,
                0, 4, 6,

                //Right
                1, 3, 7,
                1, 5, 7,

                //Front
                0, 2, 3,
                0, 1, 3,

                //Back
                4, 6, 7,
                4, 5, 7
        };


        geometry geo(vertices,indices);

        mesh m("QUEUE_KHR",geo);


        vk_renderer::get().meshes.push_back(&m);


        float aspectRatio = (float)m_window.getWindowSize().x / (float)m_window.getWindowSize().y;
        camera_ent *myCamera = new camera_ent(aspectRatio,m_window.mainWindow);
        myCamera->position = glm::vec3(0.0f, 0.0f, 0.0f);
        vk_renderer::get().setMainCamera(static_cast<craft::camera*>(myCamera));

        while (!glfwWindowShouldClose(m_window.mainWindow)) {
            static double x = 0;
            x++;
            m_mainLoopFrameTime.wait();



            if(input::get().getMouseButtonOnce(Mouse::ButtonMiddle)){
                static bool state = false;
                if(state)
                    glfwSetInputMode(m_window.mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                else
                    glfwSetInputMode(m_window.mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                state = !state;
            }

            myCamera->update();
            myCamera->updateCamera();
            m_window.update(vk_graphic_device::get().getDeviceAbstraction("QUEUE_KHR").device);
            vk_renderer::get().updateFrame();
            input::get().poolInputs();
        }
        vk_renderer::get().waitToFinish();

        LOG("Main loop has ended",0,1)
        return 0;
    }

    void App::clean() {
        m_window.free(vk_graphic_device::get().getDeviceAbstraction("QUEUE_KHR").device);
        vk_renderer::get().free();
        vk_graphic_device::get().free();
        glfwTerminate();
    }
}