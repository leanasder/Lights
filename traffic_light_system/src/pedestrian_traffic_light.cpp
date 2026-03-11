//27.02.2026
//02.03.2026
// 05.03.2026

#include "pedestrian_traffic_light.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

PedestrianTrafficLight::PedestrianTrafficLight(int id, TrafficLightBase* oppositeLight)
    : TrafficLightBase(id),
    camera(std::make_unique<Camera>(id, oppositeLight)) {
    ColoredOutput::print(id, TrafficColor::Red, "Pedestrian light created with camera");
}

PedestrianTrafficLight::~PedestrianTrafficLight() {
    stop();
}

void PedestrianTrafficLight::start() {
    TrafficLightBase::start();
    ColoredOutput::print(id, getCurrentColor(), "Pedestrian light started");
}

void PedestrianTrafficLight::stop() {
    stopCrossing();
    TrafficLightBase::stop();
    ColoredOutput::print(id, getCurrentColor(), "Pedestrian light stopped");
}

void PedestrianTrafficLight::setGreen(bool green) {
    isGreen = green;
    ColoredOutput::print(id, getCurrentColor(), green ? "GREEN" : "RED");

    // If green is on - pedestrians keep walking during the whole phase
    if (green) {  // camera check removed - not needed for pedestrians
        std::thread([this]() {
            static std::mt19937 rng(std::random_device{}());
            static std::uniform_int_distribution<> rateDist(2, 4); // 2-4 pedestrians per second
            
            while (isRunning.load() && isGreen.load()) {  // ← весь зелёный период!
                std::this_thread::sleep_for(1s);
                
                int crossingNow = rateDist(rng); // random 2, 3 or 4 this second
                int actualCrossed = 0;
                
                for (int i = 0; i < crossingNow && myQueue > 0; i++) {
                    pedestrianPassed();  // use existing method
                    actualCrossed++;
                }
                
                if (actualCrossed > 0) {
                    ColoredOutput::print(id, getCurrentColor(), 
                        "🚶 " + std::to_string(actualCrossed) + " pedestrians crossed" +
                        (actualCrossed < crossingNow ? " (queue empty)" : "") +
                        ", queue=" + std::to_string(myQueue.load()));
                }
            }
            ColoredOutput::print(id, getCurrentColor(), "🚶 Pedestrian phase ended");
        }).detach();
    }
}

void PedestrianTrafficLight::processEvent(const Event& event) {
     if (event.type == EventType::SwitchCommand) {
            setGreen(event.color == TrafficColor::Green);
        }
}


    void PedestrianTrafficLight::simulateArrival() {
        // Add random number of pedestrians (0-3) to the queue
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution<> dist(0, 3);
        
        int newPedestrians = dist(rng);
        if (newPedestrians > 0) {
            myQueue += newPedestrians;
            
            ColoredOutput::print(id, getCurrentColor(), 
                "🚶 " + std::to_string(newPedestrians) + " pedestrians arrived, queue=" + 
                std::to_string(myQueue.load()));
            
            // 🆕 SEND UPDATE TO CONTROLLER
            Event update(id, 1000, EventType::PedestrianQueueUpdate, myQueue.load());
            TrafficLightBase::sendEvent(1000, update);
        }
    }

    void PedestrianTrafficLight::pedestrianPassed() {
        if (myQueue > 0) {
            myQueue--;
            
            // 🆕 SEND UPDATE TO CONTROLLER
            Event update(id, 1000, EventType::PedestrianQueueUpdate, myQueue.load());
            TrafficLightBase::sendEvent(1000, update);
        }
    }

void PedestrianTrafficLight::startCrossing() {
    if (crossingActive.load()) return;

    crossingActive = true;

    crossingThread = std::thread([this]() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution<> rateDist(2, 4); // 2-4 пешехода в секунду
        
        while (crossingActive.load() && isRunning.load() && getCurrentColor() == TrafficColor::Green) {
            std::this_thread::sleep_for(1s);
            
            int crossingNow = rateDist(rng);
            int actualCrossed = 0;

            for (int i = 0; i < crossingNow && myQueue > 0; i++) {
                pedestrianPassed();  
                actualCrossed++;
            }

            if (actualCrossed > 0) {
                if (actualCrossed > 0) {
                ColoredOutput::print(id, getCurrentColor(), 
                    "🚶 " + std::to_string(actualCrossed) + " pedestrians crossed this second" +
                    (actualCrossed < crossingNow ? " (queue empty)" : "") +
                    ", queue now " + std::to_string(myQueue.load()));
                } else if (myQueue == 0) {
                    // Optional: print when queue becomes empty
                    ColoredOutput::print(id, getCurrentColor(), "🚶 Queue empty");
                }
            }
        }
        ColoredOutput::print(id, getCurrentColor(), "🚶 Crossing thread ended");
    });
}

void PedestrianTrafficLight::stopCrossing() {
    crossingActive = false;
    if (crossingThread.joinable()) {
        crossingThread.join();
    }
}

void PedestrianTrafficLight::processEvents() {
    // processing of events. Will be later
}

void PedestrianTrafficLight::handleEvent() {
    // processing of events. Will be later
}
