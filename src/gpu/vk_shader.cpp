#include <fstream>
#include "vk_shader.h"
namespace craft{
    static std::vector<char> readBinFile(const char* path){
        std::ifstream file(path, std::ios::ate | std::ios::binary);



        if(!file.is_open()){
            LOG_TERMINAL("Fail opening binary file...\n\tPath: " + std::string(path),5)
        }
        auto file_s = (uint32_t)file.tellg();
        std::vector<char> in(file_s);

        file.seekg(0);
        file.read(in.data(), file_s);
        file.close();

        return in;
    }

    void shader::createShader(std::vector<char> data,VkDevice &device){
        m_mainDevice = device;
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = data.size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(data.data());

        if(vkCreateShaderModule(device,&createInfo, nullptr,&m_shader) != VK_SUCCESS)
            LOG("Fail creating shader module",999, -1)
    }

    shader::shader(const char* path, VkDevice device) {
        m_shader = nullptr;
        createShader(readBinFile(path),device);

    }

    void shader::free() {
        vkDestroyShaderModule(m_mainDevice,m_shader, nullptr);
    }

    graphics_shader::graphics_shader(const char *path, VkDevice device, VkShaderStageFlagBits stage) : shader(path,device), m_stage(stage){
        m_shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        m_shaderStage.stage = m_stage;
        m_shaderStage.module = m_shader;
        m_shaderStage.pName = "main";


    }

    void graphics_shader::populateShaderPair(shaderPair &pair) {
        if(m_stage == VK_SHADER_STAGE_VERTEX_BIT){
            pair.vertex = m_shaderStage;
            pair.complete[0] = true;
        }

        if(m_stage == VK_SHADER_STAGE_FRAGMENT_BIT){
            pair.fragments = m_shaderStage;
            pair.complete[1] = true;
        }
    }

    void shaderPair::createDouble() {
        if(!complete[0] || !complete[1]){
            LOG_TERMINAL("Error while creating shaders",1);
        }

        stages[0] = vertex;
        stages[1] = fragments;
    }
}