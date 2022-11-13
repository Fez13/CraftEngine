#pragma once


#include <iostream>
#include <vector>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

//#define RELEASE
#include "utils.hpp"
#include "debug/log.h"
#include "debug/time.h"

#include "vk_instance.h"
#include "vk_graphic_device.h"
#include "vk_window.h"
#include "shaders/compile_Spir_V.h"
#include "vk_renderer.h"


namespace craft{
    
    class App {

    public:

        App(const char* appName, uint32_t appVersion, uint32_t apiVersion, const std::vector<std::string>& layers,const std::vector<std::string> &extensions);

        int mainLoop();

        void clean();

    private:

        std::unique_ptr<graphicProcessor> m_gpu;

        vk_window m_window;

        vk_instance m_instance;

        vk_renderer m_rendered;

    };
}

