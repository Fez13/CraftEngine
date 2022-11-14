#include "vk_device_abstraction.h"

namespace craft{

    void deviceAbstraction::findQueue() {
        vkGetDeviceQueue(device, family, 0, &queue);
    }

    void deviceAbstraction::createCommandPool() {
        VkCommandPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = family;

        if (vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
            LOG("Fail creating command pool...\n\t Device name: " + name,999,-1)
            exit(1);
        }
    }

    void deviceAbstraction::free() const {
        if(commandPool != nullptr)
            vkDestroyCommandPool(device,commandPool, nullptr);
        vkDestroyFence(device,fence, nullptr);
        vkDestroyDevice(device, nullptr);
    }

    VkCommandBuffer deviceAbstraction::createCommandBuffer(VkCommandBufferLevel bufferLevel, uint32_t bufferCount) const {
        if(bufferCount == 0)
            LOG("Why would you create empty command buffer? Anyways here is your warning...",1,0)

        VkCommandBuffer commandBuffer;
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = bufferLevel;
        allocInfo.commandBufferCount = bufferCount;

        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            LOG("Error creating command buffer...",999,-1)
            exit(1);
        }
        return commandBuffer;
    }

    void deviceAbstraction::SubmitWork(VkSemaphore) {

    }

    void deviceAbstraction::createFence(VkFenceCreateFlags flags) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = flags;
        if(vkCreateFence(this->device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
            LOG("Error creating fence in: " + this->name,1,0)
    }

}
