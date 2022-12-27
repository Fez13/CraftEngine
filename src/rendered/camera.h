#pragma once


#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include"../vendor/glm/gtx/rotate_vector.hpp"
#include"../vendor/glm/gtx/vector_angle.hpp"
#include "../vendor/glm/glm.hpp"
#include "../debug/log.h"
#include "../gameObjects/entity.h"
#include "../gameObjects/entitiesExtencions.h"
#include "../input/input.h"
#include "../gpu/vk_buffer.h"
#include "../vendor/glm/gtc/matrix_transform.hpp"
#include "../gpu/vk_graphic_device.h"
#include "../gameObjects/extencionsLabels.h"
#include "../gameObjects/entitiesExtencions.h"

namespace craft{

    struct Camera {
    public:
        explicit Camera(float aspectRation);

        void updateMainMat(glm::vec3 position,glm::vec3 direction);

        void updatePerspective();

        glm::mat4 &getMainMatrix();

        glm::mat4 *getMainMatrixPointer();

        //Has to be call after creating a gpu
        static void initMainBuffer(deviceAbstraction&);

        static void freeMainBuffer();

        static void setMainBufferData(Camera*);

        static vk_buffer s_cameraBuffer;

        Entity* attachment;

        static std::vector<std::shared_ptr<Camera>> s_instancies;

        const static extencion s_extencionLabel;

        ~Camera(){LOG_COMPONENT_DESTRUCTION("Camera")}


    private:
        glm::mat4 m_ProjectionMat;
        glm::mat4 m_ViewMat;
        glm::mat4 m_ViewProjectionMat;

        float m_nearPlane;
        float m_farPlane;
        float m_aspectRation;
        float m_fov;

        glm::vec3 forward = glm::vec3(0, 0, -1);
        const glm::vec3 up = glm::vec3(0, 1, 0);
    };

    void* cameraUpdate(Entity* entt);
}
