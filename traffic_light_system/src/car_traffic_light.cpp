//05.03.2026

#include "car_traffic_light.h"
#include <chrono>
#include <thread>
#include <random>

using namespace std::chrono_literals;

void CarTrafficLight::vehiclePassed() {
    if (myQueue > 0) {
        myQueue--;
        // later this will be added
    }
}

void CarTrafficLight::simulateArrival() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, 3);
    myQueue += dist(rng);
    // later this wiil be added
}

CarTrafficLight::CarTrafficLight(int id, Direction dir, CarTrafficLight * oppositeLight,
                                 bool leader, int partner, int opponent, int thresh)
    : TrafficLightBase(id),
    camera(std::make_unique<Camera>(id, oppositeLight)),
    rng(std::random_device{}()),
    direction(dir),
    group((dir == Direction::North || dir == Direction::South) ?
          DirectionGroup::NorthSouth : DirectionGroup::EastWest),
    isLeader(leader),
    partnerId(partner),
    opponentLeaderId(opponent),
    threshold(thresh),
    partnerQueue(-1),
    opponentGroupTotal(-1),
    waitingForPartner(false),
    waitingForOpponent(false) { 

    ColoredOutput::print(id, TrafficColor::Red, "Car light created with camera");
}

CarTrafficLight::~CarTrafficLight() {
    stop();
}

void CarTrafficLight::start() {
    TrafficLightBase::start(); // call method base class
    
    ColoredOutput::print(id, currentColor.load(), "Car light started");
    // here need to create the thread to manage events
}

void CarTrafficLight::stop() {
   
    TrafficLightBase::stop(); // call method base class
    ColoredOutput::print(id, currentColor.load(), "Car light stopped");
}

void CarTrafficLight::setColor(TrafficColor color) {
    currentColor = color;
    ColoredOutput::print(id, color, "Color changed");

    //if the light turns green, cars will move (reduce queue)
    if (color ==TrafficColor::Green ) {
        // for green's time rides few cars
        std::thread([this]() {
            for (int i = 0; i < 3 && isRunning.load(); ++i) {
                std::this_thread::sleep_for(1s);
                vehiclePassed();
            }
        }).detach();
    }
}

void CarTrafficLight::processEvents() {
    // implement later
}

void CarTrafficLight::handleEvent() {
    //implement later
}