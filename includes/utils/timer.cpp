#include "Timer.h"

#include <iostream>

Timer::Timer(std::string name)
    : m_Name(name), m_StartTime(std::chrono::high_resolution_clock::now()) {}

double Timer::Stop(bool log)
{
    using namespace std::chrono;

    duration<double> duration(high_resolution_clock::now() - m_StartTime);
    std::cout << m_Name << " finished in " << duration.count()*1000 << "ms" << std::endl;
    return duration.count()*1000;
}