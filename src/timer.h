#ifndef TIMER_H
#define TIMER_H

#include <ctime>

class Timer {
   public:
    Timer() { m_preTime = clock(); }

    void recordCurrent() { m_preTime = clock(); }

    long getTimePass() {
        return 1.0 * (clock() - m_preTime) / CLOCKS_PER_SEC * 1000;
    }

    const static int TIME_OUT = __INT32_MAX__;
    time_t m_preTime;
};

#endif