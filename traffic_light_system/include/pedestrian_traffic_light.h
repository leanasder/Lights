//27.02.2026

#pragma once

#include "traffic_light_base.h"

class PedestrianTrafficLight : public TrafficLightBase {
    std::atomic<bool> isGreen{false}; // true == Green, false == Red

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
};