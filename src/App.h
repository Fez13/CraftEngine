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

namespace craft{

    class App {

    public:

        App(const char* appName, uint32_t appVersion, uint32_t apiVersion, const std::vector<std::string>& layers);

        glm::ivec2 GetWindowSize() const;

        void SetWindowSize(glm::ivec2 newSize);

        int mainLoop();

        void clean();

        void setDebug(bool state){DEBUG = state;}

    private:

        bool DEBUG;

        std::unique_ptr<graphicProcessor> m_gpu;

        glm::ivec2 m_windowSize{};

        vk_instance m_instance;

        GLFWwindow *m_window;

    };


}

