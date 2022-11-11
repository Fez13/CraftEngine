#pragma once
#include <vector>
#include <chrono>
#include <string>

namespace craft{

    using timePoint = std::chrono::steady_clock::time_point;

    inline std::vector<std::pair<std::string,timePoint>> logTimes;

#ifndef RELEASE
    inline long getElapsedTime(const std::string &name){

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

        for(auto& obj : logTimes)
            if(obj.first == name){
                long time = std::chrono::duration_cast<std::chrono::nanoseconds> (now - obj.second).count();
                obj.second = now;
                return time;
            }
        logTimes.emplace_back(name,now);
        return 0;
    }
    inline void cleanElapsedTimes(long minTime){
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        std::vector<std::pair<std::string,timePoint>> newList;

        for(auto& obj : logTimes){
            long time = std::chrono::duration_cast<std::chrono::nanoseconds> (now - obj.second).count();
            if(time >= minTime)
                newList.push_back(obj);

            }
        logTimes = newList;
    }
#else
    inline long getElapsedTime(const std::string &name){
        return -1;
    }
    void cleanElapsedTimes();
#endif


}