#include "frameRate.h"

namespace craft{

    void frameRate::wait() {

        start = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> work_time = start - finish;

        if (work_time.count() < 16.666667f)
        {
            std::chrono::duration<double, std::milli> delta_ms(16.666667f - work_time.count());
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