#include "entitiesExtencions.h"

namespace craft{

void Transform::updateTransformMatrix(){

/*
    glm::mat4 trs = glm::translate(glm::mat4(1.0f),position);
    
    glm::quat rotation_quat(rotation);
    
    glm::mat4 rtt = glm::toMat4(rotation_quat);

    glm::mat4 rtt_ = glm::rotate(glm::mat4(1.0f),glm::radians(rotation.x),{1.0f,0,0});
    rtt_ = glm::rotate(rtt_,glm::radians(rotation.y),{0,1.0f,0});
    rtt_ = glm::rotate(rtt_,glm::radians(rotation.z),{0,0,1.0f});



    glm::mat4 scl = glm::scale(glm::mat4(1.0f),scale);    
*/

    mainMat = glm::translate(glm::mat4(1.0f),position);
    mainMat = glm::rotate(mainMat,glm::radians(rotation.x),glm::vec3(1.0f,0,0));
    mainMat = glm::rotate(mainMat,glm::radians(rotation.y),{0,1.0f,0});
    mainMat = glm::rotate(mainMat,glm::radians(rotation.z),{0,0,1.0f});
    mainMat = glm::scale(mainMat,scale);

}


}
