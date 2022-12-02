#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <memory>

namespace craft{

    template<typename T>
    bool inVector(const std::vector<T> &vector,const T &obj,uint32_t &index){
        for(uint32_t i = 0; i < vector.size(); i++)
            if(vector[i] == obj){
                index = i;
                return true;
            }
        return false;
    }


    // Only numerical types
    template <typename T>
    inline void Clamp(T &value, T max, T min)
    {
        if (value > max)
            value = max;

        if (value < min)
            value = min;
    }

    template <class T>
    inline void RemoveFromVector(T object, std::vector<T> &vector)
    {
        for (int i = 0; i < vector.size(); i++)
            if (vector[i] == object)
            {
                vector.erase(vector.begin() + i);
                return;
            }
    }

    template <class T, class U>
    inline U *FindPointerInVector(T object, std::vector<U *> &vector)
    {
        for (int i = 0; i < vector.size(); i++)
            if (vector[i] == object)
                return vector[i];
        return nullptr;
    }

    template <class T>
    inline bool VectorContains(T object, std::vector<T> &vector)
    {
        for (int i = 0; i < vector.size(); i++)
            if (vector[i] == object)
                return 1;
        return 0;
    }

    // Returns a pointer to an object in a vector with a certain tag
    // the class inputed has to contain a .tag type
    template <class T>
    inline T *VectorContainsTag(std::string tag, std::vector<T *> &vector)
    {
        for (int i = 0; i < vector.size(); i++)
            if (vector[i]->tag == tag)
                return vector[i];
        return nullptr;
    }

    template <class T>
    inline std::shared_ptr<T> shVectorContainsTag(std::string tag, std::vector<std::shared_ptr<T>> &vector)
    {
        for (int i = 0; i < vector.size(); i++)
            if (vector[i].get()->tag == tag)
                return vector[i];
        return nullptr;
    }

    /* TODO
    inline std::vector<glm::vec2> MeshToVector(const std::vector<Vertex> vertices)
    {

        std::vector<glm::vec2> out(vertices.size());

        for (int i = 0; i < vertices.size(); i++)
            out[i] = glm::vec2(vertices[i].position);

        return out;
    }
    */


}