#pragma once
#include "Timer.h"

class TimeManagement
{
private:
    Milliseconds start_time;
    Milliseconds allocated_time = DEFAULT_SEARCH_TIME;
    Milliseconds move_time = 0;
    Milliseconds white_time = 0;
    Milliseconds black_time = 0;
    Milliseconds white_increment = 0;
    Milliseconds black_increment = 0;

    bool white_to_move;
    bool configured = false;
    void updateAllocatedTime();

public:
    void start(bool white_to_move);
    void reset();
    bool timeLeft();
    bool sufficientTimeForNextIterationLeft();
    Milliseconds getElapsedTime();
    void configure(Milliseconds move_time);
    void configure(Milliseconds white_time, Milliseconds black_time, Milliseconds white_increment, Milliseconds black_increment);
};