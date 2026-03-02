//27.02.2026
// 02.03.2026

#pragma once

#include "traffic_light_base.h"
#include "camera.h"
#include <memory>


class PedestrianTrafficLight : public TrafficLightBase {
    std::atomic<bool> isGreen{false}; // true == Green, false == Red
    std::unique_ptr<Camera> camera;

protected:
    void processEvents() override;
    void handleEvent() override;

public:
    PedestrianTrafficLight(int id);
    ~PedestrianTrafficLight();

    void start() override;
    void stop() override;

    void setGreen(bool green);
    bool isPedestrianGreen() const { return isGreen.load(); }

    //for compatibility with the common interface
    TrafficColor getCurrentColor() const {
        return isGreen ? TrafficColor::Green : TrafficColor::Red;
    }

    // methods for work with camera
    int getQueueLenght() const { return camera ? camera->getQueueLength() : 0; }
    void pedestrianPassed() { if (camera) camera->pedestrianPassed(); }
    void simulateArrival() { if (camera) camera->simulateArrival(); }
};