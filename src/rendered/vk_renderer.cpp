#include "vk_renderer.h"


namespace craft{

    vk_renderer* mainRendered;

    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        //mainRendered->waitToFinish();
        //mainRendered->updateWindowSize({width,height});
    }


    vk_renderer::vk_renderer(deviceAbstraction *mainDevice){
        m_mainWindow = nullptr;
        m_pipelineLayout = nullptr;
        m_renderPass = nullptr;
        m_pipeline = nullptr;
        m_mainDevice = mainDevice;
        m_clearColor = {0,0,0,1};
        m_countOfVertices = 0;
        m_verticesOffset = 0;
        mainRendered = this;
    }

    void vk_renderer::setDynamicStates(const std::vector<VkDynamicState> &states) {
        m_dynamicStates = states;
    }


    void vk_renderer::loadShaders(const char *vert_path, const char *frag_path) {
        m_vert = graphics_shader(vert_path,m_mainDevice->device,VK_SHADER_STAGE_VERTEX_BIT);
        m_frag = graphics_shader(frag_path,m_mainDevice->device,VK_SHADER_STAGE_FRAGMENT_BIT);

    }

    void vk_renderer::createShaderPipeline() {

        if(m_mainDevice == nullptr){
            LOG_TERMINAL("Rendered without main device",999)
        }
        if(m_mainWindow == nullptr){
            LOG_TERMINAL("Rendered without main window",999)
        }
        m_usedFamilies = m_mainGpu->getAllUsedFamilies();
        m_mainWindow->createSwapChain(m_mainDevice->device,m_usedFamilies);

        createRenderPass();

        shaderPair pair;
        m_frag.populateShaderPair(pair);
        m_vert.populateShaderPair(pair);


        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
        dynamicStateCreateInfo.pDynamicStates = m_dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo inputStateCreateInfo{};

        auto binding = vertex::getBindingDescription();
        auto attributes = vertex::getAttributeDescription();


        inputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        inputStateCreateInfo.pNext = nullptr;
        inputStateCreateInfo.pVertexAttributeDescriptions = attributes.data();
        inputStateCreateInfo.pVertexBindingDescriptions = &binding;
        inputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
        inputStateCreateInfo.vertexBindingDescriptionCount = 1;


        VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo{};
        assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        //TODO:Should be dynamically modifiable
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_mainWindow->getExtent().width);
        viewport.height = static_cast<float>(m_mainWindow->getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_mainWindow->getExtent();

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.pScissors = &scissor;
        //TODO END

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = m_polygonMode;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

        //May change
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
        rasterizationStateCreateInfo.depthBiasClamp = 0.0f; // Optional
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional

        //Will be enabled in the near future
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
        multisampleStateCreateInfo.pSampleMask = nullptr; // Optional
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        //TODO:Should be modifiable
        colorBlendAttachmentState.blendEnable = VK_TRUE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

        //TODO:Should be modifiable
        colorBlendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
        colorBlendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
        colorBlendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
        colorBlendStateCreateInfo.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0; // Optional
        pipelineLayoutCreateInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(m_mainDevice->device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS){
            LOG_TERMINAL("Fail creating a pipelineLayout",999)
        }
        pair.createDouble();
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = 2;
        graphicsPipelineCreateInfo.pStages = pair.stages;

        graphicsPipelineCreateInfo.pVertexInputState = &inputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &assemblyStateCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pDepthStencilState = nullptr; // Optional
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

        graphicsPipelineCreateInfo.layout = m_pipelineLayout;
        graphicsPipelineCreateInfo.renderPass = m_renderPass;
        graphicsPipelineCreateInfo.subpass = 0;

        //May change
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        graphicsPipelineCreateInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(m_mainDevice->device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_pipeline) != VK_SUCCESS){
            LOG_TERMINAL("Error initializing the pipeline...",999)
        }
        m_mainWindow->setRenderPass(m_renderPass);
        m_mainWindow->createFrameBuffers(m_mainDevice->device);
        m_mainDevice->createFence(VK_FENCE_CREATE_SIGNALED_BIT);
        //WILL CHANGE
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if(vkCreateSemaphore(m_mainDevice->device, &semaphoreCreateInfo, nullptr, &m_waitImage) != VK_SUCCESS){
            LOG_TERMINAL("Fail creating the semaphore...",999)
        }

        if(vkCreateSemaphore(m_mainDevice->device, &semaphoreCreateInfo, nullptr, &m_waitRender) != VK_SUCCESS){
            LOG_TERMINAL("Fail creating the semaphore...",999)
        }

        //Command buffer
        m_mainCommandBuffer = m_mainDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY,1);
    }

    void vk_renderer::setMainWindow(vk_window *mainWindow) {
        m_mainWindow = mainWindow;
        glfwSetWindowSizeCallback(mainWindow->mainWindow , window_size_callback);
    }

    void vk_renderer::free() {
        vkDestroySemaphore(m_mainDevice->device,m_waitRender, nullptr);
        vkDestroySemaphore(m_mainDevice->device,m_waitImage, nullptr);
        vkDestroyPipeline(m_mainDevice->device,m_pipeline, nullptr);
        vkDestroyRenderPass(m_mainDevice->device,m_renderPass, nullptr);
        vkDestroyPipelineLayout(m_mainDevice->device,m_pipelineLayout, nullptr);
        m_vert.free();
        m_frag.free();
    }

    void vk_renderer::setPolygonMode(VkPolygonMode mode) {
        m_polygonMode = mode;
    }

    void vk_renderer::createRenderPass() {
        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = m_mainWindow->getSwapChainFormat();
        //TODO multi-sampling
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        //TODO textures
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference attachmentReference{};
        attachmentReference.attachment = 0;
        attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        //May be useful with the RTX problem
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &attachmentReference;


        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &attachmentDescription;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_mainDevice->device, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS){
            LOG_TERMINAL("Couldn't initialize the render pass...",999)
        }
    }

    VkRenderPass vk_renderer::getRenderPass() const {
        return m_renderPass;
    }

    void vk_renderer::recordCommandBuffer(uint32_t index, VkCommandBuffer &buffer) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS){
            LOG_TERMINAL("Error recording command buffer, index: " + std::to_string(index),999)
        }

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_renderPass;
        renderPassBeginInfo.framebuffer = m_mainWindow->getFrameBuffer(index);
        //Should be dynamic
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = m_mainWindow->getExtent();

        VkClearValue clearColor = {m_clearColor.x,m_clearColor.y,m_clearColor.z,m_clearColor.w};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(buffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

        VkDeviceSize offset[] = {0};

        vkCmdBindVertexBuffers(buffer,0,1,draws.data(),offset);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_mainWindow->getExtent().width);
        viewport.height = static_cast<float>(m_mainWindow->getExtent().height);


        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_mainWindow->getExtent();
        vkCmdSetScissor(buffer, 0, 1, &scissor);

        //TODO:WILL CHANGE, DO NOT FORGET TO REMOVE PLS
        m_countOfVertices = 3;

        vkCmdDraw(buffer, m_countOfVertices, 1, 0, 0);
        vkCmdEndRenderPass(buffer);
        if (vkEndCommandBuffer(buffer) != VK_SUCCESS){
            LOG_TERMINAL("Error recording command buffer",999)
        }
    }

    void vk_renderer::setClearColor(glm::vec4 newColor) {
        m_clearColor = newColor;
    }

    void vk_renderer::updateFrame() {
        vkWaitForFences(m_mainDevice->device, 1, &m_mainDevice->fence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_mainDevice->device, 1, &m_mainDevice->fence);
        uint32_t index;
        m_mainWindow->getNextSwapChainImage(index,m_waitImage,m_mainDevice->device);

        vkResetCommandBuffer(m_mainCommandBuffer, 0);
        recordCommandBuffer(index,m_mainCommandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_waitImage};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_mainCommandBuffer;

        VkSemaphore signalSemaphores[] = {m_waitRender};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_mainDevice->queue[0], 1, &submitInfo, m_mainDevice->fence) != VK_SUCCESS){
            LOG_TERMINAL("Fail submitting work to the gpu",999)
        }

        VkPresentInfoKHR presentInfoKhr = m_mainWindow->getSubmitImageInfo(index,signalSemaphores);
        vkQueuePresentKHR(m_mainDevice->queue[0], &presentInfoKhr);
    }

    void vk_renderer::waitToFinish() const {
        vkDeviceWaitIdle(m_mainDevice->device);
    }

    void vk_renderer::setMainGpu(vk_graphic_device *mainGpu) {
        m_mainGpu = mainGpu;
    }

    void vk_renderer::draw_temporal(vk_buffer &buffer_) {
        draws.push_back(buffer_.getBuffer());
    }

}





