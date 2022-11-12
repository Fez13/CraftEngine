#include "vk_window.h"

namespace craft{
    vk_window::vk_window(glm::ivec2 windowSize, uint32_t refreshRate) : m_refreshRate(refreshRate) , m_windowSize(windowSize), surface(){

        glfwWindowHint(GLFW_REFRESH_RATE,60);
        glfwWindowHint(GLFW_RESIZABLE,GLFW_TRUE);
        mainWindow = glfwCreateWindow(windowSize.x, windowSize.y, "Craft", nullptr, nullptr);

        if(mainWindow == nullptr)
            throw std::runtime_error("Fail creating vk_window, can be related to extensions errors");

    }

    void vk_window::free(const VkInstance& instance, const VkDevice &device) const {

        for(const auto& iv : m_swapChain.imagesViews)
            vkDestroyImageView(device, iv, nullptr);

        glfwDestroyWindow(mainWindow);
        vkDestroySwapchainKHR(device,m_swapChain.swapChainKhr, nullptr);
        vkDestroySurfaceKHR(instance,surface, nullptr);
    }

    vk_window::vk_window() : m_refreshRate(60) {}

    void vk_window::createSurface(VkInstance &instance) {
        if(glfwCreateWindowSurface(instance,mainWindow, nullptr,&surface) != VK_SUCCESS){
            throw std::runtime_error("Error creating windowSurface");
        }
    }

    void vk_window::setWindowName(const std::string &newName) const {
        glfwSetWindowTitle(mainWindow, newName.c_str());
    }

    void vk_window::setWindowSize(const glm::ivec2 &newSize) {
        m_windowSize = newSize;
        glfwSetWindowSize(mainWindow,m_windowSize.x,m_windowSize.y);
    }

    glm::ivec2 vk_window::getWindowSize() const {
        return m_windowSize;
    }

    uint32_t vk_window::getRefreshRate() const {
        return m_refreshRate;
    }

    uint32_t vk_window::findQueueFamily(VkPhysicalDevice mainDevice) const {
        if(!m_swapChainData.populated)
            throw std::runtime_error("You can´t find a queue family without calling createSwapChainProperties before");

        uint32_t families = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mainDevice, &families, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(families);
        vkGetPhysicalDeviceQueueFamilyProperties(mainDevice, &families, queueFamilies.data());

        for(uint32_t i = 0; i < queueFamilies.size(); i++){
            VkBool32 presentSupport = false;

            if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                vkGetPhysicalDeviceSurfaceSupportKHR(mainDevice, i, surface, &presentSupport);

            if(presentSupport == false)
                continue;

            if(m_swapChainData.formats.empty() || m_swapChainData.presentModes.empty())
                continue;

            m_queueFamily = i;
            return i;
        }

        throw std::runtime_error("There is not queue family with requested capabilities");
    }

    void vk_window::createSwapChainProperties(VkPhysicalDevice const &device) const {

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &m_swapChainData.capabilities);

