//25.02.2026

#pragma once

#include "traffic_light_base.h"

class CarTrafficLight : public TrafficLightBase {
    std::atomic<TrafficColor> currentColor{TrafficColor::Red};

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
};