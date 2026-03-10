//05.03.2026
// 06.03.2026

#include "car_traffic_light.h"
#include <chrono>
#include <thread>
#include <random>
#include "event.h"

using namespace std::chrono_literals;

void CarTrafficLight::checkThreshold() {
    if (!isLeader) return;  // только лидеры начинают опрос

    // checking timeout waiting
    auto now = std::chrono::steady_clock::now();
    if (waitingForPartner || waitingForOpponent) {
        if (now - surveyStartTime > SURVEY_TIMEOUT) {
            ColoredOutput::print(id, currentColor,
                "⚠️ Survey timeout! Resetting waiting flags");
            waitingForOpponent = false;
            waitingForPartner = false;   
        }
    }

     ColoredOutput::print(id, currentColor, 
        "checkThreshold: queue=" + std::to_string(myQueue.load()) + 
        ", threshold=" + std::to_string(threshold) +
        ", waiting=" + std::to_string(waitingForPartner));

    if (myQueue >= threshold && !waitingForPartner && !waitingForOpponent) {
        waitingForPartner = true;
        partnerQueue = -1;
        
        Event query(id, partnerId, EventType::QueryPartnerQueue, 0);
        TrafficLightBase::sendEvent(partnerId, query);
        ColoredOutput::print(id, currentColor, "Starting group survey, queue=" + std::to_string(myQueue.load()));
    }
}

void CarTrafficLight::vehiclePassed() {
    if (myQueue > 0) {
        myQueue--;
        checkThreshold();
    }
}

void CarTrafficLight::simulateArrival() {
    static std::mt19937 rng(std::random_device{}());
    
    // how many cars arrived dureing this tick
    static std::uniform_int_distribution<> countDist(0, 3);
    // direction for each car (0- straight, 1 - right)
    static std::uniform_int_distribution<> turnDist(0, 1);
    
    int carsCount = countDist(rng);
    int straightInBatch = 0;
    int rightInBatch = 0;
    
    // each car independently chooses the direction
    for (int i = 0; i < carsCount; ++i) {
        int turn = turnDist(rng);
        myQueue++;  // encreasing the total queue
        
        if (turn == 0) {
            straightInBatch++;
            // straightCount++; 
        } else {
            rightInBatch++;
            // rightCount++;
        }
    }
    
    // beautiful output with information about the directions
    std::string directionInfo;
    if (carsCount > 0) {
        directionInfo = " (straight:" + std::to_string(straightInBatch) + 
                       ", right:" + std::to_string(rightInBatch) + ")";
    }
    
    ColoredOutput::print(id, currentColor, 
        "🚗 " + std::to_string(carsCount) + " cars arrived" + directionInfo + 
        ", queue=" + std::to_string(myQueue.load()));
    
    checkThreshold();
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
    waitingForOpponent(false),
    surveyStartTime(std::chrono::steady_clock::now()) {   

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

void CarTrafficLight::processEvent(const Event& event) {
    switch (event.type) {
        case EventType::QueryPartnerQueue: {
            // Меня спросили, сколько у меня машин
            ColoredOutput::print(id, currentColor, 
                "Received QueryPartnerQueue from " + std::to_string(event.senderId));
            
            Event response(id, event.senderId, EventType::PartnerQueueResponse, myQueue.load());
            TrafficLightBase::sendEvent(event.senderId, response);
            break;
        }
        
        case EventType::PartnerQueueResponse: {
                ColoredOutput::print(id, currentColor, 
                    "📩 Received PartnerQueueResponse: queue=" + std::to_string(event.queueLength) +
                    " from " + std::to_string(event.senderId));
                
                if (isLeader && waitingForPartner) {
                    partnerQueue = event.queueLength;
                    waitingForPartner = false;
                    ColoredOutput::print(id, currentColor, 
                        "✅ Partner queue updated to " + std::to_string(partnerQueue));
                    
                    // sending query to opponent
                    waitingForOpponent = true;
                    surveyStartTime = std::chrono::steady_clock::now(); 

                // ✅ ТЕПЕРЬ ОПРАШИВАЕМ ПРОТИВОПОЛОЖНУЮ ГРУППУ!
                Event query(id, opponentLeaderId, EventType::QueryOpponentGroup, 0);
                TrafficLightBase::sendEvent(opponentLeaderId, query);
                ColoredOutput::print(id, currentColor, 
                    "➡️ Sending QueryOpponentGroup to " + std::to_string(opponentLeaderId));
                    
                }
                break;
        }
        
        case EventType::QueryOpponentGroup: {
            ColoredOutput::print(id, currentColor, 
                "🔥🔥🔥 Received QueryOpponentGroup from " + std::to_string(event.senderId) +
                ", isLeader=" + std::to_string(isLeader) +
                ", partnerQueue=" + std::to_string(partnerQueue));
            
            if (isLeader) {
                if (partnerQueue == -1) {
                    ColoredOutput::print(id, currentColor, 
                        "⏳ partnerQueue unknown, requesting from partner " + std::to_string(partnerId));
                    waitingForPartner = true;
                    surveyStartTime = std::chrono::steady_clock::now();
                    Event query(id, partnerId, EventType::QueryPartnerQueue, 0);
                    TrafficLightBase::sendEvent(partnerId, query);
                } else {
                    int total = myQueue.load() + partnerQueue;
                    ColoredOutput::print(id, currentColor, 
                        "📤 Sending opponent response: total=" + std::to_string(total) +
                        " (myQueue=" + std::to_string(myQueue.load()) + 
                        " + partnerQueue=" + std::to_string(partnerQueue) + ")");
                    Event response(id, event.senderId, EventType::OpponentGroupResponse, total);
                    TrafficLightBase::sendEvent(event.senderId, response);
                }
            } else {
                ColoredOutput::print(id, currentColor, 
                    "❌ Ignoring QueryOpponentGroup - not a leader");
            }
            break;
        }
        case EventType::OpponentGroupResponse: {
            // Получили сумму противоположной группы
            if (isLeader && waitingForOpponent) {
                opponentGroupTotal = event.queueLength;
                waitingForOpponent = false;
                
                int myTotal = myQueue.load() + partnerQueue;
                ColoredOutput::print(id, currentColor, 
                    "My total=" + std::to_string(myTotal) + 
                    ", opponent total=" + std::to_string(opponentGroupTotal));
                
                if (myTotal > opponentGroupTotal) {
                    // Наша группа загружена больше – просим контроллер
                    Event req(id, 1000, EventType::RequestSwitch, myTotal, opponentGroupTotal);
                    TrafficLightBase::sendEvent(1000, req);
                    ColoredOutput::print(id, currentColor, 
                        "🔔 Sending RequestSwitch to controller");
                }
            }
            break;
        }
        
        case EventType::SwitchCommand: {
            std::string colorStr = (event.color == TrafficColor::Green) ? "GREEN" : "RED";
            ColoredOutput::print(id, currentColor, 
            "📥 Received SwitchCommand to set " + colorStr);
            setColor(event.color);
            break;
        }
        
        default:
            ColoredOutput::print(id, currentColor, "Unknown event type");
            break;
    }
}