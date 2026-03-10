//27.02.2026
// 02.03.2026
// 05.03.2026


#pragma once

#include "traffic_light_base.h"
#include "camera.h"
#include <memory>
#include <atomic>
#include <random>


class PedestrianTrafficLight : public TrafficLightBase {
    std::atomic<bool> isGreen{false}; // true == Green, false == Red
    std::unique_ptr<Camera> camera;
    std::atomic<int> myQueue{0};

    std::atomic<bool> crossingActive{false};
    std::thread crossingThread;

protected:
     void processEvent(const Event& event) override; 

public:
    PedestrianTrafficLight(int id, TrafficLightBase* oppositeLight);
       
    ~PedestrianTrafficLight(); 

     void stop() override;
     void start() override; 

     void setGreen(bool green); 

    bool isPedestrianGreen() const { return isGreen.load(); }

    //for compatibility with the common interface
    TrafficColor getCurrentColor() const {
        return isGreen ? TrafficColor::Green : TrafficColor::Red;
    }

    // methods for work with camera
    int getQueueLenght() const { return myQueue.load(); }

     void pedestrianPassed(); 

    void simulateArrival();

    int getQueueLength() const override {
        return myQueue.load(); 
    }

    void startCrossing();
    void stopCrossing();
 
    void processEvents() override;  
    void handleEvent() override;    
};