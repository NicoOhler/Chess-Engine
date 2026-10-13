#pragma once

#include "definitions.h"
#include "Logger.h"
#include <string>
#include <iostream>

std::string convertMillisecondsToString(Milliseconds milliseconds);
Milliseconds getMilliseconds();

class Timer
{
public:
    void start();
    Milliseconds stop(LogType log_type = TIME);

private:
    Milliseconds start_time;
    Milliseconds end_time;
};
