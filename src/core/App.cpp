#include "App.h"


/*

*/

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
                    const std::vector<std::string> &layers,const std::vector<std::string> &extensions) : m_instance(appName,appVersion,apiVersion,layers,extensions){

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


        m_window = vk_window({1200, 675}, 60);
        input::get().setMainWindow(m_window.mainWindow);

        glfwSetCursorPosCallback(m_window.mainWindow, mouse_callback);

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

        mesh m(m_gpu,"QUEUE_KHR",geo);


        m_rendered.meshes.push_back(&m);


        float aspectRatio = (float)m_window.getWindowSize().x / (float)m_window.getWindowSize().y;
        camera_ent *myCamera = new camera_ent(aspectRatio,m_window.mainWindow);
        myCamera->position = glm::vec3(0.0f, 0.0f, 0.0f);

        while (!glfwWindowShouldClose(m_window.mainWindow)) {
            static double x = 0;
            x++;
            m_mainLoopFrameTime.wait();

            // m_rendered.setClearColor({std::sin(i / 1000.0f),std::sin((i/ 1000.0f) * 2),std::sin((i/ 1000.0f) * 3),1});

            m_rendered.setMainCamera(static_cast<craft::camera*>(myCamera));

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
            m_window.update(m_gpu.getDeviceAbstraction("QUEUE_KHR").device);
            m_rendered.updateFrame();
            input::get().poolInputs();
        }
        m_rendered.waitToFinish();

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