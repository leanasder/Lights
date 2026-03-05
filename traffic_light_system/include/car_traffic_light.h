//25.02.2026
//05.03.2026

#pragma once

#include "traffic_light_base.h"
#include "camera.h"
#include <memory>
#include <atomic>
#include <random>

enum class Direction {
    North, South, West, East
};

enum class DirectionGroup {
    NorthSouth, EastWest
};

class CarTrafficLight : public TrafficLightBase {
    std::atomic<TrafficColor> currentColor{TrafficColor::Red};
    std::unique_ptr<Camera> camera;
    std::atomic<int> myQueue{0};

    std::mt19937 rng;
    std::uniform_int_distribution<> arrivalDist{0, 3};

    Direction direction;
    DirectionGroup group;
    bool isLeader;
    int partnerId;           // ID светофора в той же группе
    int opponentLeaderId;    // ID лидера противоположной группы
    int threshold;           // порог для начала опроса
    
    // Для временного хранения данных опроса
    int partnerQueue;
    int opponentGroupTotal;
    bool waitingForPartner;
    bool waitingForOpponent;

protected:
    void processEvent(const Event& event) override {
        // TODO: реализовать логику событий
    }

public:
    CarTrafficLight(int id, Direction dir, CarTrafficLight* oppositeLight, 
                    bool leader, int partner, int opponent, int thresh);
    ~CarTrafficLight();

    void start() override;
    void stop() override;

    void setColor(TrafficColor color);
    TrafficColor getCurrentColor() const { return currentColor.load(); }

    //methods for work with the queue
    int getQueueLength() const { return myQueue.load();}
    void vehiclePassed(); 
    

    //simulation
    void simulateArrival(); 

    Direction getDirection() const { return direction; }
    DirectionGroup getGroup() const { return group; }
    bool isLeaderLight() const { return isLeader; }

    void processEvents() override; 
    void handleEvent() override;
};