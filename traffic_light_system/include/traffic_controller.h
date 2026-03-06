// 06.03.2026

#ifndef TRAFFIC_CONTROLLER_H
#define TRAFFIC_CONTROLLER_H

#include "traffic_light_base.h"
#include "event.h"
#include <vector>

class TrafficController : public TrafficLightBase {
    std::vector<TrafficLightBase*> lights;
    
protected:
    void processEvent(const Event& event) override;

public:
    static constexpr int CONTROLLER_ID = 1000;

    TrafficController();
    ~TrafficController();
    void start();
    void stop();


    void registerLight(TrafficLightBase* light);

    void processEvents() override;
    void handleEvent() override;
    TrafficColor getCurrentColor() const override ;
    int getQueueLength() const override ;
};

#endif