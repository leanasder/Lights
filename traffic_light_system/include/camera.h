//27.02.2026
#pragma once

#include <atomic>
#include <random>

class Camera {
    int lightId;
    std::atomic<int> queueLength{0};
    
    //for simulation random ariiving cars or pedestrians
    std::mt19937 rng;
    std::uniform_int_distribution<> arrivalDist{0, 3};  // 0-3 new in second 
    
public:
    Camera(int id);
    
    //counting current queue
    int countQueue();
    
    //simulated passage/transition (queue reduction)
    void vehiclePassed();     // for cars
    void pedestrianPassed();  // for pedestrians
    
    // simulation arriving new (queue increase)
    void simulateArrival();
    
    int getQueueLength() const { return queueLength.load(); }
    
    //start the background simulator
    void startSimulation();
    void stopSimulation();
};