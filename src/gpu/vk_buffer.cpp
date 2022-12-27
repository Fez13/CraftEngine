#include "vk_buffer.h"


namespace craft{

  

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
        VkCommandBuffer cmd = input.m_mainDevice->createOneTimeUseCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkBufferCopy bufferCopy{};
        bufferCopy.dstOffset = 0;
        bufferCopy.srcOffset = 0;
        bufferCopy.size = input.m_size;

        vkCmdCopyBuffer(cmd,input.m_buffer,output.m_buffer,1,&bufferCopy);
        vkEndCommandBuffer(cmd);

        input.m_mainDevice->SubmitWork(&cmd,1);
        input.m_mainDevice->waitToFinish(1);
    }


    

    void vk_buffer::copyDataToImage(VkImage dst,const uint32_t &width,const uint32_t &height) {

        if(!(m_localUsages & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)){
            LOG("Wen writing in a image the source buffer must have the 'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' flag, the operation will be aborted.",5,-1)
            return;
        }
 

        if(m_mainDevice->queue.size() == 1){
            LOG("The n of queues in the device: " + m_mainDevice->name + " is to low for a buffer copy, a new queue will be created",5,0)
            m_mainDevice->findNewQueue();
        }
        VkCommandBuffer cmd = m_mainDevice->createOneTimeUseCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width,height,1};

        vkCmdCopyBufferToImage(cmd,m_buffer,dst,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&region);

        vkEndCommandBuffer(cmd);
        m_mainDevice->SubmitWork(&cmd,1);
        m_mainDevice->waitToFinish(1);
    }

}








