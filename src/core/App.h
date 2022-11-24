#pragma once


#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glm.hpp"

//#define RELEASE
#include "../utils/utils.hpp"
#include "../debug/log.h"
#include "../debug/time.h"

#include "../rendered/vk_instance.h"
#include "../gpu/vk_graphic_device.h"
#include "../rendered/vk_window.h"
#include "../shaders/compile_Spir_V.h"
#include "../rendered/vk_renderer.h"
#include "../gpu/mesh.h"
#include "../gpu/vk_buffer.h"


namespace craft{


    class App {

    public:

        App(const char* appName, uint32_t appVersion, uint32_t apiVersion, const std::vector<std::string>& layers,const std::vector<std::string> &extensions);

        int mainLoop();

        void clean();

    private:

        vk_graphic_device m_gpu;

        vk_window m_window;

        vk_instance m_instance;

        vk_renderer m_rendered;

    };
}

