#ifndef OBJECTTimeMASTER_H
#define OBJECTTIMEMASTER_H


#include <ctime>
#include <chrono>
#include <iostream>
#include <vector>


class TimeMater
{
    public:
    TimeMater(const TimeMater &) = delete;
    TimeMater& operator=(const TimeMater&) = delete;

    static TimeMater * getInstance() {
        static TimeMater instance;  // C++11 保证线程安全
        return &instance;
    }

    public:

    void start();
    void getPassedCpuTimeFromLast();
    void getPassedCpuTimeFromStart();

    private:
    TimeMater(){};
    std::vector<std::chrono::_V2::system_clock::time_point> d_cpu_time_list;


};






#endif