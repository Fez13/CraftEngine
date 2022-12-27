#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtx/quaternion.hpp"
#include "../vendor/glm/gtc/quaternion.hpp"
#include "../utils/utils.hpp"
#include "../debug/log.h"
#include "extencionsLabels.h"
#include "entity.h"
#include "../rendered/mesh.h"
#include "../rendered/vk_renderer.h"
#include "../gpu/texture.h"


namespace craft{

    struct Transform{
    public:
        glm::vec3 position{0,0,0};
        glm::vec3 rotation{0,0,0};
        glm::vec3 scale{1,1,1};

        glm::mat4 mainMat;

        void updateTransformMatrix();

        Entity* attachment;

        const static extencion s_extencionLabel;

        static std::vector<std::shared_ptr<Transform>> s_instancies;
    
        ~Transform(){LOG_COMPONENT_DESTRUCTION("Transform")}

    };

    inline void* defaultEmptyFuntion(Entity* attachment){return nullptr;}

    struct Update{

        std::function<void*(Entity*)> function;

        Update() : function(defaultEmptyFuntion){};
        Update(std::function<void*(Entity*)> function) : function(function){}

        Entity* attachment;

        static std::vector<std::shared_ptr<Update>> s_instancies;

        const static extencion s_extencionLabel;

        ~Update(){LOG_COMPONENT_DESTRUCTION("Update")}

    };

    
    using ShouldDraw = std::uint16_t;
    enum : ShouldDraw{
            CR_DRAW_STATUS_NEVER_DRAW = 0,
            CR_DRAW_STATUS_ALWAYS_DRAW = 1,
            CR_DRAW_STATUS_USER_HANDEL_DRAW = 2,
            CR_DRAW_STATUS_ENGINE_HANDEL_DRAW = 3,            
        };


    inline bool defaultDrawCondition(Entity* attachment){return false;}
    struct Drawable3D{

        public:

            Drawable3D(Mesh* mesh,Texture* texture, Transform* transform = nullptr);
            Drawable3D() : m_drawStatus(CR_DRAW_STATUS_NEVER_DRAW), drawCondition(defaultDrawCondition){};

            void setMeshAndTransform(Mesh* mesh, Transform* geometry);

            void setDrawStatus(ShouldDraw drawStatus);

            std::function<bool(Entity* attachment)> drawCondition;
            void draw();

            Entity* attachment;

            static std::vector<std::shared_ptr<Drawable3D>> s_instancies;

            const static extencion s_extencionLabel;
    
            Mesh* m_mesh = nullptr;

            Transform* m_transform = nullptr;

            ~Drawable3D(){LOG_COMPONENT_DESTRUCTION("Drawable3D")}


        private:
        ShouldDraw m_drawStatus = CR_DRAW_STATUS_ALWAYS_DRAW;

        Geometry* m_geometry;
        Texture* m_texture;
        
        //TODO: Material* m_material;

    };

    struct CameraController{

        float m_camera_speed = 1.0f;
        float m_camera_sens = 1.0f;
        void *m_window;

        float m_nearPlane;
        float m_farPlane;
        float m_aspectRation;
        float m_fov;

        glm::vec3 forward = glm::vec3(0, 0, -1);
        const glm::vec3 up = glm::vec3(0, 1, 0);

        Entity* attachment;

        static std::vector<std::shared_ptr<CameraController>> s_instancies;

        const static extencion s_extencionLabel;

        ~CameraController(){LOG_COMPONENT_DESTRUCTION("CameraController")}

    };


    template<typename Extencion, typename ...args>
    Extencion* attachExtencion(Entity& entt,args ...initializationData){

        if(entt.extencions[Extencion::s_extencionLabel] != nullptr){
            LOGI("Entitie already has extencion: N " + std::to_string(Extencion::s_extencionLabel) + "... If you want to replace the extencion with a new one use replaceExtencion<> instead",1,1);
            return static_cast<Extencion*>(entt.extencions[Extencion::s_extencionLabel]);
        }
        Extencion::s_instancies.push_back(std::make_shared<Extencion>(initializationData...));
        Extencion* newExtencion = Extencion::s_instancies.back().get();  
        entt.extencions[Extencion::s_extencionLabel] = newExtencion;
        newExtencion->attachment = &entt;
        return newExtencion;
    }

    template<typename Extencion, typename ...args>
    Extencion* replaceExtencion(Entity& entt,args& ...initializationData){
        
        Extencion* oldExtencion = static_cast<Extencion*>(entt.extencions[Extencion::s_extencionLabel]);

        auto iter = std::find_if(Extencion::s_instancies.begin(), Extencion::s_instancies.end(),
                             [&](const Extencion& e){return &e == oldExtencion;});


        if(iter != Extencion::s_instancies.end()){
            Extencion::s_instancies.erase(iter);
            entt.extencions[Extencion::s_extencionLabel] = nullptr;
            delete oldExtencion;
        }else{
            LOGI("Entitie dosent contain the extencion that you are traing to replace a new one will be added instead...",1,1);
        }
        return attachExtencion<Extencion,args...>(entt,initializationData...);
    }

    template<typename Extencion>
    Extencion* getExtencion(Entity& entt){
        Extencion* extencion = static_cast<Extencion*>(entt.extencions[Extencion::s_extencionLabel]);
        if(extencion != nullptr)
            return extencion;
        LOGI("Entitie dosent contain the extencion that you are traing to get...",5,0);
        return nullptr;
    }

    template<typename Extencion>
    Extencion* getExtencion(Entity* entt){
        Extencion* extencion = static_cast<Extencion*>(entt->extencions[Extencion::s_extencionLabel]);
        if(extencion != nullptr)
            return extencion;
        LOGI("Entitie dosent contain the extencion that you are traing to get...",5,0);
        return nullptr;
    }

    template<typename Extencion>
    Extencion* getExtencion(std::unordered_map<extencion,void*> &extencions){
        Extencion* extencion = static_cast<Extencion*>(extencions[Extencion::s_extencionLabel]);
        if(extencion != nullptr)
            return extencion;
        LOGI("Entitie dosent contain the extencion that you are traing to get...",5,0);
        return nullptr;
    }

    template<typename Extencion, typename ...args>
    Extencion* attachExtencion(Entity* entt,args ...initializationData){
        if(entt->extencions[Extencion::s_extencionLabel] != nullptr){
            LOGI("Entitie already has extencion: N " + std::to_string(Extencion::s_extencionLabel) + "... If you want to replace the extencion with a new one use replaceExtencion<> instead",1,1);
            return static_cast<Extencion*>(entt->extencions[Extencion::s_extencionLabel]);
        }
        Extencion::s_instancies.push_back(std::make_shared<Extencion>(initializationData...));
        Extencion* newExtencion = Extencion::s_instancies.back().get();  
        entt->extencions[Extencion::s_extencionLabel] = newExtencion;
        newExtencion->attachment = entt;
        return newExtencion;
    }
}