#pragma once
#include <chrono>
#include <thread>

namespace craft{

    class frameRate {
    public:

        frameRate();

        float frameObjective;

        //Time of 1 cycle in miliseconds
        double getElapsedTime() const;

        void wait();

    private:
        double m_elapsedTime = 0;

        std::chrono::system_clock::time_point start;
        std::chrono::system_clock::time_point finish;

    };

}