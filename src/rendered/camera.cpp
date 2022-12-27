#include "camera.h"


namespace craft{

    vk_buffer Camera::s_cameraBuffer;

    const extencion Camera::s_extencionLabel = Extencions::CR_CAMERA_EXTENCION;

    std::vector<std::shared_ptr<Camera>> Camera::s_instancies;

    struct cameraDataBuffer{
        glm::mat4 projection;
        glm::mat4 view;
    };

    //Camera extencion
     Camera::Camera(float aspectRation) : m_ViewMat(1.0f),m_ProjectionMat(1.0f),m_ViewProjectionMat(1.0f) {
        m_aspectRation = aspectRation;
        m_farPlane = 100.0f;
        m_nearPlane = 0.01f;
        m_fov = 75.0f;
        updatePerspective();
    }

    void Camera::updateMainMat(glm::vec3 position,glm::vec3 direction){
        m_ViewMat = glm::lookAt(position, position + (direction), up);
        m_ViewProjectionMat = m_ProjectionMat * m_ViewMat;
    }

    glm::mat4 &Camera::getMainMatrix(){
        return m_ViewProjectionMat;
    }

    glm::mat4 *Camera::getMainMatrixPointer(){
        return &m_ViewProjectionMat;
    }

    void Camera::updatePerspective() {
        m_ProjectionMat = glm::perspective(glm::radians(m_fov),m_aspectRation,m_nearPlane,m_farPlane);
    }

    void Camera::initMainBuffer(deviceAbstraction& device){
        s_cameraBuffer = vk_buffer(device, sizeof(cameraDataBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
        s_cameraBuffer.createBufferMemoryRequirements(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vk_graphic_device::get().getPhysicalDevice());
        s_cameraBuffer.allocateMemory();
        s_cameraBuffer.bindBuffer();
    }

    void Camera::freeMainBuffer(){
        s_cameraBuffer.free();
    }

    void Camera::setMainBufferData(Camera* cma){
        auto *data_gpu = s_cameraBuffer.getMemoryLocation<cameraDataBuffer>();
        
        cameraDataBuffer data;
        data.view = cma->m_ViewMat; 
        data.projection = cma->m_ProjectionMat; 
        
        std::memcpy(data_gpu, &data, sizeof(data));
        s_cameraBuffer.unMapMemory();
    }

    void* cameraUpdate(Entity* entt){
        static Transform *transform = nullptr;
        static CameraController *cameraData = nullptr;
        static Camera *camera = nullptr;


        if(transform == nullptr || cameraData == nullptr || camera == nullptr){
            cameraData = getExtencion<CameraController>(entt);
            transform = getExtencion<Transform>(entt);
            camera = getExtencion<Camera>(entt);
        
        }


        static double previousTime = glfwGetTime();

        double currentTime = glfwGetTime();

        float deltaT = currentTime - previousTime;

        previousTime = currentTime;



        glm::vec2 CurrentMousePosition = input::get().GetMousePosition();

        if(glfwGetInputMode(static_cast<GLFWwindow*>(cameraData->m_window),GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
            return nullptr;

        static auto mousePosition_old = glm::vec2(0.0f);

        glm::vec2 MouseDelta = (CurrentMousePosition - mousePosition_old) * deltaT;

        MouseDelta.y *= -1.0f;

        transform->rotation = glm::vec3(MouseDelta * cameraData->m_camera_sens, 0);

        mousePosition_old = CurrentMousePosition;

        glm::vec3 right = glm::cross(cameraData->forward, cameraData->up);

        if (input::get().getKeyDowm(Key::W))
            transform->position += cameraData->m_camera_speed * cameraData->forward * deltaT;

        if (input::get().getKeyDowm(Key::S))
            transform->position -= cameraData->m_camera_speed * cameraData->forward * deltaT;

        if (input::get().getKeyDowm(Key::A))
            transform->position -= right * cameraData->m_camera_speed * deltaT;

        if (input::get().getKeyDowm(Key::D))
            transform->position += right * cameraData->m_camera_speed * deltaT;

        if (input::get().getKeyDowm(Key::Space))
            transform->position -= cameraData->up * cameraData->m_camera_speed * deltaT;

        if (input::get().getKeyDowm(Key::LeftControl))
            transform->position += cameraData->up * cameraData->m_camera_speed * deltaT;

        glm::quat q = glm::normalize(
                glm::cross(glm::angleAxis(transform->rotation.y, right), glm::angleAxis(transform->rotation.x,cameraData->up)));
        cameraData->forward = cameraData->forward * q;
        
        camera->updateMainMat(transform->position, cameraData->forward);
        Camera::setMainBufferData(camera);
        return nullptr;   
    }
}
