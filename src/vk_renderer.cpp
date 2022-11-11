#include "vk_renderer.h"


namespace craft{

    VkShaderModule createShaderModule(std::vector<char> data,VkDevice &device){
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = data.size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(data.data());

        VkShaderModule obj;
        if(vkCreateShaderModule(device,&createInfo, nullptr,&obj) != VK_SUCCESS)
            LOG("Fail creating shader module","vk_renderer.cpp/14",999, -1);
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
        m_mainDevice = nullptr;
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
            LOG("Rendered without main device","vk_rendered/54",999,-1);
            exit(1);
        }
        if(m_mainWindow == nullptr){
            LOG("Rendered without main window","vk_rendered/58",999,-1);
            exit(1);
        }


        VkPipelineShaderStageCreateInfo createInfoVertex{};
        createInfoVertex.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfoVertex.stage = VK_SHADER_STAGE_VERTEX_BIT;
        createInfoVertex.module = m_vertexModule;
        createInfoVertex.pName = "main";

        VkPipelineShaderStageCreateInfo createInfoFragment{};
        createInfoVertex.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        createInfoVertex.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        createInfoVertex.module = m_fragModule;
        createInfoVertex.pName = "main";

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
    }

    void vk_renderer::setMainWindow(vk_window *mainWindow) {
        m_mainWindow = mainWindow;
    }

    void vk_renderer::free() {
        vkDestroyShaderModule(m_mainDevice,m_vertexModule, nullptr);
        vkDestroyShaderModule(m_mainDevice,m_fragModule, nullptr);
    }

    void vk_renderer::setPolygonMode(VkPolygonMode mode) {
        m_polygonMode = mode;
    }

}

