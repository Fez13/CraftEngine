#include <iostream>

#include "src/App.h"

int main() {

    const std::vector<std::string> layers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<std::string> extensions = {"VK_KHR_xcb_surface","VK_KHR_surface","VK_KHR_display"};


    int close = 1;
    while(close){
        try{
        craft::App app("Engine", VK_MAKE_VERSION(1,0,0),VK_API_VERSION_1_3,layers,extensions);
        close = app.mainLoop();
        app.clean();
        } catch (const std::exception &error){
            std::cerr<<error.what()<<'\n';
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
