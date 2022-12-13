#include "vk_buffer.h"


namespace craft{

    uint32_t findMemory(const VkMemoryPropertyFlags memoryProperties,VkPhysicalDeviceMemoryProperties deviceMemoryProperties,uint32_t memoryType){

        for(uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount;i++){
            if(!(memoryType & (1 < i)))
                continue;

            if((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties)
                return i;
        }
        LOG_TERMINAL("Couldn't find memory properties",1)
    }

    vk_buffer::vk_buffer(deviceAbstraction &mainDevice, uint32_t size,const VkBufferUsageFlags &usage,
                                VkSharingMode sharingMode) {
        m_memoryRequirements = VkMemoryRequirements{};
        m_mainDevice = &mainDevice;
        m_size = size;
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.sharingMode = sharingMode;
        bufferCreateInfo.size = m_size;
        m_localUsages = usage;
        bufferCreateInfo.usage = usage;

        if(vkCreateBuffer(mainDevice.device,&bufferCreateInfo, nullptr,&m_buffer) != VK_SUCCESS){
            LOG_TERMINAL("Error creating a buffer",1)
        }
    }

    vk_buffer::vk_buffer(deviceAbstraction *mainDevice,uint32_t size,const VkBufferUsageFlags &usage, VkSharingMode sharingMode){
        m_memoryRequirements = VkMemoryRequirements{};
        m_mainDevice = mainDevice;
        m_size = size;
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.sharingMode = sharingMode;
        bufferCreateInfo.size = m_size;
        m_localUsages = usage;
        bufferCreateInfo.usage = usage;

        if(vkCreateBuffer(mainDevice->device,&bufferCreateInfo, nullptr,&m_buffer) != VK_SUCCESS){
            LOG_TERMINAL("Error creating a buffer",1)
        }
    }


    void vk_buffer::free() {
        vkDestroyBuffer(m_mainDevice->device,m_buffer, nullptr);
        vkFreeMemory(m_mainDevice->device,m_memory, nullptr);
    }

    void vk_buffer::createBufferMemoryRequirements(const VkMemoryPropertyFlags &memoryProperties, const VkPhysicalDevice& gpu) {
        m_mainGpu = gpu; 
        m_memoryRequirements.size = m_size;
        m_memoryProperties = memoryProperties;

        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(gpu,&deviceMemoryProperties);

        m_memoryIndex = findMemory(m_memoryProperties,deviceMemoryProperties,1);

        vkGetBufferMemoryRequirements(m_mainDevice->device,m_buffer,&m_memoryRequirements);
    }


    void vk_buffer::allocateMemory() {
        VkMemoryAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = m_memoryRequirements.size;
        allocateInfo.memoryTypeIndex = m_memoryIndex;


        if(vkAllocateMemory(m_mainDevice->device , &allocateInfo, nullptr, &m_memory) != VK_SUCCESS){
            LOG_TERMINAL("Error allocating gpu buffer",5)
        }
    }

    size_t vk_buffer::getSize() const{
        return m_size;
    }


    void vk_buffer::bindBuffer() {
        vkBindBufferMemory(m_mainDevice->device ,m_buffer,m_memory,0);
    }

    void vk_buffer::unMapMemory() {
        vkUnmapMemory(m_mainDevice->device,m_memory);
    }

    VkBuffer vk_buffer::getBuffer() {
        return m_buffer;
    }

    void operator<<(vk_buffer &output, vk_buffer &input) {

        if(!(input.m_localUsages & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)){
            LOG("Wen writing in a gpu buffer the source must have the 'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' flag, the operation will be aborted.",5,-1)
            return;
        }
        if(!(output.m_localUsages & VK_BUFFER_USAGE_TRANSFER_DST_BIT)){
            LOG("Wen writing in a gpu buffer the destine must have the 'VK_BUFFER_USAGE_TRANSFER_DST_BIT' flag, the operation will be aborted.",5,-1)
            return;
        }

        if(input.m_mainDevice->queue.size() == 1){
            LOG("The n of queues in the device: " + input.m_mainDevice->name + " is to low for a buffer copy, a new queue will be created",5,0)
            input.m_mainDevice->findNewQueue();
        }
        VkCommandBuffer cmd = input.m_mainDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY,1);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd,&beginInfo);

        VkBufferCopy bufferCopy{};
        bufferCopy.dstOffset = 0;
        bufferCopy.srcOffset = 0;
        bufferCopy.size = input.m_size;

        vkCmdCopyBuffer(cmd,input.m_buffer,output.m_buffer,1,&bufferCopy);
        vkEndCommandBuffer(cmd);

        input.m_mainDevice->SubmitWork(&cmd,1);
        input.m_mainDevice->waitToFinish(1);

        vkFreeCommandBuffers(input.m_mainDevice->device,input.m_mainDevice->commandPool,1,&cmd);
    }

}








