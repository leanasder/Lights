// 27.02.2026
#include "camera.h"

Camera::Camera(int id) : lightId(id), rng(std::random_device{}()) {}

int Camera::countQueue() {
    return queueLength.load();
}

void Camera::vehiclePassed() {
    if (queueLength > 0) {
        queueLength--;
    }
}

void Camera::pedestrianPassed() {
    if (queueLength > 0) {
        queueLength--;
    }
}

void Camera::simulateArrival() {
    int newArrivals = arrivalDist(rng);
    if (newArrivals > 0) {
        queueLength += newArrivals;
    }
}

void Camera::startSimulation() {
    // TODO: start a thread to simulate arrivals
}

void Camera::stopSimulation() {
    // TODO: stop thread
}