#include "vk_window.h"

namespace craft{

    vk_window* pMainWindow;

    void createImageView(VkImageViewType viewType, VkFormat format,VkDevice device, VkImageView& imageView, VkImage& image, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT
                , uint32_t baseMinp = 0, uint32_t baseArray = 0, uint32_t levelCount = 1, uint32_t layerCount = 1){
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.viewType = viewType;
        info.image = image;
        info.format = format;

        //TODO: Maybe this could be customizable in the future
        info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        info.subresourceRange.aspectMask = aspectFlags;
        info.subresourceRange.baseMipLevel = baseMinp;
        info.subresourceRange.levelCount = levelCount;
        info.subresourceRange.baseArrayLayer = baseArray;
        info.subresourceRange.layerCount = layerCount;


        if (vkCreateImageView(device, &info, nullptr, &imageView) != VK_SUCCESS){
            LOG_TERMINAL("Fail creating a image view...",999)
        }
    }

    //I think this should be a type of buffer, but I am not sure if it fits the definition
    void createImage(glm::ivec2 size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image
                     , VkDeviceMemory& imageMemory, VkDevice &device,VkPhysicalDevice gpu){
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size.x;
        imageInfo.extent.height = size.y;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            LOG_TERMINAL("Fail creating a image...",999)
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);

        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(gpu,&deviceMemoryProperties);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemory(properties,deviceMemoryProperties, memRequirements.memoryTypeBits);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
            LOG_TERMINAL("Fail creating an image memory...",999)
        }
        vkBindImageMemory(device, image, imageMemory, 0);
    }


    vk_window::vk_window(glm::ivec2 windowSize, uint32_t refreshRate) : m_refreshRate(refreshRate) , m_windowSize(windowSize), surface(){

        glfwWindowHint(GLFW_REFRESH_RATE,60);

        //Todo: Should be RESIZABLE
        glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
        mainWindow = glfwCreateWindow(windowSize.x, windowSize.y, "Craft", nullptr, nullptr);

        if(mainWindow == nullptr){
            LOG_TERMINAL("Fail creating vk_window, can be related to extensions errors",999);
        }
        pMainWindow = this;
    }

    void vk_window::free(const VkInstance& instance, const VkDevice &device) const {
        cleanFrameBuffers(device);
        glfwDestroyWindow(mainWindow);
        vkDestroySwapchainKHR(device,m_swapChain.swapChainKhr, nullptr);
        vkDestroySurfaceKHR(instance,surface, nullptr);
    }

    vk_window::vk_window() : m_refreshRate(60) {}

    void vk_window::createSurface(VkInstance &instance) {
        if(glfwCreateWindowSurface(instance,mainWindow, nullptr,&surface) != VK_SUCCESS){
           LOG_TERMINAL("Error creating windowSurface",999);
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
        if(!m_swapChainData.populated){
            LOG_TERMINAL("You can´t find a queue family without calling createSwapChainProperties before",5)
        }
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

        LOG_TERMINAL("There is not queue family with requested capabilities",999)
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
        m_mainDevice = device;
        if(m_queueFamily == -1){
            LOG_TERMINAL("'createSwapChain' must be call after finding a queue",999)
        }
        if(queueFamilies.empty()){
            LOG_TERMINAL("There are not available queueFamilies",999)
        }
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

        VkSwapchainCreateInfoKHR swapChainInfo = {};
        swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainInfo.surface = surface;
        swapChainInfo.minImageCount = m_swapChainData.imagesInSwapChain;
        swapChainInfo.imageFormat = m_swapChain.format.format;
        swapChainInfo.imageColorSpace = m_swapChain.format.colorSpace;
        swapChainInfo.imageExtent = m_swapChain.extent;
        swapChainInfo.imageArrayLayers = 1;
        swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if(queueFamilies.size() != 1){
            m_swapChainInfoStatic.shm = VK_SHARING_MODE_CONCURRENT;
            m_swapChainInfoStatic.queueFamilyIndexCount = 2;
            m_swapChainInfoStatic.queueFamilyIndices = queueFamilies.data();
        }
        else{
            m_swapChainInfoStatic.shm = VK_SHARING_MODE_EXCLUSIVE;
            m_swapChainInfoStatic.queueFamilyIndexCount = 0;
            m_swapChainInfoStatic.queueFamilyIndices = nullptr;
        }

        swapChainInfo.preTransform = m_swapChainData.capabilities.currentTransform;
        swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainInfo.presentMode = m_swapChain.presentMode;
        swapChainInfo.clipped = VK_TRUE;
        swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

        createSwapChain(device);

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



    void vk_window::createFrameBuffers(VkDevice const &device,VkImageView depthImage) {
        m_swapChain.frameBuffers.resize(m_swapChain.imagesViews.size());

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.width = getExtent().width;
        framebufferInfo.height = getExtent().height;
        framebufferInfo.layers = 1;

        for (size_t i = 0; i < m_swapChain.imagesViews.size(); i++) {
            VkImageView attachments[2] = {
                    m_swapChain.imagesViews[i],
                    depthImage
            };
            framebufferInfo.pAttachments = attachments;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_swapChain.frameBuffers[i]) != VK_SUCCESS){
               LOG_TERMINAL("Error creating frame buffers...",999)
            }

        }

    }

    VkFramebuffer vk_window::getFrameBuffer(uint32_t index) const {
        if(index >= m_swapChain.frameBuffers.size())
        {
            LOG("FrameBuffer request out of array, returning last of the list",1,0)
            return m_swapChain.frameBuffers[m_swapChain.frameBuffers.size()];
        }
        m_swapChain.frameBuffers[index];
        return m_swapChain.frameBuffers[index];
    }

    void vk_window::getNextSwapChainImage(uint32_t &index, VkSemaphore finish, VkDevice device) {
        vkAcquireNextImageKHR(device,m_swapChain.swapChainKhr, UINT64_MAX, finish, VK_NULL_HANDLE, &index);
    }

    VkPresentInfoKHR vk_window::getSubmitImageInfo(uint32_t &index, VkSemaphore wait[]) {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = wait;
        m_swapChain.swapChainKhr_arr = {m_swapChain.swapChainKhr};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapChain.swapChainKhr_arr;
        presentInfo.pImageIndices = &index;
        presentInfo.pResults = nullptr;
        return presentInfo;
    }

    void vk_window::cleanFrameBuffers(const VkDevice &mainDevice) const{
        for(const auto& fb : m_swapChain.frameBuffers)
            vkDestroyFramebuffer(mainDevice,fb, nullptr);

        for(const auto& iv : m_swapChain.imagesViews)
            vkDestroyImageView(mainDevice, iv, nullptr);
    }

    void vk_window::setWindowSizeNoUpdate(glm::ivec2 newSize) {
        m_windowSize = newSize;
        m_swapChain.extent = m_swapChainData.capabilities.currentExtent;
    }

    void vk_window::createSwapChain(const VkDevice device) {
        VkSwapchainCreateInfoKHR swapChainInfo = {};
        swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainInfo.surface = surface;
        swapChainInfo.minImageCount = m_swapChainData.imagesInSwapChain;
        swapChainInfo.imageFormat = m_swapChain.format.format;
        swapChainInfo.imageColorSpace = m_swapChain.format.colorSpace;
        swapChainInfo.imageExtent = m_swapChain.extent;
        swapChainInfo.imageArrayLayers = 1;
        swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainInfo.preTransform = m_swapChainData.capabilities.currentTransform;
        swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainInfo.presentMode = m_swapChain.presentMode;
        swapChainInfo.clipped = VK_TRUE;
        swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
        swapChainInfo.imageSharingMode = m_swapChainInfoStatic.shm;
        swapChainInfo.queueFamilyIndexCount = m_swapChainInfoStatic.queueFamilyIndexCount;
        swapChainInfo.pQueueFamilyIndices = m_swapChainInfoStatic.queueFamilyIndices;

        if (vkCreateSwapchainKHR(device, &swapChainInfo, nullptr, &m_swapChain.swapChainKhr) != VK_SUCCESS){
            LOG_TERMINAL("Fail creating the swapChain",999)
        }

    }

    void vk_window::setRenderPass(VkRenderPass newRenderPass) {
        m_renderPass = newRenderPass;
    }

    void vk_window::SwapChain::UpdateImages(const VkDevice& device) {

        uint32_t currentImageCount = 0;
        vkGetSwapchainImagesKHR(device,swapChainKhr,&currentImageCount, nullptr);

        images.resize(currentImageCount);
        vkGetSwapchainImagesKHR(device,swapChainKhr,&currentImageCount,images.data());

        if(currentImageCount != images.size())
            LOG("WARNING, the n of images in the swap-chain is not the expected\n\t Expected: " + std::to_string(images.size()) + ", Received: " + std::to_string(currentImageCount),2,0)
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

        for(uint32_t i = 0; i<imagesViews.size(); i++)
            createImageView(VK_IMAGE_VIEW_TYPE_2D,format.format,device,imagesViews[i], images[i]);
    }
}