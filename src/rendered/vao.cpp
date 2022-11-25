#include "vao.h"

namespace craft{
    vao::vao(const std::vector<vertex> &vertices,deviceAbstraction& deviceAbstraction,const VkPhysicalDevice &device){

        vk_buffer vertex_stage(deviceAbstraction, sizeof(vertex) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);
        vertex_stage.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device);
        vertex_stage.allocateMemory();
        vertex_stage.bindBuffer();

        auto* vertices_gpu = vertex_stage.getMemoryLocation<vertex>();
        std::memcpy(vertices_gpu, vertices.data(), vertex_stage.getSize());
        vertex_stage.unMapMemory();

        m_buffer = vk_buffer(deviceAbstraction, sizeof(vertex) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT , VK_SHARING_MODE_EXCLUSIVE);
        m_buffer.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device);
        m_buffer.allocateMemory();
        m_buffer.bindBuffer();

        m_buffer << vertex_stage;
        vertex_stage.free();
    }

    VkBuffer vao::getBuffer() {
        return m_buffer.getBuffer();
    }

    void vao::free() {
        m_buffer.free();
    }
}
