#include "Timer.h"
#include <chrono>

Milliseconds Timer::getMilliseconds()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

void Timer::start()
{
    start_time = getMilliseconds();
}

Milliseconds Timer::stop(LogType log_type)
{
    end_time = getMilliseconds();
    log(log_type, "Time taken: " + convertMillisecondsToString(end_time - start_time));
    return end_time;
}

bool Timer::timeLeft()
{
    return start_time + limit > getMilliseconds();
}

std::string Timer::convertMillisecondsToString(Milliseconds milliseconds)
{
    int hours = milliseconds / 3600000;
    milliseconds %= 3600000;
    int minutes = milliseconds / 60000;
    milliseconds %= 60000;
    int seconds = milliseconds / 1000;
    milliseconds %= 1000;
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s " + std::to_string(milliseconds) + "ms";
}
