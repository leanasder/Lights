#ifndef TRAFFIC_CONTROLLER_H
#define TRAFFIC_CONTROLLER_H

#include "colored_output.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std::chrono_literals;

//Preliminary announcement class SynchronizedTrafficLight;
class SynchronizedTrafficLight;

class TrafficController { 
    std::vector<TrafficLightBase*> lights;
    std::atomic<bool> isRunning{false};
    std::atomic<bool> isStopping{false};
    std::thread controllerThread;

    std::mutex mtx;
    std::condition_variable cv;
    bool allLightsReady{false};

    std::chrono::seconds greenDuration{5};
    std::chrono::seconds yellowDuration{2};

    void controlCycle();

public:
    TrafficController(std::chrono::seconds greenTime = 5s,
                    std::chrono::seconds yellowTime = 2s);
    ~TrafficController();

    void registerLight(TrafficLightBase* light);
    void start();
    void stop();

    bool waitForSignal(SynchronizedTrafficLight* light);
    bool isControllerRunning() const;
};

#endif //TRAFFIC_CONTROLLER_H