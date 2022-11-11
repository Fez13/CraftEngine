#pragma once
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace craft{

    using shaderType = std::uint8_t;

    //I know that there are more types, that's why this is here, Unfortunately I couldn't the types and the glslc "-fshader-stage" types so...
    //TODO:
    enum : shaderType{
        COMPUTE = 0,
        EMPTY = 255
    };

    int compile(const char* path,shaderType type,std::string &newPath,bool terminal = false);

}