#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

#include "../glm/glm.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../utils/utils.hpp"
#include "../debug/log.h"
#include "extencionsLabels.h"
#include "entity.h"


namespace craft{

    struct Transform{
    public:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale{1,1,1};

        glm::mat4 mainMat;

        void updateTransformMatrix();

        Entity* attachment;

        const static extencion s_extencionLabel;

        static std::vector<Transform> s_instancies;
    
    };

    struct Update{

        std::function<void*(Entity*)> function;

        Entity* attachment;

        static std::vector<Update> s_instancies;

        const static extencion s_extencionLabel;

    };

    //TODO: All the speed are being multiplied by a constant framerate, this is not good
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

        static std::vector<CameraController> s_instancies;

        const static extencion s_extencionLabel;
    };


    template<typename Extencion, typename ...args>
    Extencion* attachExtencion(Entity& entt,args& ...initializationData){

        Extencion::s_instancies.emplace_back(initializationData...);
        Extencion* newExtencion = &Extencion::s_instancies.back();  
        if(entt.extencions[Extencion::s_extencionLabel] != nullptr){
            LOGI("Entitie already has extencion: N " + std::to_string(Extencion::s_extencionLabel) + "... If you want to replace the extencion with a new one use replaceExtencion<> instead",1,1);
            return static_cast<Extencion*>(entt.extencions[Extencion::s_extencionLabel]);
        }
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
    Extencion* attachExtencion(Entity* entt,args& ...initializationData){

        Extencion::s_instancies.emplace_back(initializationData...);
        Extencion* newExtencion = &Extencion::s_instancies.back();  
        if(entt->extencions[Extencion::s_extencionLabel] != nullptr){
            LOGI("Entitie already has extencion: N " + std::to_string(Extencion::s_extencionLabel) + "... If you want to replace the extencion with a new one use replaceExtencion<> instead",1,1);
            return static_cast<Extencion*>(entt->extencions[Extencion::s_extencionLabel]);
        }
        entt->extencions[Extencion::s_extencionLabel] = newExtencion;
        newExtencion->attachment = entt;
        return newExtencion;
    }
}