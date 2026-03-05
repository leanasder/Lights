//27.02.2026 - temporary test code
//02.03.2026

#include "car_traffic_light.h"
#include "pedestrian_traffic_light.h"
#include "colored_output.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    using namespace std::chrono_literals;

    std::cout << "\033[1;35m"
              << "=========================================" << std::endl
              << "    SYNCHRONIZED TRAFFIC LIGHTS SYSTEM   " << std::endl
              << "========================================="
              <<"\033[0m" << std::endl;

    CarTrafficLight* ns = new CarTrafficLight(0, Direction::North, nullptr, 
                                             true, 2, 1, 15);
    CarTrafficLight* sn = new CarTrafficLight(2, Direction::South, ns, 
                                             false, 0, 1, 15);
    CarTrafficLight* we = new CarTrafficLight(1, Direction::West, nullptr, 
                                             true, 3, 0, 10);
    CarTrafficLight* ew = new CarTrafficLight(3, Direction::East, we, 
                                             false, 1, 0, 10);

    PedestrianTrafficLight* pedNorth = new PedestrianTrafficLight(4, ns); // смотрит на NS
    PedestrianTrafficLight* pedSouth = new PedestrianTrafficLight(5, sn); // смотрит на SN

    ns->start();
    sn->start();
    we->start();
    ew->start();
    pedNorth->start();
    pedSouth->start();

    std::cout << "\033[1;32m"
              << "=========================================" << std::endl
              << "      SYSTEM STOPPED SUCCESSFULLY        " << std::endl
              << "========================================="
              << "\033[0m" << std::endl;

    return 0;
}