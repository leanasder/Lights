//05.03.2026

#pragma once

#include "colored_output.h"
#include "event.h"
#include <atomic>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>

class TrafficLightBase {
protected:
    int id;
    std::atomic<bool> isRunning{false};
    std::unique_ptr<std::thread> workerThread;

    // Queue events for this light
    std::queue <Event> eventQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    virtual void processEvent(const Event& event) = 0; // processing a single event
    virtual void processEvents() = 0; 
    virtual void handleEvent() = 0;

    // stream function - infinitely waits and processes events
    void eventLoop() {
        while (isRunning.load()) {
            std::unique_lock<std::mutex> lock(queueMutex);
            // wait until an event occurs or until it is stopped
            queueCV.wait(lock, [this] { return !eventQueue.empty() || !isRunning.load(); });

            if (!isRunning.load()) break;

            Event event = eventQueue.front();
            eventQueue.pop();
            lock.unlock();

            // processing event in successor
            processEvent(event);
        }
    }

public:
    TrafficLightBase(int id) : id(id) {
        registerLight(id, this); // auto registration in creation
    }
    virtual ~TrafficLightBase() {
        stop();
        unregisterLight(id); // removing from registry
    } ;

    virtual void start() {
        if (isRunning.load()) return;
        isRunning = true;
        workerThread = std::make_unique<std::thread> (&TrafficLightBase::eventLoop, this);
    }

    virtual void stop() {
        isRunning = false;
        queueCV.notify_all(); // wake up the thread to turns off
        if (workerThread && workerThread->joinable()) {
            workerThread->join();
        }
    }

    int getId() const { return id; }
    bool isLightRunning() const { return isRunning.load(); }

    // recieving the event (puts in queue)
    void receiveEvent(const Event& event) {
        std::lock_guard<std::mutex> lock(queueMutex);
        eventQueue.push(event);
        queueCV.notify_one();
    }


    // ================== Registry of lights (static) =========================
private:
    static std::map<int, TrafficLightBase*> registry;
    static std::mutex registryMutex;

    static void registerLight(int id, TrafficLightBase* light) {
        std::lock_guard<std::mutex> lock(registryMutex);
        registry[id] = light;
    }

    static void unregisterLight(int id) {
        std::lock_guard<std::mutex> lock(registryMutex);
        registry.erase(id);
    }

public:
    // Sending an event by ID (can be called from any thread)
    static bool sendEvent(int targetId, const Event& event) {
        std::lock_guard<std::mutex> lock(registryMutex);
        auto it = registry.find(targetId);
        if (it != registry.end()) {
            it->second->receiveEvent(event);
            return true;
        }
        return false; // recipient not found
    }

    // Synchronously receiving a pointer to another traffic light by ID
    static TrafficLightBase* getLight(int id) {
        std::lock_guard<std::mutex> lock(registryMutex);
        auto it = registry.find(id);
        if (it != registry.end())
            return it->second;
        return nullptr;
    }

    // virtual methods for getting state (overridden in descendants)
    virtual TrafficColor getCurrentColor() const = 0;
    virtual int getQueueLength() const = 0;
};