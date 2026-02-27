//27.02.2026 - temporary test code

#include "traffic_controller.h"
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
    
    // auto GREEN_RED_DUR = 10s;
    // auto YELLOW_DUR = 2s;
    // auto WORK_DUR = 60s;

    // TrafficController controller(GREEN_RED_DUR, YELLOW_DUR);

    // CarTrafficLight light0(0);
    // CarTrafficLight light1(1);

    // controller.registerLight(&light0);
    // controller.registerLight(&light1);

    // ColoredOutput::printInfo("System configuration:");
    // ColoredOutput::printInfo(" • Green duration: " + std::to_string(GREEN_RED_DUR.count())
    //                          + " seconds");
    // ColoredOutput::printInfo(" • Yellow duration: " + std::to_string(YELLOW_DUR.count())
    //                          + " seconds");
    // ColoredOutput::printInfo(" • Light 0: North-South direction");
    // ColoredOutput::printInfo(" • Light 1: East-West direction");
    // ColoredOutput::printInfo("Starting in 2 seconds...");

    // std::this_thread::sleep_for(2s);

    // light0.start();
    // light1.start();
    // std::this_thread::sleep_for(100ms);
    // controller.start();

    // std::this_thread::sleep_for(WORK_DUR);

    // ColoredOutput::printInfo("\nInitiating system shutdown...");
    // controller.stop();
    // light0.stop();
    // light1.stop();

    //creating test light with camera
    CarTrafficLight carLight(0);
    carLight.start();

    // Testing the camera
    std::cout << "\nTesting camera for 10 seconds..." << std::endl;

    for (int i = 0; i < 10; ++i) {
        carLight.simulateArrival(); // random arrivals
        if (i == 3) carLight.setColor(TrafficColor::Green); // Green is on
        if (i == 7) carLight.setColor(TrafficColor::Red); // Red is on
        
        std::this_thread::sleep_for(1s);
    }

    carLight.stop();

    std::cout << "\033[1;32m"
              << "=========================================" << std::endl
              << "      SYSTEM STOPPED SUCCESSFULLY        " << std::endl
              << "========================================="
              << "\033[0m" << std::endl;

    return 0;
}