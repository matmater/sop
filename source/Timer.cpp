/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#include "Timer.h"

Timer::Timer()
{
    Restart();
}

Timer::~Timer()
{

}

void Timer::Restart()
{
    mStartTime = std::chrono::high_resolution_clock::now();
}

Real Timer::GetTimeElapsed()
{
    std::chrono::duration<Real> dt = (std::chrono::high_resolution_clock::now() - mStartTime);

    return dt.count();
}
