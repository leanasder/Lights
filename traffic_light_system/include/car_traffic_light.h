//25.02.2026

#pragma once

#include "traffic_light_base.h"
#include "camera.h"
#include <memory>

class CarTrafficLight : public TrafficLightBase {
    std::atomic<TrafficColor> currentColor{TrafficColor::Red};
    std::unique_ptr<Camera> camera;

protected:
    void processEvents() override;
    void handleEvent() override;

public:
    CarTrafficLight(int id);
    ~CarTrafficLight();

    void start() override;
    void stop() override;

    void setColor(TrafficColor color);
    TrafficColor getCurrentColor() const { return currentColor.load(); }

    //methods for work with the queue
    int getQueueLength() const { return camera ? camera->getQueueLength() : 0;}
    void vehiclePassed() { if (camera) camera->vehiclePassed(); }

    //simulation
    void simulateArrival() { if (camera) camera->simulateArrival(); }
};