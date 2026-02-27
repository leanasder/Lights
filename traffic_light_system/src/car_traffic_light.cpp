#include "car_traffic_light.h"
//while make comment
// #include "traffic_controller.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

CarTrafficLight::CarTrafficLight(int id)
    : TrafficLightBase(id),
    camera(std::make_unique<Camera>(id)) { //create the camera
    ColoredOutput::print(id, TrafficColor::Red, "Car light created with camera");
}

CarTrafficLight::~CarTrafficLight() {
    stop();
}

void CarTrafficLight::start() {
    TrafficLightBase::start(); // call method base class
    if (camera) {
        camera->startSimulation();
    }
    ColoredOutput::print(id, currentColor.load(), "Car light started");
    // here need to create the thread to manage events
}

void CarTrafficLight::stop() {
    if (camera) {
        camera->stopSimulation();
    }
    TrafficLightBase::stop(); // call method base class
    ColoredOutput::print(id, currentColor.load(), "Car light stopped");
}

void CarTrafficLight::setColor(TrafficColor color) {
    currentColor = color;
    ColoredOutput::print(id, color, "Color changed");

    //if the light turns green, cars will move (reduce queue)
    if (color ==TrafficColor::Green && camera) {
        // for green's time rides few cars
        std::thread([this]() {
            for (int i = 0; i < 3 && isRunning.load(); ++i) {
                std::this_thread::sleep_for(1s);
                vehiclePassed();
            }
        }).detach();
    }
}

void CarTrafficLight::processEvents() {
    // implement later
}

void CarTrafficLight::handleEvent() {
    //implement later
}