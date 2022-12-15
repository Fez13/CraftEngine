#pragma once

#include<iostream>
#include<unordered_map>

#include"extencionsLabels.h"


namespace craft{



    class Entity {

    public:
            
        Entity() = default;

        std::unordered_map<extencion,void*> extencions;
    

    };

}