#include "frameRate.h"

namespace craft{

    double frameRate::getElapsedTime() const{
        return m_elapsedTime;
    }

    void frameRate::wait() {

        start = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> work_time = start - finish;

        m_elapsedTime = work_time.count(); 

        if (work_time.count() < (1000 * (1.0f / frameObjective)))
        {
            std::chrono::duration<double, std::milli> delta_ms((1000 * (1.0f / frameObjective)) - work_time.count());
            auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
        }

        finish = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> sleep_time = finish - start;

    }

    frameRate::frameRate() {
        start = std::chrono::system_clock::now();
        finish = std::chrono::system_clock::now();
        frameObjective = 60;
    }
}