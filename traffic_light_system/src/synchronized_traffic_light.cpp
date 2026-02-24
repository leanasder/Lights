#include "synchronized_traffic_light.h"
#include "traffic_controller.h"
#include <chrono>

using namespace std::chrono_literals;

SynchronizedTrafficLight::SynchronizedTrafficLight(int id, TrafficController* ctrl)
    : lightId(id), controller(ctrl) {
}

SynchronizedTrafficLight::~SynchronizedTrafficLight() { 
    stop(); 
}

void SynchronizedTrafficLight::setColor(TrafficColor color) { 
    currentColor = color; 
    ColoredOutput::print(lightId, color, "Color changed");
}

void SynchronizedTrafficLight::cycleColors() {
    ColoredOutput::print(lightId, TrafficColor::Red, "Light started");

    while (isRunning.load() && controller->isControllerRunning()) {
        if (!controller->waitForSignal(this)) {
            break;
        }
        std::this_thread::sleep_for(100ms);
    }
    
    ColoredOutput::print(lightId, currentColor.load(), "Light stopped");
}

void SynchronizedTrafficLight::start() {
    if (isRunning.load()) return;
    
    isRunning = true;
    workerThread = std::make_unique<std::thread>([this]() {
        cycleColors();
    });
}

void SynchronizedTrafficLight::stop() {
    isRunning = false;
    if (workerThread && workerThread->joinable()) {
        workerThread->join();
    }
}