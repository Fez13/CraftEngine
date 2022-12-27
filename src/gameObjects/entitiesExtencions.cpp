#include "entitiesExtencions.h"

namespace craft{

    //Transform

    void Transform::updateTransformMatrix(){
        
        mainMat = glm::translate(glm::mat4(1.0f),position);
        mainMat = glm::rotate(mainMat,glm::radians(rotation.x),glm::vec3(1.0f,0,0));
        mainMat = glm::rotate(mainMat,glm::radians(rotation.y),{0,1.0f,0});
        mainMat = glm::rotate(mainMat,glm::radians(rotation.z),{0,0,1.0f});
        mainMat = glm::scale(mainMat,scale);
    }

    std::vector<std::shared_ptr<Transform>> Transform::s_instancies{};

    const extencion Transform::s_extencionLabel = Extencions::CR_EXTENCION_TRANSFORM; 


    //Upadate

    std::vector<std::shared_ptr<Update>> Update::s_instancies{};

    const extencion Update::s_extencionLabel = Extencions::CR_EXTENCION_UPDATE; 

    //CameraController

    std::vector<std::shared_ptr<CameraController>> CameraController::s_instancies{};

    const extencion CameraController::s_extencionLabel = Extencions::CR_EXTENCION_CAMERACONTROLLER;

    //Renderer3D

    std::vector<std::shared_ptr<Drawable3D>> Drawable3D::s_instancies{};

    const extencion Drawable3D::s_extencionLabel = Extencions::CR_EXTENCION_DRAWABLE3D;

    void Drawable3D::setDrawStatus(ShouldDraw drawStatus){
        m_drawStatus = drawStatus;
    }
    
    void Drawable3D::setMeshAndTransform(Mesh* mesh, Transform* transform){
        if(mesh == nullptr)
            LOG("A setMeshAndTransform with a nullptr as Mesh has been call...",5,0)

        if(transform == nullptr)
            LOG("A setMeshAndTransform with a nullptr as transform has been call...",5,0)
        
        m_mesh = mesh;
        m_transform = transform;
    }

    //std::function<bool(Entity* attachment)> drawCondition
    void Drawable3D::draw(){

        if(m_mesh == nullptr){
            LOG("Drawable3D::draw has benn call but there is no mesh to draw...",5,0)
            return;
        }

        if(m_transform == nullptr)
            m_transform = getExtencion<Transform>(attachment);
        
        switch (m_drawStatus)
        {
        case CR_DRAW_STATUS_NEVER_DRAW:
            break;

        case CR_DRAW_STATUS_ALWAYS_DRAW:
       
            m_transform->updateTransformMatrix();
            vk_renderer::get().createDrawCall({m_mesh,m_transform->mainMat,m_texture});
            break;

        case CR_DRAW_STATUS_USER_HANDEL_DRAW:
            if(!drawCondition)
                break;
            m_transform->updateTransformMatrix();
            vk_renderer::get().createDrawCall({m_mesh,m_transform->mainMat,m_texture});
            break;
        
        case CR_DRAW_STATUS_ENGINE_HANDEL_DRAW:
            break;

        default:
            break;
        }
    }

    Drawable3D::Drawable3D(Mesh* mesh,Texture* txt, Transform* transform) : m_mesh(mesh), m_transform(transform),m_texture(txt), drawCondition(defaultDrawCondition){
        if(mesh == nullptr)
            LOG("A Renderer3D with a nullptr as Mesh has been initialized...",5,0)

        if(transform == nullptr)
            LOG("A Renderer3D with a nullptr as Transform has been initialized...",1,0)
    }       
}
