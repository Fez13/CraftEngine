#include "ebo.h"

namespace craft{

    ebo::ebo(const std::vector<uint32_t> &indices, deviceAbstraction &deviceAbstraction, VkPhysicalDevice const &device) {
        vk_buffer index_stage(deviceAbstraction, sizeof(uint32_t) * indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);
        index_stage.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device);
        index_stage.allocateMemory();
        index_stage.bindBuffer();

        auto* vertices_gpu = index_stage.getMemoryLocation<vertex>();
        std::memcpy(vertices_gpu, indices.data(), index_stage.getSize());
        index_stage.unMapMemory();

        m_buffer = vk_buffer(deviceAbstraction, sizeof(vertex) * indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT , VK_SHARING_MODE_EXCLUSIVE);
        m_buffer.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device);
        m_buffer.allocateMemory();
        m_buffer.bindBuffer();

        m_buffer << index_stage;
        index_stage.free();
    }

    void ebo::free() {
        m_buffer.free();
    }

    VkBuffer ebo::getBuffer() {
        return m_buffer.getBuffer();
    }


}