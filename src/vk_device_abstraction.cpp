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
        vkDestroyDevice(device, nullptr);
    };
}
