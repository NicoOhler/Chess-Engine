#pragma once
#include "Engine.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

class UCI
{
public:
    UCI(Engine &engine) : engine(engine) {}
    void start();

private:
    Engine &engine;

    // background thread and synchronization
    std::thread background_thread;
    std::condition_variable background_request_cv;
    std::mutex background_request_mutex;
    std::mutex engine_mutex;
    std::atomic<bool> quit_requested{false};
    std::atomic<bool> search_requested{false};
    void handleBackgroundRequests();

    // UCI command handlers
    void handleUCI();
    void handleIsReady();
    void handleUCINewGame();
    void handlePosition(const std::string &command);
    void handleGo(const std::string &command);
    void handleStop();
    void handleQuit();

    std::string extractArgumentFromCommand(const std::string &command, const std::string &prefix, const std::string &suffix = " ");
    Milliseconds extractMillisecondsFromString(const std::string &string);
};