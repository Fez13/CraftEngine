#include <iostream>

#include "src/App.h"

int main() {

    const std::vector<std::string> layers = {"VK_LAYER_KHRONOS_validation"};

    int close = 1;
    while(close){
        craft::App app("Engine", VK_MAKE_VERSION(1,0,0),VK_API_VERSION_1_3,layers);
        app.setDebug(true);
        close = app.mainLoop();
        app.clean();
    }

    return 0;
}
