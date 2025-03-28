#ifndef OBJECTTIME_H
#define OBJECTTIME_H


#include <ctime>
#include <chrono>
#include <iostream>
#include <vector>


class Time
{
    public:
    Time(const Time &) = delete;
    Time& operator=(const Time&) = delete;

    static Time * getInstance() {
        static Time instance;  // C++11 保证线程安全
        return &instance;
    }

    public:
    double d_current_time;
    double d_cpu_passed_time;

    void start();
    void getPassedCpuTimeFromLast();
    void getPassedCpuTimeFromStart();

    private:
    Time(){};
    std::vector<std::chrono::_V2::system_clock::time_point> d_cpu_time_list;


};






#endif