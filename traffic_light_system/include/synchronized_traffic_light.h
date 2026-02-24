
#ifndef SYNCHRONIZED_TRAFFIC_LIGHT_H
#define SYNCHRONIZED_TRAFFIC_LIGHT_H

#include "colored_output.h"
#include <atomic>
#include <memory>
#include <thread>

class TrafficController;

class SynchronizedTrafficLight {
    std::atomic<TrafficColor> currentColor{TrafficColor::Red};
    std::atomic<bool> isRunning{false};
    std::unique_ptr<std::thread> workerThread;
    
    TrafficController* controller;
    int lightId;
    
    void cycleColors();
    
public:
    SynchronizedTrafficLight(int id, TrafficController* ctrl);
    ~SynchronizedTrafficLight();
    
    void start();
    void stop();
    
    TrafficColor getCurrentColor() const { return currentColor.load(); }
    void setColor(TrafficColor color);
    int getId() const { return lightId; }
    
    SynchronizedTrafficLight(const SynchronizedTrafficLight&) = delete;
    SynchronizedTrafficLight& operator=(const SynchronizedTrafficLight&) = delete;
};

#endif

