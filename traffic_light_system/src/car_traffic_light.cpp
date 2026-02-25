#include "car_traffic_light.h"
//while make comment
// #include "traffic_controller.h"

CarTrafficLight::CarTrafficLight(int id) : TrafficLightBase(id) {
    ColoredOutput::print(id, TrafficColor::Red, "Car light created");
}

CarTrafficLight::~CarTrafficLight() {
    stop();
}

void CarTrafficLight::start() {
    TrafficLightBase::start(); // call method base class
    ColoredOutput::print(id, currentColor.load(), "Car light started");
    // here need to create the thread to manage events
}

void CarTrafficLight::stop() {
    TrafficLightBase::stop(); // call method base class
    ColoredOutput::print(id, currentColor.load(), "Car light stopped");
}

void CarTrafficLight::setColor(TrafficColor color) {
    currentColor = color;
    ColoredOutput::print(id, color, "Color changed");
}

void CarTrafficLight::processEvents() {
    // implement later
}

void CarTrafficLight::handleEvent() {
    //implement later
}