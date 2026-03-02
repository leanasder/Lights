//27.02.2026 - temporary test code
//02.03.2026

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
   PedestrianTrafficLight pedLight(5);
   pedLight.start();

   ColoredOutput::printInfo("Testing pedestrian light for 15 seconds..."); 
    
    for (int i = 0; i < 15; ++i) {
        pedLight.simulateArrival(); // random arrivals

        //show queue
        std::cout << " Queue: " << pedLight.getQueueLenght() << std::endl;

        if (i == 3) pedLight.setGreen(true); // green is on
        if (i == 8) pedLight.setGreen(false); // red is on

        std::this_thread::sleep_for(1s);
    }

    pedLight.stop();

    std::cout << "\033[1;32m"
              << "=========================================" << std::endl
              << "      SYSTEM STOPPED SUCCESSFULLY        " << std::endl
              << "========================================="
              << "\033[0m" << std::endl;

    return 0;
}