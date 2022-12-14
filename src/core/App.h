#pragma once


#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtx/transform.hpp"

//#define RELEASE
#define LOG_COMPONENT_DESTRUCTION

#include "../utils/utils.hpp"
#include "../utils/frameRate.h"
#include "../debug/log.h"
#include "../debug/time.h"

#include "vk_instance.h"
#include "../gpu/vk_graphic_device.h"
#include "../rendered/vk_window.h"
#include "../shaders/compile_Spir_V.h"
#include "../rendered/vk_renderer.h"
#include "../rendered/mesh.h"
#include "../gpu/vk_buffer.h"
#include "../rendered/camera.h"
#include "../input/input.h"
#include "../gpu/texture.h"

namespace craft{


    class App {

    public:

        App(const char* appName, uint32_t appVersion, uint32_t apiVersion, const std::vector<std::string>& layers,const std::vector<std::string> &extensions);

        int mainLoop();

        void clean();

        void setMainLoopFramerate(float newFrameRate);

    private:

        void defaultInputs();

        float m_refreshRate = 60;

        frameRate m_mainLoopFrameTime;

        vk_window m_window;

    };

}

