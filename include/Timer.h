/*!
 *  This file is part of a speaker recognition group project (SOP, 2015-2016)
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include "Common.h"

/*! \class Timer
 *  \brief Measures elapsed time in seconds.
 */
class Timer
{
public:
    /*! \brief Default constructor.
     *
     *  Starts the timer.
     */
    Timer();

    /*! \brief Virtual destructor.
     */
    virtual ~Timer();

    /*! \brief Restart the timer.
     */
    void Restart();

    /*! \brief Get time elapsed from the construction of
     *  the timer or from restart in seconds.
     *
     *  \return Elapsed time in seconds.
     */
    Real GetTimeElapsed();

private:
    std::chrono::high_resolution_clock::time_point mStartTime;
};

#endif
