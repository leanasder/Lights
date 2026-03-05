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

     void pedestrianPassed() {
        if (myQueue > 0) {
            myQueue--;
        }
    }

    void simulateArrival() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution<> dist(0, 3);
        myQueue += dist(rng);
    }

    int getQueueLength() const override {
        return 0; // пока заглушка
    }

    void processEvents() override;  
    void handleEvent() override;    
};