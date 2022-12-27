#include "vk_renderer.h"


namespace craft{

    vk_renderer vk_renderer::s_vk_renderer;

    void vk_renderer::reCreateSwapChain(int width, int height){

        vkDeviceWaitIdle(m_mainDevice->device);
        m_mainWindow->setWindowSize({width,height});
        m_mainWindow->clearImageViews(m_mainDevice->device);
        m_mainWindow->createSwapChainProperties(temporal);
        m_mainWindow->createSwapChain(m_mainDevice->device,m_usedFamilies);
        m_depthImage.free(m_mainDevice->device);
        m_depthImage.create(m_mainWindow,m_mainDevice->device);
        m_mainWindow->reCreateFrameBuffers(m_depthImage.imageView);
        plCreateViewportStateCreateInfo();
        vkDestroyPipelineLayout(m_mainDevice->device,m_pipelineLayout,nullptr);
        vkDestroyPipeline(m_mainDevice->device,m_pipeline,nullptr);
        createPipelineLayout();
        createPipeline();
    }

    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        vk_renderer::get().reCreateSwapChain(width,height);
    }


    void vk_renderer::initialize(deviceAbstraction *mainDevice) {
        m_mainWindow = nullptr;
        m_pipelineLayout = nullptr;
        m_renderPass = nullptr;
        m_pipeline = nullptr;
        m_mainDevice = mainDevice;
        m_clearColor = {0,0,0,1};
        m_countOfVertices = 0;
        m_verticesOffset = 0;
    }

    void vk_renderer::setDynamicStates(const std::vector<VkDynamicState> &states) {
        m_dynamicStates = states;
    }


    void vk_renderer::loadShaders(const char *vert_path, const char *frag_path) {
        m_vert = graphics_shader(vert_path,m_mainDevice->device,VK_SHADER_STAGE_VERTEX_BIT);
        m_frag = graphics_shader(frag_path,m_mainDevice->device,VK_SHADER_STAGE_FRAGMENT_BIT);

    }

    void vk_renderer::depthImage::create(vk_window* window, VkDevice &device){
        format = VK_FORMAT_D32_SFLOAT;
        createImage({window->getExtent().width, window->getExtent().height},  format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    depthImage, memory,device,vk_graphic_device::get().getPhysicalDevice());
        createImageView(VK_IMAGE_VIEW_TYPE_2D, format,device, imageView,depthImage,VK_IMAGE_ASPECT_DEPTH_BIT,0,0,1,1);
    }

    void vk_renderer::createShaderPipeline() {

        if(m_mainDevice == nullptr){
            LOG_TERMINAL("Rendered without main device",999)
        }
        if(m_mainWindow == nullptr){
            LOG_TERMINAL("Rendered without main window",999)
        }
        m_usedFamilies = vk_graphic_device::get().getAllUsedFamilies();
        m_mainWindow->createSwapChain(m_mainDevice->device,m_usedFamilies);


        descriptorSetsLayouts.emplace_back();

        descriptorSetsLayouts[0].addBinding(0);
        descriptorSetsLayouts[0].addBindingImage(1, VK_SHADER_STAGE_FRAGMENT_BIT);
        
        descriptorSetsLayouts[0].InitDescriptorAllocator(m_mainDevice->device);


        //May be convenient to add support to other types of format...
        m_depthImage.create(m_mainWindow,m_mainDevice->device);
        createRenderPass();
        


        m_pair = {};
        m_frag.populateShaderPair(m_pair);
        m_vert.populateShaderPair(m_pair);
        m_pair.createDouble();

        plCreateDynamicStateCreateInfo();

        plCreateVertexInputStateCreateInfo();

        plCreateInputAssemblyStateCreateInfo();

        plCreateViewportStateCreateInfo();

        plCreateRasterizationStateCreateInfo();

        plCreateMultisampleStateCreateInfo();

        plCreateColorBlendStateCreateInfo();
       
        plCreateDepthStencilStateCreateInfo();

        createPipelineLayout();

        createPipeline();

        m_mainWindow->setRenderPass(m_renderPass);
        m_mainWindow->createFrameBuffers(m_mainDevice->device,m_depthImage.imageView);
        m_mainDevice->createFence(VK_FENCE_CREATE_SIGNALED_BIT);


        //DescriptorSets
        descriptorSetsLayouts[0].allocateDescriptorSet(m_mainDevice->device);
        descriptorSetsLayouts[0].updateBufferBinding(m_cameraBuffer,0,m_mainDevice->device);


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
        m_depthImage.free(m_mainDevice->device);
        vkDestroySemaphore(m_mainDevice->device,m_waitRender, nullptr);
        vkDestroySemaphore(m_mainDevice->device,m_waitImage, nullptr);
        vkDestroyPipeline(m_mainDevice->device,m_pipeline, nullptr);
        descriptorSetsLayouts[0].free(m_mainDevice->device);
        vkDestroyRenderPass(m_mainDevice->device,m_renderPass, nullptr);
        vkDestroyPipelineLayout(m_mainDevice->device,m_pipelineLayout, nullptr);
        vkDestroyDescriptorPool(m_mainDevice->device, m_descriptionPool, nullptr);
        m_vert.free();
        m_frag.free();
    }

    void vk_renderer::setPolygonMode(VkPolygonMode mode) {
        m_polygonMode = mode;
    }

    void vk_renderer::createRenderPass() {
        //Color
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


        //Depth
        VkAttachmentDescription depthAttachmentDescription{};
        depthAttachmentDescription.format = m_depthImage.format;
        depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentReference{};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription SubpassDescription{};
        SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        SubpassDescription.colorAttachmentCount = 1;
        SubpassDescription.pColorAttachments = &attachmentReference;
        SubpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

        VkAttachmentDescription descriptions[2] = {attachmentDescription, depthAttachmentDescription};


        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 2;
        renderPassCreateInfo.pAttachments = descriptions;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &SubpassDescription;
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

        VkClearValue clearValues[2] = {};
        clearValues[0].color = {{m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w}};
        clearValues[1].depthStencil = {1.0f, 0};


        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(buffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

        VkDeviceSize offset[] = {0};

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

        if(m_cameraBuffer == nullptr){
            LOG_TERMINAL("The rendered has no camera... TODO: Add a default one",999) //TODO: default camera
        }
        vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &descriptorSetsLayouts[0].descriptorSet, 0, nullptr);


        std::pair<VkSampler,VkImageView> bindData = Texture::getDefaultBindData(); //Default texture to avoid crashes //TODO: TEXTURES
        //std::pair<VkSampler,VkImageView> bindData = m_drawCalls[0].texture->bindData();
        

        descriptorSetsLayouts[0].updateImageBinding(bindData.second,bindData.first,1,m_mainDevice->device);

        for(uint32_t i = 0; i < m_drawCalls.size();i++){

            pushConstData data = {m_drawCalls[i].transform, false};
            
            if(m_drawCalls[i].texture != nullptr)
                data.hasTexture = true;
            vkCmdPushConstants(buffer,m_pipelineLayout,VK_SHADER_STAGE_ALL,0,sizeof(pushConstData),&data);    

            vkCmdBindVertexBuffers(buffer, 0, 1,m_drawCalls[i].mesh->getVaoArray().data(), offset);
            vkCmdBindIndexBuffer(buffer, m_drawCalls[i].mesh->getEbo().getBuffer(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(buffer, static_cast<uint32_t>(m_drawCalls[i].mesh->getIndicesCount()), 1, 0, 0, 0);
        }
        m_drawCalls.clear();

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
        m_mainWindow->getNextSwapChainImage(index,m_waitImage,m_mainDevice->device,m_depthImage.imageView);

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

    vk_renderer &vk_renderer::get() {
         return vk_renderer::s_vk_renderer;
    }

    void vk_renderer::createDrawCall(drawCall newOrder){
        m_drawCalls.push_back(newOrder);
    }

    void vk_renderer::createPipelineLayout(){
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL; //At the moment push constants afect ALL stages
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(pushConstData);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = descriptorSetsLayouts.size();

        //TODO                                       HARD CODED CERO

        VkDescriptorSetLayout layouts[descriptorSetsLayouts.size()];
        for(int i = 0; i < descriptorSetsLayouts.size(); i++)
            layouts[i] = descriptorSetsLayouts[i].dstLayout;

        pipelineLayoutCreateInfo.pSetLayouts = layouts;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1; //Only one push constant
        pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange; 

        if (vkCreatePipelineLayout(m_mainDevice->device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS){
            LOG_TERMINAL("Fail creating a pipelineLayout",999)
        }
    }

    void vk_renderer::createPipeline(){
        m_pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        m_pipelineInfo.stageCount = 2;
        m_pipelineInfo.pStages = m_pair.stages;
        m_pipelineInfo.layout = m_pipelineLayout;
        m_pipelineInfo.renderPass = m_renderPass;
        m_pipelineInfo.subpass = 0;

        //May change
        m_pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        m_pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(m_mainDevice->device, VK_NULL_HANDLE, 1, &m_pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS){
            LOG_TERMINAL("Error initializing the pipeline...",999)
        }
    }
    
    void vk_renderer::plCreateColorBlendStateCreateInfo(){
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        colorBlendAttachmentState.blendEnable = VK_TRUE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
        m_pipelineInfo.pColorBlendState = &colorBlendStateCreateInfo;
    }
    
    void vk_renderer::plCreateDepthStencilStateCreateInfo(){
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        m_pipelineInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    }


    void vk_renderer::plCreateDynamicStateCreateInfo(){
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
        dynamicStateCreateInfo.pDynamicStates = m_dynamicStates.data();
        m_pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
    }

    void vk_renderer::plCreateInputAssemblyStateCreateInfo(){
        assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
        m_pipelineInfo.pInputAssemblyState = &assemblyStateCreateInfo;

    }
    void vk_renderer::plCreateMultisampleStateCreateInfo(){
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
        multisampleStateCreateInfo.pSampleMask = nullptr; // Optional
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional
        m_pipelineInfo.pMultisampleState = &multisampleStateCreateInfo;
    }


    void vk_renderer::plCreateRasterizationStateCreateInfo(){
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = m_polygonMode;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

        //May change
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
        rasterizationStateCreateInfo.depthBiasClamp = 0.0f; // Optional
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional
        m_pipelineInfo.pRasterizationState = &rasterizationStateCreateInfo;
    }

    void vk_renderer::plCreateVertexInputStateCreateInfo(){

        bindings = vertex::getBindingDescription();
        attributes = vertex::getAttributeDescription();

        inputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        inputStateCreateInfo.pNext = nullptr;
        inputStateCreateInfo.pVertexAttributeDescriptions = attributes.data();
        inputStateCreateInfo.pVertexBindingDescriptions = &bindings;
        inputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
        inputStateCreateInfo.vertexBindingDescriptionCount = 1;
        m_pipelineInfo.pVertexInputState = &inputStateCreateInfo;

    }

    //TODO: Should be dynamically modifiable
    void vk_renderer::plCreateViewportStateCreateInfo(){
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_mainWindow->getExtent().width);
        viewport.height = static_cast<float>(m_mainWindow->getExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset = {0, 0};
        scissor.extent = m_mainWindow->getExtent();

        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.pScissors = &scissor;
        m_pipelineInfo.pViewportState = &viewportStateCreateInfo;
    }

}





