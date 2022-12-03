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

    struct cameraPushData{
        glm::mat4 cameraMatrix;

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

        std::vector<mesh*> meshes;

        std::vector<std::pair<vk_buffer,glm::mat4*>> *ubo_buffers;

        void setMainCamera(camera *pCamera);

    private:

        static vk_renderer s_vk_renderer;

        vk_renderer() = default;

        struct depthImage{
        public:
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

        //May be public in the future since it will be personalised by the user

        vk_window *m_mainWindow;


        std::vector<VkDynamicState> m_dynamicStates;

        deviceAbstraction *m_mainDevice;

        graphics_shader m_frag;
        graphics_shader m_vert;

        VkPolygonMode m_polygonMode = VK_POLYGON_MODE_FILL;

        //May be removed from the class
        VkPipelineLayout m_pipelineLayout;

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

        VkDescriptorSetLayout m_descriptorSetLayout;

        camera* m_currentCamera;

    };
}


