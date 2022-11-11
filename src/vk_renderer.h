#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glm.hpp"


#include "utils.hpp"
#include "debug/log.h"
#include "vk_window.h"


namespace craft{

    //This shouden't be in the renderer file, it should be in a "Shader class" instead
    VkShaderModule createShaderModule(std::vector<char>, VkDevice&);


    //This is quite of a complicated class since it will not only handle all the basic 3D rendering,
    // it will also take care of ray tracing that will be used to the reflective objects of the scene
    class vk_renderer {
    public:

        explicit vk_renderer(VkDevice mainDevice);
        explicit vk_renderer()= default;

        void loadShaders(const char* vert_path,const char* frag_path);

        void createShaderPipeline();

        void setDynamicStates(const std::vector<VkDynamicState> &states);

        void setMainWindow(vk_window *mainWindow);

        void setPolygonMode(VkPolygonMode);

        void free();

    private:

        vk_window *m_mainWindow;

        std::vector<VkDynamicState> m_dynamicStates;

        VkDevice m_mainDevice;

        std::vector<char> m_frag;
        std::vector<char> m_vert;

        VkShaderModule m_fragModule;
        VkShaderModule m_vertexModule;

        VkPolygonMode m_polygonMode = VK_POLYGON_MODE_FILL;


    };
}


