#ifndef _TIMER_H_
#define _TIMER_H_

#include "Common.h"

class Timer
{
public:
    Timer();

    virtual ~Timer();

    void Restart();

    Real GetTimeElapsed();

private:
    std::chrono::high_resolution_clock::time_point mStartTime;
};

#endif