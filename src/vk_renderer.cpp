#include "vk_renderer.h"


namespace craft{

    VkShaderModule createShaderModule(std::vector<char> data,VkDevice &device){
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = data.size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(data.data());

        VkShaderModule obj;
        if(vkCreateShaderModule(device,&createInfo, nullptr,&obj) != VK_SUCCESS)
            LOG("Fail creating shader module",999, -1)
        return obj;
    }

    static std::vector<char> readBinFile(const char* path){
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if(!file.is_open())
            throw std::runtime_error("Fail opening binary file...\n\tPath: " + std::string(path));

        auto file_s = (uint32_t)file.tellg();
        std::vector<char> in(file_s);

        file.seekg(0);
        file.read(in.data(), file_s);
        file.close();

        return in;
    }

    vk_renderer::vk_renderer(VkDevice mainDevice) {
        m_mainWindow = nullptr;
        m_pipelineLayout = nullptr;
        m_renderPass = nullptr;
        m_mainDevice = mainDevice;
    }

    void vk_renderer::setDynamicStates(const std::vector<VkDynamicState> &states) {
        m_dynamicStates = states;
    }


    void vk_renderer::loadShaders(const char *vert_path, const char *frag_path) {
        m_vert = readBinFile(vert_path);
        m_frag = readBinFile(frag_path);

        m_vertexModule = createShaderModule(m_vert,m_mainDevice);
        m_fragModule = createShaderModule(m_frag,m_mainDevice);
    }

    void vk_renderer::createShaderPipeline() {

        if(m_mainDevice == nullptr){
            LOG("Rendered without main device",999,-1)
            exit(1);
        }
        if(m_mainWindow == nullptr){
            LOG("Rendered without main window",999,-1)
            exit(1);
        }

        createRenderPass();

        VkPipelineShaderStageCreateInfo createInfoVertex{};
        createInfoVertex.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfoVertex.stage = VK_SHADER_STAGE_VERTEX_BIT;
        createInfoVertex.module = m_vertexModule;
        createInfoVertex.pName = "main";

        VkPipelineShaderStageCreateInfo createInfoFragment{};
        createInfoFragment.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfoFragment.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        createInfoFragment.module = m_fragModule;
        createInfoFragment.pName = "main";


        VkPipelineShaderStageCreateInfo stages[] = {createInfoVertex,createInfoFragment};

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(m_dynamicStates.size());
        dynamicStateCreateInfo.pDynamicStates = m_dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo inputStateCreateInfo{};
        inputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        inputStateCreateInfo.pNext = nullptr;
        inputStateCreateInfo.pVertexAttributeDescriptions = nullptr;
        inputStateCreateInfo.vertexAttributeDescriptionCount = 0;
        inputStateCreateInfo.vertexBindingDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo{};
        assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        assemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        //TODO:Should be dynamically modifiable
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) m_mainWindow->getExtent().width;
        viewport.height = (float) m_mainWindow->getExtent().height;
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

        if (vkCreatePipelineLayout(m_mainDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
            LOG("Fail creating a pipelineLayout",999,-1)
            exit(1);
        }

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = 2;
        graphicsPipelineCreateInfo.pStages = stages;

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

        if (vkCreateGraphicsPipelines(m_mainDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
            LOG("Error initializing the pipeline...",999,-1)
            exit(1);
        }
    }

    void vk_renderer::setMainWindow(vk_window *mainWindow) {
        m_mainWindow = mainWindow;
    }

    void vk_renderer::free() {
        vkDestroyPipeline(m_mainDevice,m_pipeline, nullptr);
        vkDestroyRenderPass(m_mainDevice,m_renderPass, nullptr);
        vkDestroyPipelineLayout(m_mainDevice,m_pipelineLayout, nullptr);
        vkDestroyShaderModule(m_mainDevice,m_vertexModule, nullptr);
        vkDestroyShaderModule(m_mainDevice,m_fragModule, nullptr);
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

        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &attachmentDescription;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;

        if (vkCreateRenderPass(m_mainDevice, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
            LOG("Couldn't initialize the render pass...",999,-1)
            exit(1);
        }
    }

}



