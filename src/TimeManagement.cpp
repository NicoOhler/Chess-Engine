#include "TimeManagement.h"

void TimeManagement::start(bool white_to_move)
{
    this->white_to_move = white_to_move;
    updateAllocatedTime();
    start_time = getMilliseconds();
}

Milliseconds TimeManagement::elapsedTime()
{
    return getMilliseconds() - start_time;
}

void TimeManagement::updateAllocatedTime()
{
    if (!configured)
        return;

    if (move_time > 0)
    {
        allocated_time = move_time;
        return;
    }

    if (white_to_move)
        allocated_time = white_time / FRACTION_OF_TIME_TO_ALLOCATE + white_increment;
    else
        allocated_time = black_time / FRACTION_OF_TIME_TO_ALLOCATE + black_increment;
}

void TimeManagement::reset()
{
    configured = false;
    move_time = DEFAULT_SEARCH_TIME;
    white_time = 0;
    black_time = 0;
    white_increment = 0;
    black_increment = 0;
}

bool TimeManagement::timeLeft()
{
    if (!configured)
        return true;

    return elapsedTime() < allocated_time;
}

void TimeManagement::configure(Milliseconds move_time)
{
    if (move_time <= 0)
    {
        log(TIME, "Invalid move time. Please specify a positive integer for the move time.");
        return;
    }
    this->move_time = move_time;
    configured = true;
}

void TimeManagement::configure(Milliseconds white_time, Milliseconds black_time)
{
    configure(white_time, black_time, 0, 0);
}

void TimeManagement::configure(Milliseconds white_time, Milliseconds black_time, Milliseconds white_increment, Milliseconds black_increment)
{
    if (white_time <= 0 || black_time <= 0 || white_increment < 0 || black_increment < 0)
    {
        log(TIME, "Invalid time control. Please specify positive integers for the white and black times, and non-negative integers for the increments.");
        return;
    }
    this->white_time = white_time;
    this->black_time = black_time;
    this->white_increment = white_increment;
    this->black_increment = black_increment;
    configured = true;
}
