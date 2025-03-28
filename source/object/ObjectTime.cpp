#include"ObjectTime.h"


void Time::start()
{
    auto cpu_time = std::chrono::high_resolution_clock::now();
    d_cpu_time_list.push_back(cpu_time);
}

void Time::getPassedCpuTimeFromLast()
{
    auto cpu_time = std::chrono::high_resolution_clock::now();
    d_cpu_time_list.push_back(cpu_time);
    int size = d_cpu_time_list.size();
    std::chrono::duration<double, std::milli> duration = d_cpu_time_list[size - 1] - d_cpu_time_list[size - 2]; 
    std::cout << "time of read input data: " << duration.count() << " ms" << std::endl;

}


void Time::getPassedCpuTimeFromStart()
{
    auto cpu_time = std::chrono::high_resolution_clock::now();
    d_cpu_time_list.push_back(cpu_time);
    int size = d_cpu_time_list.size();
    std::chrono::duration<double, std::milli> duration = d_cpu_time_list[size - 1] - d_cpu_time_list[0]; 
    std::cout << "time of read input data: " << duration.count() << " ms" << std::endl;
}