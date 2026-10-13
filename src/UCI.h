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

    // engine worker thread and synchronization
    std::thread engine_worker_thread;
    std::condition_variable event_cv;
    std::mutex event_queue_mutex;
    std::mutex engine_mutex;
    std::atomic<bool> quit_requested{false};
    std::atomic<bool> search_requested{false};
    void workerLoop();

    // UCI command handlers
    void handleUCI();
    void handleIsReady();
    void handleUCINewGame();
    void handlePosition(std::string parameters);
    void handleGo(std::string parameters);
    void handleStop();
    void handleQuit();

    std::string extractArgumentFromCommand(const std::string &command, const std::string &prefix, const std::string &suffix);
    Milliseconds parseTimeString(const std::string &time_string);
};