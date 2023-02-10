#pragma once

#include <string>
#include <chrono>

class Timer
{
public:

    Timer(std::string name);

    void Stop();

private:

    std::string m_Name;
    std::chrono::high_resolution_clock::time_point m_StartTime;

};