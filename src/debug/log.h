#pragma once
#include <string>
#include <iostream>

namespace craft{

    #ifndef ERROR_LEVEL
    #define ERROR_LEVEL 0.0f
    #endif

    #ifndef EXIT_LEVEL
    #define EXIT_LEVEL 0.0f
    #endif

    #ifndef RELEASE
    //3 arguments, the error text, the level of priority and the state:
    //  -1 for error
    //  0 for warning
    //  any other for comments
    #define LOG(text,level,state) if(level >= ERROR_LEVEL){ \
    if(state == -1) std::cout<<"\033[1;31m"<< "Error"; else if(state == 0) std::cout<<"\033[1;33m"<<"Warning"; else if(state == 1) std::cout<<"\033[1;36m"<<"Comment";\
    std::cout<<" in file: "<<__FILE__<<", line: "<<__LINE__<<"\n\t Data: "<<text<<'\n'<<"\033[0m";}

    //2 arguments, the error text, and the level of the error,
    // if the level is below the definition of EXIT_LEVEL, the error won't be thrown
    #define LOG_TERMINAL(text,level) \
   if (1 >= 0.0f) {std::cout << "\033[1;31m" << "Terminal error"; \
   std::cout << " in file: " << __FILE__ << ", line: " << __LINE__ << "\n\t Data: " << text << '\n'<< "\033[0m";throw std::runtime_error("Fatal error");} \
   else {std::cout << "\033[1;31m"<< "A terminal error was shoot, however, the EXIT_LEVEL is of the error is below the definition..."<< '\n' << "\033[0m";}

#else
    #define LOG(error,level,state,exit);

    #define LOG_TERMINAL(text,level) \
    std::cout<<"\033[1;31m"<< "Terminal error";\
    std::cout<<" in file: "<<__FILE__<<", line: "<<__LINE__<<"\n\t Data: "<<text<<'\n'<<"\033[0m"; exit(1);

#endif
}
