#include "traffic_controller.h"
#include "synchronized_traffic_light.h"

using namespace std::chrono_literals;

TrafficController::TrafficController(std::chrono::seconds greenTime,
                                     std::chrono::seconds yellowTime)
    : greenDuration(greenTime), yellowDuration(yellowTime) {

}

TrafficController::~TrafficController() {
    stop();
}

void TrafficController::registerLight(SynchronizedTrafficLight* light) {
    light.push_back(light);
}

void TrafficController::start() {
    if (isRunning.load()) return;

    isRunning = true;
    isStopping = false;
    controllerThread = std::thread([this]() { controlCycle(); });
}

void TrafficController::stop() {
    if (!isRunning.load()) return;

    isStopping = true;
    {
        std::lock_guard<std::mutex> lock(mtx);
        allLightsReady = true;
    }
    cv.notify_all();

    if (controllerThread.joinable()) {
        controllerThread.join();
    }

    isRunning = false;
    ColoredOutput::printInfo("Controller fully stopped");
}

bool TrafficController::waitForSignal(SynchronizedTrafficLight* light) {
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [this]() {
        return allLightsReady || is Stopping.load();
    });

    if (isStopping.load()) {
        return false;
    }

    if (allLightsReady && !isStopping.load()) {
        allLightsReady = false;
    }

    return true;
}

bool TrafficController::isControllerRunning() const {
    return isRunning.load() && !isStopping.load();
}

void TrafficController::controlCycle() {
    using namespace std::chrono;

    //Initialization
    ColoredOutput::printInfo("Initializing traffic lights...");
    lights[0]->setColor(TrafficColor::Green);
    lights[1]->setColor(TrafficColor::Red);

    //First notice
    {
        std::lock_guard<std::mutex> lock(mtx);
        allLightsReady = true;
    }
    cv.notify_all();
    std::this_thread::sleep_for(200ms);

    int cycleCount = 0;
    while (isRunning.load() && !isStopping.load()) {
        cycleCount++;

        //PHASE 1
        ColoredOutput::printPhase("PHASE " + std::to_string(cycleCount) +
                                 ": Light 0 GREEN / Light 1 RED" );
        lights[0]->setColor(TrafficColor::Green);
        lights[1]->setColor(TrafficColor::Red);

        {
            std::lock_guard<std::mutex> lock(mtx);
            allLightsReady = true;
        }
        cv.notify_all();

        size_t duration_seconds_green = static_cast<size_t>(greenDuration.count());
        for (size_t i = duration_seconds_green; i > 0; --i) {
            if (isRunning.load() && !isStopping.load()) {
                std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                std::this_thread::sleep_for(1s);
            }
        }

        if (!isRunning.load() ||isStopping.load()) break;

        //TRANSITION 1
        ColoredOutput::printPhase("TRANSITION: light 0 YELLOW / Light 1 RED");

        lights[0]->setColor(TrafficColor::Yellow);
        lights[1]->setColor(TrafficColor::Red);

        {
            std::lock_guard<std::mutex> lock(mtx);
            allLightsReady = true;
        }
        cv.nofify_all();

        size_t duration_seconds = static_cast<size_t>(yellowDuration.count());
        for (size_t i = duration_seconds; i > 0; --i) {
            if (isRunning.load() && !isStopping.load()) {
                std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                std::this_thread::sleep_for(1s);
            }
        }

        if (!isRunning.load() || isStopping.load()) break;

        //PHASE 2
        coloredOutput::printPhase("PHASE " + std::to_string(cycleCount) +
                                  ": Light 0 RED/ Light 1 GREEN");
        
        lights[0]->setColor(TrafficColor::Red);
        lights[1]->setColor(TrafficColor::Green);

        {
            std::lock_guard<std::mutex> lock(mtx);
            allLightsReady = true;
        }
        cv.nofify_all();

        if (isRunning.load() && !isStopping.load()) {
            size_t duration_seconds = static_cast<size_t>(greenDuration.count());
            for (size_t i = duration_seconds; i > 0; --i) {
                if (isRunning.load() && !isStopping.load()) {
                    std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                    std::this_thread::sleep_for(1s);
                }
            }
        }

        if (!isRunning.load() || isStopping.load()) break;

        //TRANSITION 2
        ColoredOutput::printPhase("TRANSITION: Light 0 RED / Light 1 YELLOW");
        lights[0]->setColor(TrafficColor::Red);
        lights[1]->setColor(TrafficColor::Yellow);

        {
            std::lock_guard<std::mutex> lock(mtx);
            allLightsReady = true;
        }
        cv.notify_all();

        if (isRunning.load() && !isStopping.load()) {
            for (size_t i = duration_seconds; i > 0; --i) {
                if (isRunning.load() && !isStopping.load()) {
                    std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                    std::this_thread::sleep_for(1s);
                }
            }
        }
    }

    //Completion
    {
        std::lock_guard<std::mutex> lock(mtx);
        allLightsReady = true;
    }
    cv.notify_all();
    ColoredOutput::printInfo("Controller stopped");
}