// 27.02.2026
#include "camera.h"
#include <thread>
#include <chrono>
#include <iostream>  // for debugging, i'll remove it later

Camera::Camera(int id) : lightId(id), rng(std::random_device{}()) {}

int Camera::countQueue() {
    return queueLength.load();
}

void Camera::vehiclePassed() {
    if (queueLength > 0) {
        queueLength--;
        std::cout << "Camera " << lightId << ": vehicle passed, queue=" 
                  << queueLength.load() << std::endl;  // debugging
    }
}

void Camera::pedestrianPassed() {
    if (queueLength > 0) {
        queueLength--;
        std::cout << "Camera " << lightId << ": pedestrian passed, queue=" 
                  << queueLength.load() << std::endl;  // debugging
    }
}

void Camera::simulateArrival() {
    int newArrivals = arrivalDist(rng);
    if (newArrivals > 0) {
        queueLength += newArrivals;
        std::cout << "Camera " << lightId << ": " << newArrivals 
                  << " arrived, queue=" << queueLength.load() << std::endl;  // debugging
    }
}

void Camera::startSimulation() {
    // TODO: start a thread to simulate arrivals
    std::cout << "Camera " << lightId << ": simulation started" << std::endl;
}

void Camera::stopSimulation() {
    // TODO: stop thread
    std::cout << "Camera " << lightId << ": simulation stopped" << std::endl;
}