#include "traffic_controller.h"
#include "synchronized_traffic_light.h"
#include <thread>
#include <iostream>

int main() {
    using namespace std::chrono_literals;

    std::cout << "\033[1;35m"
              << "=========================================" << std::endl
              << "    SYNCHRONIZED TRAFFIC LIGHTS SYSTEM   " << std::endl
              << "========================================="
              <<"\033[0m" << std::endl;
    
    auto GREEN_RED_DUR = 10s;
    auto YELLOW_DUR = 2s;
    auto WORK_DUR = 60s;

    TrafficController controller(GREEN_RED_DUR, YELLOW_DUR);

    SynchronizedTrafficLight light0(0, &controller);
    SynchronizedTrafficLight light1(1, &controller);

    controller.registerLight(&light0);
    controller.registerLight(&light1);

    ColoredOutput::printInfo("System configuration:");
    ColoredOutput::printInfo(" • Green duration: " + std::to_string(GREEN_RED_DUR.count())
                             + " seconds");
    ColoredOutput::printInfo(" • Yellow duration: " + std::to_string(YELLOW_DUR.count())
                             + " seconds");
    ColoredOutput::printInfo(" • Light 0: North-South direction");
    ColoredOutput::printInfo(" • Light 1: East-West direction");
    ColoredOutput::printInfo("Starting in 2 seconds...");

    std::this_thread::sleep_for(2s);

    light0.start();
    light1.start();
    std::this_thread::sleep_for(100ms);
    controller.start();

    std::this_thread::sleep_for(WORK_DUR);

    ColoredOutput::printInfo("\nInitiating system shutdown...");
    controller.stop();
    light0.stop();
    light1.stop();

    std::cout << "\033[1;32m"
              << "=========================================" << std::endl
              << "      SYSTEM STOPPED SUCCESSFULLY        " << std::endl
              << "========================================="
              << "\033[0m" << std::endl;

    return 0;
}