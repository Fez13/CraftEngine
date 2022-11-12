#pragma once
#include <string>
#include <iostream>


namespace craft{

    #ifndef ERROR_LEVEL
    #define ERROR_LEVEL 0.0f
    #endif

    #ifndef RELEASE
    //4 arguments, the error text, the line of the error, the level of priority and the state;
    //  -1 for error
    //  0 for warning
    //  any other for comments
    #define LOG(text,level,state) if(level >= ERROR_LEVEL){ \
    if(state == -1) std::cout<<"\033[1;31m"<< "Error"; else if(state == 0) std::cout<<"\033[1;33m"<<"Warning"; else if(state == 1) std::cout<<"\033[1;36m"<<"Comment";\
    std::cout<<" in file: "<<__FILE__<<", line: "<<__LINE__<<"\n\t Data: "<<text<<'\n'<<"\033[0m";}
    #else
    #define LOG(error,level,state);
    #endif
}
