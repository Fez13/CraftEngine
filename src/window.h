#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

#include "glm.hpp"

namespace craft{

    struct window{

        GLFWwindow *mainWindow;

        VkSurfaceKHR surface;

        explicit window(glm::ivec2 windowSize, uint32_t refreshRate);

        explicit window();

        void createSurface(VkInstance& instance);

        void setWindowName(const std::string &newName) const;

        void setWindowSize(const glm::ivec2 &newSize);

        glm::ivec2 getWindowSize() const;

        uint32_t getRefreshRate() const;


        void free(const VkInstance& instance) const;
    private:

        uint32_t m_refreshRate;

        glm::ivec2 m_windowSize;
    };

}
