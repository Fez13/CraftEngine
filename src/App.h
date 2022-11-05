#pragma once

#include <iostream>
#include <vector>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glm.hpp"

#include "utils.hpp"
#include "vk_instance.h"
#include "vk_graphic_device.h"
#include "window.h"

namespace craft{


    class App {

    public:

        App(const char* appName, uint32_t appVersion, uint32_t apiVersion, const std::vector<std::string>& layers,const std::vector<std::string> &extensions);

        int mainLoop();

        void clean();

        void setDebug(bool state){DEBUG = state;}

    private:

        bool DEBUG;

        std::unique_ptr<graphicProcessor> m_gpu;

        window m_window;

        vk_instance m_instance;

    };


}

