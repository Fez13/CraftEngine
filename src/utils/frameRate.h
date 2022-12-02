#pragma once
#include <chrono>
#include <thread>

namespace craft{

    class frameRate {
    public:

        frameRate();

        float frameObjective;

        void wait();

    private:
        std::chrono::system_clock::time_point start;
        std::chrono::system_clock::time_point finish;

    };

}