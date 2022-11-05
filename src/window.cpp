#include "window.h"

namespace craft{
    window::window(glm::ivec2 windowSize, uint32_t refreshRate) : m_refreshRate(refreshRate) , m_windowSize(windowSize), surface(){

        glfwWindowHint(GLFW_REFRESH_RATE,60);
        mainWindow = glfwCreateWindow(windowSize.x, windowSize.y, "Craft", nullptr, nullptr);

        if(mainWindow == nullptr)
            throw std::runtime_error("Fail creating window, can be related to extensions errors");

    }

    void window::free(const VkInstance& instance) const {
        glfwDestroyWindow(mainWindow);
        vkDestroySurfaceKHR(instance,surface, nullptr);
    }

    window::window() : m_refreshRate(60) {}

    void window::createSurface(VkInstance &instance) {
        if(glfwCreateWindowSurface(instance,mainWindow, nullptr,&surface) != VK_SUCCESS){
            throw std::runtime_error("Error creating windowSurface");
        }
    }

    void window::setWindowName(const std::string &newName) const {
        glfwSetWindowTitle(mainWindow, newName.c_str());
    }

    void window::setWindowSize(const glm::ivec2 &newSize) {
        m_windowSize = newSize;
        glfwSetWindowSize(mainWindow,m_windowSize.x,m_windowSize.y);
    }

    glm::ivec2 window::getWindowSize() const {
        return m_windowSize;
    }

    uint32_t window::getRefreshRate() const {
        return m_refreshRate;
    }

}