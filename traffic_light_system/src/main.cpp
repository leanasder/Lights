//27.02.2026 - temporary test code
//02.03.2026
// 06.03.2026

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
    controller->start();

    // using smart pointers - automatic deletion
    auto ns = std::make_unique<CarTrafficLight>(0, Direction::North, nullptr, 
                                                true, 2, 1, 15);
    auto sn = std::make_unique<CarTrafficLight>(2, Direction::South, ns.get(), 
                                                false, 0, 1, 15);
    auto we = std::make_unique<CarTrafficLight>(1, Direction::West, nullptr, 
                                                true, 3, 0, 10);
    auto ew = std::make_unique<CarTrafficLight>(3, Direction::East, we.get(), 
                                                false, 1, 0, 10);

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
    
    // activating all traffic lights
    ns->start();
    sn->start();
    we->start();
    ew->start();
    pedNorth->start();
    pedSouth->start();

    ColoredOutput::printInfo("Simulation running for 60 seconds...");

    // simulating the arrival of cars every second 
    for (int i = 0; i < 60; ++i) {
        ns->simulateArrival();
        sn->simulateArrival();
        we->simulateArrival();
        ew->simulateArrival();
        
        // pedestrians also arrive
        pedNorth->simulateArrival();
        pedSouth->simulateArrival();

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