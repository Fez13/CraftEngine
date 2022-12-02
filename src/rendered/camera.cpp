#include "camera.h"


namespace craft{

    void camera_ent::update() {

        //Hard coded deltaT
        const float deltaT = 1.0f / 60.0f;

        glm::vec2 CurrentMousePosition = input::get().GetMousePosition();

        if(glfwGetInputMode(m_window,GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
            return;

        static auto mousePosition_old = glm::vec2(0.0f);

        glm::vec2 MouseDelta = (CurrentMousePosition - mousePosition_old) * deltaT;

        MouseDelta.y *= -1.0f;

        rotation = glm::vec3(MouseDelta * m_camera_sens, 0);

        mousePosition_old = CurrentMousePosition;

        glm::vec3 right = glm::cross(forward, up);

        if (input::get().getKeyDowm(Key::W))
            position += m_camera_speed * forward;

        if (input::get().getKeyDowm(Key::S))
            position -= m_camera_speed * forward;

        if (input::get().getKeyDowm(Key::A))
            position -= right * m_camera_speed;

        if (input::get().getKeyDowm(Key::D))
            position += right * m_camera_speed;

        if (input::get().getKeyDowm(Key::Space))
            position -= up * m_camera_speed;

        if (input::get().getKeyDowm(Key::LeftControl))
            position += up * m_camera_speed;

        glm::quat q = glm::normalize(
                glm::cross(glm::angleAxis(rotation.y, right), glm::angleAxis(rotation.x, up)));
        forward = forward * q;
    }

    void camera_ent::updateCamera(){
        updateMainMat(position, forward);
    }

    camera_ent::camera_ent(float aspectRatio,GLFWwindow *window) : camera(aspectRatio), Transform() {
        position = glm::vec3(0.0f);
        m_window = window;
    }


    camera::camera(float aspectRation) : m_ViewMat(1.0f),m_ProjectionMat(1.0f),m_ViewProjectionMat(1.0f) {
        m_aspectRation = aspectRation;
        m_farPlane = 100.0f;
        m_nearPlane = 0.01f;
        m_fov = 75.0f;
        updatePerspective();
    }

    void camera::updateMainMat(glm::vec3 position,glm::vec3 direction){
        m_ViewMat = glm::lookAt(position, position + (direction), up);
        m_ViewProjectionMat = m_ProjectionMat * m_ViewMat;
    }

    glm::mat4 &camera::getMainMatrix(){
        return m_ViewProjectionMat;
    }

    void camera::updatePerspective() {
        m_ProjectionMat = glm::perspective(glm::radians(m_fov),m_aspectRation,m_nearPlane,m_farPlane);
    }


}
