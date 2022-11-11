#include "compile_Spir_V.h"

namespace craft{

    int compile(const char* path,shaderType type,std::string &newPath, bool terminal){



        if(!std::filesystem::exists(path)){
            std::cout<<"couldn't open file...\n path: ";
            std::cout<<path;
            return 1;
        }

        std::string command = "glslc ";
        switch (type) {
            case 0:
                command += "-fshader-stage=compute ";
                break;
            case 255:
                command += " ";
                break;

            default:
                command += "-fshader-stage=compute ";
                break;
        }
        command += path;
        command += " -o ";
        command += newPath;
        int end = std::system(command.c_str());

        if(end != 0)
            std::cout<<"Error while compiling\n"<<" Shader path: "<<path;

        if(!std::filesystem::exists(newPath)){
            std::cout<<"couldn't compile shader...\n path: ";
            std::cout<<path<<'\n';
            if(terminal)
                exit(1);
            return 1;
        }
        return 0;
    }
};