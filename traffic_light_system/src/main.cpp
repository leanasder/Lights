//27.02.2026 - temporary test code
//02.03.2026
// 06.03.2026
// 09.03.2026
//10.03.2026

#include "car_traffic_light.h"
#include "pedestrian_traffic_light.h"
#include "colored_output.h"
#include "traffic_controller.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

int main() {
    using namespace std::chrono_literals;

    std::cout << "\033[1;35m"
              << "=========================================" << std::endl
              << "    TRAFFIC LIGHTS WITH PEDESTRIANS     " << std::endl
              << "========================================="
              <<"\033[0m" << std::endl;

    // Create controller (not started yet)
    auto controller = std::make_unique<TrafficController>();

    // Create 4 car traffic lights
    auto ns = std::make_unique<CarTrafficLight>(0, Direction::North, nullptr, 
                                                true, 2, 1, 25);
    auto sn = std::make_unique<CarTrafficLight>(2, Direction::South, ns.get(), 
                                                false, 0, 1, 25);
    auto we = std::make_unique<CarTrafficLight>(1, Direction::West, nullptr, 
                                                true, 3, 0, 20);
    auto ew = std::make_unique<CarTrafficLight>(3, Direction::East, we.get(), 
                                                false, 1, 0, 20);

    // Set opposite lights for cameras
    ns->setOppositeLight(sn.get());
    we->setOppositeLight(ew.get());

    // 🆕 CREATE 8 PEDESTRIAN TRAFFIC LIGHTS
    // North crossings (attached to NS)
    auto pedN1 = std::make_unique<PedestrianTrafficLight>(4, ns.get());   // north crossing 1
    auto pedN2 = std::make_unique<PedestrianTrafficLight>(5, ns.get());   // north crossing 2
    
    // South crossings (attached to SN)
    auto pedS1 = std::make_unique<PedestrianTrafficLight>(6, sn.get());   // south crossing 1
    auto pedS2 = std::make_unique<PedestrianTrafficLight>(7, sn.get());   // south crossing 2
    
    // West crossings (attached to WE)
    auto pedW1 = std::make_unique<PedestrianTrafficLight>(8, we.get());   // west crossing 1
    auto pedW2 = std::make_unique<PedestrianTrafficLight>(9, we.get());   // west crossing 2
    
    // East crossings (attached to EW)
    auto pedE1 = std::make_unique<PedestrianTrafficLight>(10, ew.get());  // east crossing 1
    auto pedE2 = std::make_unique<PedestrianTrafficLight>(11, ew.get());  // east crossing 2

    // Register all car lights with controller
    controller->registerLight(ns.get());
    controller->registerLight(sn.get());
    controller->registerLight(we.get());
    controller->registerLight(ew.get());

    // 🆕 Register ALL pedestrian lights with controller
    controller->registerLight(pedN1.get());
    controller->registerLight(pedN2.get());
    controller->registerLight(pedS1.get());
    controller->registerLight(pedS2.get());
    controller->registerLight(pedW1.get());
    controller->registerLight(pedW2.get());
    controller->registerLight(pedE1.get());
    controller->registerLight(pedE2.get());

    // Start controller (now all lights are registered)
    controller->start();
    std::this_thread::sleep_for(100ms); // give time to send initial commands

    // Start all car lights
    ns->start();
    sn->start();
    we->start();
    ew->start();

    // 🆕 Start all pedestrian lights
    pedN1->start();
    pedN2->start();
    pedS1->start();
    pedS2->start();
    pedW1->start();
    pedW2->start();
    pedE1->start();
    pedE2->start();

    ColoredOutput::printInfo("Simulation running for 60 seconds...");
    ColoredOutput::printInfo("Cars arrive every 2 seconds, pedestrians every 2 seconds");

    // Simulation loop
    int arrivalCounter = 0;
    for (int i = 0; i < 60; ++i) {
        arrivalCounter++;
        
        // Cars and pedestrians arrive every 2 seconds
        if (arrivalCounter % 2 == 0) {
            // Cars arrive
            ns->simulateArrival();
            sn->simulateArrival();
            we->simulateArrival();
            ew->simulateArrival();
            
            // 🆕 Pedestrians arrive
            pedN1->simulateArrival();
            pedN2->simulateArrival();
            pedS1->simulateArrival();
            pedS2->simulateArrival();
            pedW1->simulateArrival();
            pedW2->simulateArrival();
            pedE1->simulateArrival();
            pedE2->simulateArrival();
        }

        // Display queue lengths every second
        std::cout << "\n[" << i << "s] Queues:\n"
                  << "  Cars: NS:" << ns->getQueueLength() 
                  << " SN:" << sn->getQueueLength()
                  << " WE:" << we->getQueueLength()
                  << " EW:" << ew->getQueueLength()
                  << "\n  Pedestrians:"
                  << " N1:" << pedN1->getQueueLength()
                  << " N2:" << pedN2->getQueueLength()
                  << " S1:" << pedS1->getQueueLength()
                  << " S2:" << pedS2->getQueueLength()
                  << " W1:" << pedW1->getQueueLength()
                  << " W2:" << pedW2->getQueueLength()
                  << " E1:" << pedE1->getQueueLength()
                  << " E2:" << pedE2->getQueueLength()
                  << std::endl;

        // Display colors every 10 seconds
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
                      << " Peds:"
                      << " N:" << (pedN1->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << (pedN2->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << " S:" << (pedS1->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << (pedS2->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << " W:" << (pedW1->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << (pedW2->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << " E:" << (pedE1->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << (pedE2->getCurrentColor() == TrafficColor::Green ? "G" : "R")
                      << std::endl;
        }

        std::this_thread::sleep_for(1s);
    }

    ColoredOutput::printInfo("Simulation finished");

    std::cout << "\033[1;32m"
              << "=========================================\n"
              << "      SIMULATION COMPLETED SUCCESSFULLY  \n"
              << "=========================================\n"
              << "\033[0m" << std::endl;

    return 0;
}