#include "Logger.h"

Logger *Logger::instance = nullptr;

Logger::Logger()
{
    print_to_console = LOG_TO_CONSOLE;
    filename = LOG_FILE;

    if (APPEND_TO_FILE)
        file.open(filename, std::ios_base::app);
    else
        file.open(filename);
}
Logger::~Logger()
{
    if (file.is_open())
        file.close();
}

Logger *Logger::GetInstance()
{
    if (!instance)
        instance = new Logger();
    return instance;
}

void Logger::log(LogType logType, std::string message)
{
    if (print_to_console)
        std::cout << message << std::endl;

    if (file.is_open())
        file << message << std::endl;
}

void log(LogType logType, std::string message)
{
    if (logType & LOG_TYPE_ENABLED)
        Logger::GetInstance()->log(logType, message);
}