        uint32_t formats = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats, nullptr);

        m_swapChainData.formats.resize(formats);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats, m_swapChainData.formats.data());

        uint32_t mode = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode, nullptr);

        m_swapChainData.presentModes.resize(mode);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode, m_swapChainData.presentModes.data());

        m_swapChainData.populated = true;
    }

    void vk_window::createSwapChain(const VkDevice &device, const std::vector<uint32_t>& queueFamilies, uint32_t swapChainImageCount ) {
        if(m_queueFamily == -1)
            throw std::runtime_error("'createSwapChain' must be call after finding a queue");

        if(queueFamilies.empty())
            throw std::runtime_error("There are not available queueFamilies");

        //TODO:Shit logged
        //Chose SwapChain characteristics
        bool didFind[3] = {false, false, false};

        for (const auto& format : m_swapChainData.formats)
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
                m_swapChain.format = format;
                didFind[0] = true;
            }

        if(!didFind[0]){
            m_swapChain.format = m_swapChainData.formats[0];
            LOG("Not optimal format has been chose",0,0)
        }
        for (const auto& presentMode : m_swapChainData.presentModes)
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                m_swapChain.presentMode = presentMode;
                didFind[1] = true;
            }

        if(!didFind[1]){
            m_swapChain.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            LOG("Not optimal presentMode has been chose",0,0)
        }

        if(m_swapChainData.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
            m_swapChain.extent = m_swapChainData.capabilities.currentExtent;
            didFind[2] = true;
        }

        //If the screen coordinates and the screen pixels do not match, we have to fix the error
        if(!didFind[2]){
            int x,y;
            glfwGetFramebufferSize(mainWindow,&x,&y);

            LOG("Not optimal extent has been chose",0,0)

            VkExtent2D extent2D{
                static_cast<uint32_t>(x),
                static_cast<uint32_t>(y)
            };

            m_swapChain.extent.width = std::clamp(extent2D.width, m_swapChainData.capabilities.minImageExtent.width, m_swapChainData.capabilities.maxImageExtent.width);
            m_swapChain.extent.height = std::clamp(extent2D.height, m_swapChainData.capabilities.minImageExtent.height, m_swapChainData.capabilities.maxImageExtent.height);
        }

        uint32_t swapChainImageCount_ = swapChainImageCount;

        if(swapChainImageCount == 0)
            swapChainImageCount_ = m_swapChainData.capabilities.minImageCount;
        else if(swapChainImageCount > m_swapChainData.capabilities.maxImageCount && m_swapChainData.capabilities.maxImageCount != 0)
            swapChainImageCount_ = m_swapChainData.capabilities.minImageCount;
        else if(swapChainImageCount < m_swapChainData.capabilities.minImageCount)
            swapChainImageCount_ = m_swapChainData.capabilities.minImageCount;

        m_swapChainData.imagesInSwapChain  = swapChainImageCount_;
        m_swapChain.images.resize(m_swapChainData.imagesInSwapChain);

        VkSwapchainCreateInfoKHR swapchainCreateInfoKhr{};
        swapchainCreateInfoKhr.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfoKhr.surface = surface;
        swapchainCreateInfoKhr.minImageCount = m_swapChainData.imagesInSwapChain;
        swapchainCreateInfoKhr.imageFormat = m_swapChain.format.format;
        swapchainCreateInfoKhr.imageColorSpace = m_swapChain.format.colorSpace;
        swapchainCreateInfoKhr.imageExtent = m_swapChain.extent;
        swapchainCreateInfoKhr.imageArrayLayers = 1;
        swapchainCreateInfoKhr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if(queueFamilies.size() != 1){
            swapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfoKhr.queueFamilyIndexCount = 2;
            swapchainCreateInfoKhr.pQueueFamilyIndices = queueFamilies.data();
        }
        else{
            swapchainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfoKhr.queueFamilyIndexCount = 0; // Optional
            swapchainCreateInfoKhr.pQueueFamilyIndices = nullptr; // Optional
        }

        swapchainCreateInfoKhr.preTransform = m_swapChainData.capabilities.currentTransform;
        swapchainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        swapchainCreateInfoKhr.presentMode = m_swapChain.presentMode;
        swapchainCreateInfoKhr.clipped = VK_TRUE;
        swapchainCreateInfoKhr.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &swapchainCreateInfoKhr, nullptr, &m_swapChain.swapChainKhr) != VK_SUCCESS)
            throw std::runtime_error("Fail creating the swapChain");

        m_swapChain.UpdateImages(device);
        m_swapChain.UpdateImageViews(device);

    }

    void vk_window::update(const VkDevice& device) {
        m_swapChain.UpdateImages(device);
    }

    VkExtent2D vk_window::getExtent() const {
        return m_swapChain.extent;
    }

    VkFormat vk_window::getSwapChainFormat() {
        return m_swapChain.format.format;
    }

    void vk_window::SwapChain::UpdateImages(const VkDevice& device) {

        uint32_t currentImageCount = 0;
        vkGetSwapchainImagesKHR(device,swapChainKhr,&currentImageCount, nullptr);

        images.resize(currentImageCount);
        vkGetSwapchainImagesKHR(device,swapChainKhr,&currentImageCount,images.data());

        //if(currentImageCount != images.size())
        //    std::cout<<"WARNING, the n of images in the swap-chain is not the expected\n\t Expected: "<<images.size()<<" Received: "<<currentImageCount<<'\n';
    }

    void vk_window::SwapChain::UpdateImageViews(const VkDevice& device) {
        imagesViews.resize(images.size());

        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = format.format;

        //TODO: Maybe this could be customizable in the future
        info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;

        for(uint32_t i = 0; i<imagesViews.size(); i++){
            info.image = images[i];
            if (vkCreateImageView(device, &info, nullptr, &imagesViews[i]) != VK_SUCCESS)
                throw std::runtime_error("Fail creating a image view...");

        }

    }
}