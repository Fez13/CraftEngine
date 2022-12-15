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

    std::vector<Transform> Transform::s_instancies;

    const extencion Transform::s_extencionLabel = Extencions::CR_TRANSFORM_EXTENCION; 


    //Upadate

    std::vector<Update> Update::s_instancies;

    const extencion Update::s_extencionLabel = Extencions::CR_UPDATE_EXTENCION; 

    //CameraController

    std::vector<CameraController> CameraController::s_instancies;

    const extencion CameraController::s_extencionLabel = Extencions::CR_CAMERACONTROLLER_EXTENCION;

    //Camera

   

}
