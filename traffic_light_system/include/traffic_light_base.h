#pragma once

#include "colored_output.h"
#include <atomic>
#include <memory>
#include <thread>

class TrafficLightBase {
protected:
    int id;
    std::atomic<bool> isRunning{false};
    std::unique_ptr<std::thread> workerThread;

    virtual void processEvents() = 0; 
    virtual void handleEvent();
public:
    TrafficLightBase(int id) : id(id) {}
    virtual ~TrafficLightBase() = default;

    virtual void start() {
        if (isRunning.load()) return;
        isRunning = true;
    }

    virtual void stop() {
        isRunning = false;
        if (workerThread && workerThread->joinable()) {
            workerThread->join();
        }
    }

    int getId() const { return id; }
    bool isLightRunning() const { return isRunning.load(); }
};