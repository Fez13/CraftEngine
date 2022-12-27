#include "texture.h"

namespace craft{

    Texture Texture::s_default;

    Texture::Texture(std::string path,const std::string& deviceName,VkImageTiling tiling) : m_mainDeviceAbstraction(&vk_graphic_device::get().getDeviceAbstraction(deviceName)){
        
        stbi_uc *data = stbi_load(path.c_str(), &texture_size.x, &texture_size.y, &textureChannels, STBI_rgb_alpha);;
        
        m_tiling = tiling;

        if(!data){
            LOG("Error loading texture... \n\t Texture path: " + path,5,-1)
            return;
        }
        vk_buffer stageBuffer = vk_buffer(m_mainDeviceAbstraction, texture_size.x * texture_size.y * 4,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_SHARING_MODE_EXCLUSIVE);
        stageBuffer.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vk_graphic_device::get().getPhysicalDevice());
        stageBuffer.allocateMemory();
        stageBuffer.bindBuffer();

        void* gpu_memory = stageBuffer.getMemoryLocation<void*>();
        memcpy(gpu_memory, data, static_cast<size_t>(stageBuffer.getSize()));
        stageBuffer.unMapMemory();
        
        stbi_image_free(data);

        createImages(stageBuffer);
    }

    void Texture::free(){
        vkDestroyImage(m_mainDeviceAbstraction->device,m_image,nullptr);
        vkDestroyImageView(m_mainDeviceAbstraction->device,m_imageView,nullptr);
        vkDestroySampler(m_mainDeviceAbstraction->device,m_imageSampler,nullptr);
        vkFreeMemory(m_mainDeviceAbstraction->device,m_memory,nullptr);
    }

    void Texture::createImages(vk_buffer& staginBuffer){

        createImage(
            {texture_size.x,texture_size.y},
            VK_FORMAT_R8G8B8A8_SRGB,m_tiling,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_image,
            m_memory,
            m_mainDeviceAbstraction->device,
            vk_graphic_device::get().getPhysicalDevice(),
            VK_IMAGE_LAYOUT_UNDEFINED);

    
        transitionImageLayout(m_mainDeviceAbstraction,m_image,VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        staginBuffer.copyDataToImage(m_image,texture_size.x,texture_size.y);
        transitionImageLayout(m_mainDeviceAbstraction,m_image,VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        staginBuffer.free();

        createImageView(VK_IMAGE_VIEW_TYPE_2D,VK_FORMAT_R8G8B8A8_SRGB,m_mainDeviceAbstraction->device,m_imageView,m_image,VK_IMAGE_ASPECT_COLOR_BIT);

        createImageSampler(m_imageSampler,m_mainDeviceAbstraction->device,vk_graphic_device::get().getPhysicalDevice(),VK_FILTER_LINEAR,VK_FILTER_LINEAR,VK_FALSE,VK_TRUE);

    }

    std::pair<VkSampler,VkImageView> Texture::bindData(){
        return {m_imageSampler,m_imageView};
    }
    
    void Texture::createDefaultTexture(const std::string& deviceName,VkImageTiling tiling){
        s_default = Texture("../src/textures/default.png",deviceName,tiling);
    }

    std::pair<VkSampler,VkImageView> Texture::getDefaultBindData(){
        return s_default.bindData();
    }

    void Texture::freeDefault(){
        s_default.free();
    }


};
