#pragma once

#include <vector>
#include <fstream>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../vendor/glm/glm.hpp"

#include  "../rendered/imageUtils.hpp"
#include "../utils/utils.hpp"
#include "../debug/log.h"
#include "vk_window.h"
#include "../gpu/vk_device_abstraction.h"
#include "../gpu/vk_graphic_device.h"
#include "../gpu/vk_shader.h"
#include "mesh.h"
#include "../gpu/vk_buffer.h"
#include "../gpu/texture.h"

namespace craft{

    struct drawCall{
        Mesh* mesh;
        glm::mat4 transform;
        Texture* texture;
    };
     struct pushConstData{
        glm::mat4 mat;
        bool hasTexture;

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

        void reCreateSwapChain(int width, int height);

        void setCameraBuffer(vk_buffer& buffer){m_cameraBuffer = std::make_shared<vk_buffer>(buffer);}

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

        glm::mat4* m_currentCameraMat;
        std::shared_ptr<vk_buffer> m_cameraBuffer;


        VkDescriptorPool m_descriptionPool;

        struct bufferBinding{
            bool populated = false;
            VkDescriptorSetLayoutBinding layoutBinding;
            VkDescriptorBufferInfo bufferInfo;
        };

        struct imageBinding{
            bool populated = false;
            VkDescriptorSetLayoutBinding layoutBinding;
            VkDescriptorImageInfo imageInfo;
        };

        struct descriptorSetData{
            std::vector<std::shared_ptr<vk_buffer>> buffers;

            std::unordered_map<uint32_t, bufferBinding> bufferBindings;
            std::unordered_map<uint32_t, imageBinding> imageBindings;

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

     
        
            void InitDescriptorAllocator(VkDevice &device){
                VkDescriptorSetLayoutCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                createInfo.bindingCount = bindingsCount;

                VkDescriptorSetLayoutBinding bindings[bindingsCount];
                uint32_t bindingIndex = 0;

                for(const auto& binding : imageBindings){
                    bindings[bindingIndex] = binding.second.layoutBinding;
                    bindingIndex++;
                }

                for(const auto& binding : bufferBindings){
                    bindings[bindingIndex] = binding.second.layoutBinding;
                    bindingIndex++;
                }
                                   
                createInfo.pBindings = bindings;

                if(vkCreateDescriptorSetLayout(device,&createInfo,nullptr,&dstLayout) != VK_SUCCESS)
                    LOG_TERMINAL("Error creating DescriptorSetLayout...",999)

                std::vector<VkDescriptorPoolSize> sizes;

                
                for(uint32_t i = 0; i < bufferBindings.size(); i++)
                    sizes.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1);

                for(uint32_t i = 0; i < imageBindings.size(); i++)
                    sizes.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1);
                
                VkDescriptorPoolCreateInfo dstPoolInfo{};
                dstPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                dstPoolInfo.poolSizeCount = bindingsCount;
                dstPoolInfo.pPoolSizes = sizes.data();
                dstPoolInfo.maxSets = bindingsCount;

                if(vkCreateDescriptorPool(device,&dstPoolInfo,nullptr,&dstPool) != VK_SUCCESS)
                    LOG_TERMINAL("Error creating DescriptorPool",999)
            }
            void updateImageBinding(VkImageView &imageView, VkSampler &sampler,uint32_t index, VkDevice &device){

                VkDescriptorImageInfo *obj = &imageBindings[index].imageInfo;
                obj->imageView = imageView;
                obj->sampler = sampler;
                obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                VkWriteDescriptorSet wrds{};
                wrds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                wrds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                wrds.dstBinding = index;
                wrds.dstArrayElement = 0;
                wrds.descriptorCount = 1;
                wrds.dstSet = descriptorSet;
                wrds.pImageInfo = obj;
                vkUpdateDescriptorSets(device , 1 ,&wrds,0,nullptr);
            }
            void addBindingImage(uint32_t index,VkShaderStageFlagBits stages = VK_SHADER_STAGE_VERTEX_BIT){
                bindingsCount++;

                imageBindings[index].populated = true;
                VkDescriptorSetLayoutBinding *obj = &imageBindings[index].layoutBinding;

                obj->binding = index;
                obj->descriptorCount = 1;
                obj->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                obj->pImmutableSamplers = nullptr;
                obj->stageFlags = stages;
            }

            void updateBufferBinding(std::shared_ptr<vk_buffer> buffer, uint32_t index, VkDevice &device){
                VkDescriptorBufferInfo *obj = &bufferBindings[index].bufferInfo;
                obj->buffer = buffer->getBuffer();
                obj->offset = 0;
                obj->range = buffer->getSize();

                VkWriteDescriptorSet wrds{};
                wrds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                wrds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                wrds.dstBinding = index;
                wrds.dstArrayElement = 0;
                wrds.descriptorCount = 1;
                wrds.dstSet = descriptorSet;
                wrds.pNext = nullptr;
                wrds.pBufferInfo = obj;
                vkUpdateDescriptorSets(device , 1 ,&wrds,0,nullptr);
            }

            void addBinding(uint32_t index ,VkShaderStageFlagBits stages = VK_SHADER_STAGE_VERTEX_BIT){
                bindingsCount++;
                bufferBindings[index].populated = true;
                VkDescriptorSetLayoutBinding *obj = &bufferBindings[index].layoutBinding;

                obj->binding = index;
                obj->descriptorCount = 1;
                obj->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                obj->pImmutableSamplers = nullptr;
                obj->stageFlags = stages;
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
        std::array<VkVertexInputAttributeDescription, ATRIBUTE_COUNT> attributes;
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


