#pragma once

#include "definitions.h"
#include "Logger.h"
#include <string>
#include <iostream>

class Timer
{
public:
    Milliseconds limit = DEFAULT_SEARCH_TIME;

    void start();
    Milliseconds stop(LogType log_type = TIME);
    bool timeLeft();

private:
    Milliseconds start_time;
    Milliseconds end_time;
    Milliseconds getMilliseconds();
    std::string convertMillisecondsToString(Milliseconds milliseconds);
};
