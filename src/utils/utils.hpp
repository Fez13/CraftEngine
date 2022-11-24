#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <chrono>



template<typename T>
bool inVector(const std::vector<T> &vector,const T &obj,uint32_t &index){
    for(uint32_t i = 0; i < vector.size(); i++)
        if(vector[i] == obj){
            index = i;
            return true;
        }
    return false;
}



