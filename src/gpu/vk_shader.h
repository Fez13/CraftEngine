#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "../debug/log.h"

namespace craft{

    struct shaderPair{
        bool complete[2] = {false, false};
        VkPipelineShaderStageCreateInfo vertex;
        VkPipelineShaderStageCreateInfo fragments;

        void createDouble();
        VkPipelineShaderStageCreateInfo stages[2];

    };

    static std::vector<char> readBinFile(const char* path);

    class shader{
    public:
        explicit shader() = default;

        explicit shader(const char* path, VkDevice device);

        void createShader(std::vector<char> data,VkDevice &device);

        void free();

    private:

    protected:
        VkDevice m_mainDevice;

        VkShaderModule m_shader;

    };

    class graphics_shader : public shader{
    public:

        graphics_shader(const char* path, VkDevice device, VkShaderStageFlagBits stage);

        graphics_shader() = default;

        void populateShaderPair(shaderPair &pair);

    private:

        VkShaderStageFlagBits m_stage;

        VkPipelineShaderStageCreateInfo m_shaderStage{};

    };

}