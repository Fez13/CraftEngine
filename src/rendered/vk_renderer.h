#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "glm.hpp"


#include "../utils/utils.hpp"
#include "../debug/log.h"
#include "vk_window.h"
#include "../gpu/vk_device_abstraction.h"
#include "../gpu/vk_graphic_device.h"
#include "../gpu/vk_shader.h"
#include "mesh.h"
#include "../gpu/vk_buffer.h"
#include "camera.h"

namespace craft{


    struct drawCall{
        Mesh* mesh;
        Transform* transform;
    };
    struct cameraPushData{
    };

    //TODO:
    //All the command buffer stuff will be in a separated class and file which i am not sure
    // how to implement at the moment

    //This is quite of a complicated class since it will not only handle all the basic 3D rendering,
    // it will also take care of ray tracing that will be used to the reflective objects of the scene
    class vk_renderer {
    public:

        vk_renderer(const vk_renderer&) = delete;

        static vk_renderer &get();

        void initialize(deviceAbstraction *mainDevice);

        void loadShaders(const char* vert_path,const char* frag_path);

        void createShaderPipeline();

        void setDynamicStates(const std::vector<VkDynamicState> &states);

        void setMainWindow(vk_window *mainWindow);

        void setPolygonMode(VkPolygonMode);

        VkRenderPass getRenderPass() const;

        void setClearColor(glm::vec4);

        void updateFrame();

        void waitToFinish() const;

        void free();

        void createDrawCall(drawCall newCall);
        void createDrawCall(Mesh* mesh, Transform* transform);

        void setMainCamera(camera *pCamera);

        void reCreateSwapChain(int width, int height);

        VkPhysicalDevice temporal; //!                                                          <-----


    private:

        static vk_renderer s_vk_renderer;

        vk_renderer() = default;

        std::vector<drawCall> m_drawCalls;

        struct depthImage{
        public:

            void create(vk_window* window,VkDevice &device);

            VkFormat format;
            VkDeviceMemory memory;
            VkImage depthImage;
            VkImageView imageView;

            void free(VkDevice &device){
                vkDestroyImageView(device,imageView, nullptr);
                vkDestroyImage(device,depthImage, nullptr);
                vkFreeMemory(device,memory, nullptr);
            }
        };

        depthImage m_depthImage;

        void createRenderPass();

        void recordCommandBuffer(uint32_t index, VkCommandBuffer &buffer);

        void createPipelineLayout();

        void createPipeline();

        shaderPair m_pair;

        //May be public in the future since it will be personalised by the user

        vk_window *m_mainWindow;


        std::vector<VkDynamicState> m_dynamicStates;

        deviceAbstraction *m_mainDevice;

        graphics_shader m_frag;
        graphics_shader m_vert;

        VkPolygonMode m_polygonMode = VK_POLYGON_MODE_FILL;

        //May be removed from the class
        VkPipelineLayout m_pipelineLayout;

        VkGraphicsPipelineCreateInfo m_pipelineInfo;

        VkPipeline m_pipeline;

        VkRenderPass m_renderPass;

        glm::vec4 m_clearColor;

        uint64_t m_countOfVertices;

        uint32_t m_verticesOffset;

        std::vector<uint32_t> m_usedFamilies;

        VkCommandBuffer m_mainCommandBuffer;

        //Smartphones should not be in the renderer
        VkSemaphore m_waitImage;
        VkSemaphore m_waitRender;

        camera* m_currentCamera;

        VkDescriptorPool m_descriptionPool;

        struct descriptorSetData{
            std::vector<std::unique_ptr<vk_buffer>> buffers;
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            std::vector<VkDescriptorBufferInfo> bufferInfos;

            VkDescriptorPool dstPool;
            VkDescriptorSet descriptorSet;
            VkDescriptorSetLayout dstLayout;
            uint32_t bindingsCount = 0;


            void allocateDescriptorSet(VkDevice& device){
                VkDescriptorSetAllocateInfo dstSetInfo{};
                dstSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                dstSetInfo.descriptorSetCount = 1;
                dstSetInfo.pSetLayouts = &dstLayout;
                dstSetInfo.descriptorPool = dstPool;
                dstSetInfo.pNext = nullptr;


                if(vkAllocateDescriptorSets(device,&dstSetInfo,&descriptorSet) != VK_SUCCESS){
                    LOG_TERMINAL("Error allocating aa descriptorSet",999)
                }
            }

            //Call AFTER creatin the pipeline
            void updateDescriptorSets(VkDevice &device){
                VkWriteDescriptorSet wrtDst;
                wrtDst.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                wrtDst.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                wrtDst.dstBinding = 0;
                wrtDst.dstArrayElement = 0;
                wrtDst.descriptorCount = bindingsCount;
                wrtDst.pBufferInfo = bufferInfos.data();
                wrtDst.dstSet = descriptorSet;
                wrtDst.pNext = nullptr;
                
                
                vkUpdateDescriptorSets(device , 1 ,&wrtDst,0,nullptr);
            }

        
            void InitDescriptorAllocator(VkDevice &device){
                VkDescriptorSetLayoutCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                createInfo.bindingCount = bindingsCount;
                createInfo.pBindings = bindings.data();

                if(vkCreateDescriptorSetLayout(device,&createInfo,nullptr,&dstLayout) != VK_SUCCESS)
                    LOG_TERMINAL("Error creating DescriptorSetLayout...",999)

                VkDescriptorPoolSize poolSize{};
                poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                poolSize.descriptorCount = bindingsCount;

                VkDescriptorPoolCreateInfo dstPoolInfo{};
                dstPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                dstPoolInfo.poolSizeCount = 1;
                dstPoolInfo.pPoolSizes = &poolSize;
                dstPoolInfo.maxSets = bindingsCount;

                if(vkCreateDescriptorPool(device,&dstPoolInfo,nullptr,&dstPool) != VK_SUCCESS)
                    LOG_TERMINAL("Error creating DescriptorPool",999)
            }

            void addBinding(vk_buffer& buffer, uint32_t index,VkShaderStageFlagBits stages = VK_SHADER_STAGE_VERTEX_BIT,VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
                bindingsCount++;
                bindings.emplace_back();
                buffers.emplace_back(std::make_unique<vk_buffer>(buffer));
                bufferInfos.emplace_back();

                bindings.back().binding = index;
                bindings.back().descriptorCount = 1;
                bindings.back().descriptorType = type;
                bindings.back().pImmutableSamplers = nullptr;
                bindings.back().stageFlags = stages;

                bufferInfos.back().buffer = buffers.back()->getBuffer();
                bufferInfos.back().offset = 0;
                bufferInfos.back().range = buffers.back()->getSize();
            }

            void free(VkDevice& device){
                vkDestroyDescriptorPool(device,dstPool,nullptr);
                vkDestroyDescriptorSetLayout(device,dstLayout,nullptr);
            }

        };


        std::vector<descriptorSetData> descriptorSetsLayouts;

        //Pipeline components

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
        void plCreateDynamicStateCreateInfo();


        VkVertexInputBindingDescription bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        VkPipelineVertexInputStateCreateInfo inputStateCreateInfo;
        void plCreateVertexInputStateCreateInfo();

        VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo;
        void plCreateInputAssemblyStateCreateInfo();


        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
        void plCreateViewportStateCreateInfo();

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
        void plCreateRasterizationStateCreateInfo();

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
        void plCreateMultisampleStateCreateInfo();


        VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
        void plCreateColorBlendStateCreateInfo();

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
        void plCreateDepthStencilStateCreateInfo();


    };
}


