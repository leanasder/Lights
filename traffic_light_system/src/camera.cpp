// 27.02.2026
#include "camera.h"
#include <thread>
#include <chrono>

Camera::Camera(int id) : lightId(id), rng(std::random_device{}()) {}

int Camera::countQueue() {
    return queueLength.load();
}

void Camera::vehiclePassed() {
    if (queueLength > 0) {
        queueLength--;
        ColoredOutput::print(lightId, TrafficColor::Green,
            "Vehicle passed, queue=" + std::to_string(queueLength.load()));
    }
}

void Camera::pedestrianPassed() {
    if (queueLength > 0) {
        queueLength--;
        ColoredOutput::print(lightId, TrafficColor::Yellow,
            "Pedestrian passed, queue=" + std::to_string(queueLength.load()));
    }
}

void Camera::simulateArrival() {
    int newArrivals = arrivalDist(rng);
    if (newArrivals > 0) {
        queueLength += newArrivals;
        ColoredOutput::print(lightId, TrafficColor::Red,
        std::to_string(newArrivals) + " arrived, queue=" +
        std::to_string(queueLength.load()));
    }
}

void Camera::startSimulation() {
    // TODO: start a thread to simulate arrivals
    ColoredOutput::printInfo("Camera " + std::to_string(lightId) +
                             ": simulation started");
}

void Camera::stopSimulation() {
    // TODO: stop thread
    ColoredOutput::printInfo("Camera " + std::to_string(lightId) +
                             ": simulation stopped");
}