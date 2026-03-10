//27.02.2026 - temporary test code
//02.03.2026
// 06.03.2026
// 09.03.2026

#include "car_traffic_light.h"
#include "pedestrian_traffic_light.h"
#include "colored_output.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include "traffic_controller.h"

int main() {
    using namespace std::chrono_literals;

    std::cout << "\033[1;35m"
              << "=========================================" << std::endl
              << "    SYNCHRONIZED TRAFFIC LIGHTS SYSTEM   " << std::endl
              << "========================================="
              <<"\033[0m" << std::endl;

    // ✅ СОЗДАЁМ КОНТРОЛЛЕР
    auto controller = std::make_unique<TrafficController>();

    // using smart pointers - automatic deletion
    auto ns = std::make_unique<CarTrafficLight>(0, Direction::North, nullptr, 
                                                true, 2, 1, 25);
    auto sn = std::make_unique<CarTrafficLight>(2, Direction::South, ns.get(), 
                                                false, 0, 1, 25);
    auto we = std::make_unique<CarTrafficLight>(1, Direction::West, nullptr, 
                                                true, 3, 0, 20);
    auto ew = std::make_unique<CarTrafficLight>(3, Direction::East, we.get(), 
                                                false, 1, 0, 20);

    // installation of opposite traffic lights
    ns->setOppositeLight(sn.get());
    we->setOppositeLight(ew.get());


    auto pedNorth = std::make_unique<PedestrianTrafficLight>(4, ns.get());
    auto pedSouth = std::make_unique<PedestrianTrafficLight>(5, sn.get());

    controller->registerLight(ns.get());
    controller->registerLight(ns.get());
    controller->registerLight(sn.get());
    controller->registerLight(we.get());
    controller->registerLight(ew.get());
    controller->registerLight(pedNorth.get());
    controller->registerLight(pedSouth.get());
    
    controller->start();

    std::this_thread::sleep_for(100ms);

    // activating all traffic lights
    ns->start();
    sn->start();
    we->start();
    ew->start();
    pedNorth->start();
    pedSouth->start();

    ColoredOutput::printInfo("Simulation running for 60 seconds...");
    ColoredOutput::printInfo("Cars arrive every 2 seconds (more realistic)");

    // simulating the arrival of cars every 2 seconds 
    int arrivalCounter = 0;
    for (int i = 0; i < 60; ++i) {
        arrivalCounter++;

        // we add new cars and pedestrians every 2 seconds.
        if (arrivalCounter % 2 == 0) { 
        ns->simulateArrival();
        sn->simulateArrival();
        we->simulateArrival();
        ew->simulateArrival();
        
        // pedestrians also arrive
        pedNorth->simulateArrival();
        pedSouth->simulateArrival();
        }
        // displaying the current status of queues
        std::cout << "\n[" << i << "s] Queues: "
                  << " NS:" << ns->getQueueLength()
                  << " SN:" << sn->getQueueLength()
                  << " WE:" << we->getQueueLength()
                  << " EW:" << ew->getQueueLength()
                  << " PedN:" << pedNorth->getQueueLength()
                  << " PedS:" << pedSouth->getQueueLength()
                  << std::endl;

        // showing traffic light colors every 10 seconds
        if (i % 10 == 0) {
            std::cout << "Colors: "
                      << " NS:" << (ns->getCurrentColor() == TrafficColor::Green ? "G" :
                                   ns->getCurrentColor() == TrafficColor::Yellow ? "Y" : "R")
                      << " SN:" << (sn->getCurrentColor() == TrafficColor::Green ? "G" :
                                   sn->getCurrentColor() == TrafficColor::Yellow ? "Y" : "R")
                      << " WE:" << (we->getCurrentColor() == TrafficColor::Green ? "G" :
                                   we->getCurrentColor() == TrafficColor::Yellow ? "Y" : "R")
                      << " EW:" << (ew->getCurrentColor() == TrafficColor::Green ? "G" :
                                   ew->getCurrentColor() == TrafficColor::Yellow ? "Y" : "R")
                      << std::endl;
        }

        std::this_thread::sleep_for(1s);
    }

    // automatically stopping when destroyed unique_ptr
    ColoredOutput::printInfo("Simulation finished");

    std::cout << "\033[1;32m"
              << "=========================================\n"
              << "      SIMULATION COMPLETED SUCCESSFULLY  \n"
              << "=========================================\n"
              << "\033[0m" << std::endl;

    return 0;
}