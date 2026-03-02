//27.02.2026
//02.03.2026

#include "pedestrian_traffic_light.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

PedestrianTrafficLight::PedestrianTrafficLight(int id)
    : TrafficLightBase(id),
    camera(std::make_unique<Camera>(id)) {
    ColoredOutput::print(id, TrafficColor::Red, "Pedestrian light created with camera");
}

PedestrianTrafficLight::~PedestrianTrafficLight() {
    stop();
}

void PedestrianTrafficLight::start() {
    TrafficLightBase::start();
    ColoredOutput::print(id, getCurrentColor(), "Pedestrian light started");
}

void PedestrianTrafficLight::stop() {
    TrafficLightBase::stop();
    ColoredOutput::print(id, getCurrentColor(), "Pedestrian light stopped");
}

void PedestrianTrafficLight::setGreen(bool green) {
    isGreen = green;
    ColoredOutput::print(id, getCurrentColor(), green ? "GREEN" : "RED");

    //if green is on - pedestrians goes
    if (green && camera) {
        std::thread([this]() {
            for (int i = 0; i < 3 && isRunning.load(); ++i) {
                std::this_thread::sleep_for(1s);
                if (isRunning.load()) {
                    pedestrianPassed(); // one pedestian goes in one second
                }
            }
        }).detach();
    }
}

void PedestrianTrafficLight::processEvents() {
    // processing of events. Will be later
}

void PedestrianTrafficLight::handleEvent() {
    // processing of events. Will be later
}
