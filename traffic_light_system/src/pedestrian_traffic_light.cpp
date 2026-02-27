//27.02.2026

#include "pedestrian_traffic_light.h"

PedestrianTrafficLight::PedestrianTrafficLight(int id)
    : TrafficLightBase(id) {
    ColoredOutput::print(id, TrafficColor::Red, "Pedestrian light created");
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
}

void PedestrianTrafficLight::processEvents() {
    // processing of events. Will be later
}

void PedestrianTrafficLight::handleEvent() {
    // processing of events. Will be later
}
