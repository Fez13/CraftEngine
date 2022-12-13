#pragma once


#include <cstring>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include"../glm/gtx/rotate_vector.hpp"
#include"../glm/gtx/vector_angle.hpp"
#include "../glm/glm.hpp"
#include "../debug/log.h"
#include "../gameObjects/entity.h"
#include "../gameObjects/entitiesExtencions.h"
#include "../input/input.h"
#include "../gpu/vk_buffer.h"
#include "../glm/gtc/matrix_transform.hpp"
#include "../gpu/vk_graphic_device.h"

namespace craft{

    class camera {
    public:
        explicit camera(float aspectRation);

        void updateMainMat(glm::vec3 position,glm::vec3 direction);

        void updatePerspective();

        glm::mat4 &getMainMatrix();

        //Has to be call after creating a gpu
        static void initMainBuffer(deviceAbstraction&);

        static void freeMainBuffer();

        static void setMainBufferData(camera*);

        static vk_buffer s_cameraBuffer;

    private:
        glm::mat4 m_ProjectionMat;
        glm::mat4 m_ViewMat;
        glm::mat4 m_ViewProjectionMat;

    protected:
        float m_nearPlane;
        float m_farPlane;
        float m_aspectRation;
        float m_fov;

        glm::vec3 forward = glm::vec3(0, 0, -1);
        const glm::vec3 up = glm::vec3(0, 1, 0);
    };

    class camera_ent : Entity, Update, public Transform, public camera{

    public:

        camera_ent(float aspectRatio,  GLFWwindow *window);

        void update() override;

        void updateCamera();

    protected:
    
        float m_camera_speed = 1.0f * 0.016f;
        float m_camera_sens = 10 * 0.016f;
        GLFWwindow *m_window;

    };
}
