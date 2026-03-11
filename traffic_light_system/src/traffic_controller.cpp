// 06.03.2026
// 09.03.2026
#include "traffic_controller.h"
#include "colored_output.h"
#include <thread>

TrafficController::TrafficController() 
    : TrafficLightBase(CONTROLLER_ID) {
    ColoredOutput::printInfo("Controller created with ID " + std::to_string(CONTROLLER_ID));
}

TrafficController::~TrafficController() {
    stop();
}

void TrafficController::start() {
    TrafficLightBase::start();
    
    // Запускаем новый поток контроля времени
    timingThread = std::thread(&TrafficController::timingLoop, this);
    
    // Начинаем с NS-группы
    startPhase(DirectionGroup::NorthSouth);
    
    ColoredOutput::printInfo("Controller started");
}

void TrafficController::stop() {
    TrafficLightBase::stop();
    if (timingThread.joinable()) {
        timingThread.join();
    }
    ColoredOutput::printInfo("Controller stopped");
}

void TrafficController::startPhase(DirectionGroup group) {
    // Clear pending requests when starting a new phase
    pendingGroup.reset();
    pendingLeaderId.reset();
    
    phase.currentGroup = group;
    phase.phaseStartTime = std::chrono::steady_clock::now();
    phase.switchRequested = false;
    
    ColoredOutput::printInfo("🟢 Starting " + 
        std::string(group == DirectionGroup::NorthSouth ? "NS" : "WE") + 
        " phase (min=20s, max=30s)");
    
    // Send commands to ALL traffic lights
    for (auto* light : lights) {
        if (!light) continue;
        
        TrafficColor newColor;
        
        if (light->getId() < 4) {
            // 🚗 CAR LIGHTS (IDs 0-3)
            bool isNS = (light->getId() == 0 || light->getId() == 2);
            newColor = (isNS == (group == DirectionGroup::NorthSouth)) 
                ? TrafficColor::Green : TrafficColor::Red;
        } else {
            // 🚶 PEDESTRIAN LIGHTS (IDs 4-11)
            // During car phases, ALL pedestrians are RED
            newColor = TrafficColor::Red;
        }
        
        Event cmd(CONTROLLER_ID, light->getId(), EventType::SwitchCommand, newColor);
        TrafficLightBase::sendEvent(light->getId(), cmd);
    }
}
void TrafficController::processEvent(const Event& event) {
    if (event.type == EventType::RequestSwitch) {
        processSwitchRequest(event.senderId);
    }
    else if (event.type == EventType::PedestrianQueueUpdate) {
        updateTotalPedestrians();
    }
}

void TrafficController::processSwitchRequest(int requestingLeader) {
    // 🆕 If pedestrian phase is active, remember the request
    if (pedestrianMode) {
        DirectionGroup requestedGroup = (requestingLeader == 0 || requestingLeader == 2) 
            ? DirectionGroup::NorthSouth : DirectionGroup::EastWest;
        
        pendingGroup = requestedGroup;
        pendingLeaderId = requestingLeader;
        
        ColoredOutput::printInfo("📝 Pedestrian phase active, remembering request from Light " + 
            std::to_string(requestingLeader) + 
            " (" + (requestedGroup == DirectionGroup::NorthSouth ? "NS" : "WE") + " group)");
        return;
    }
    
    // Normal switch request processing (existing code)
    auto elapsed = phase.elapsed();
    
    ColoredOutput::printInfo("📨 Switch request from Light " + 
        std::to_string(requestingLeader) + " at " + 
        std::to_string(elapsed.count()) + "s");
    
    if (elapsed < MIN_GREEN_TIME) {
        phase.switchRequested = true;
        phase.requestingLeader = requestingLeader;
        ColoredOutput::printInfo("⏳ Request queued - need " + 
            std::to_string((MIN_GREEN_TIME - elapsed).count()) + 
            "s more of minimum time");
    } else {
        ColoredOutput::printInfo("🔄 Immediate switch after " + 
            std::to_string(elapsed.count()) + "s");
        switchToOppositeGroup();
    }
}

void TrafficController::timingLoop() {
    while (isRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (pedestrianMode) {
            auto elapsed = std::chrono::steady_clock::now() - pedestrianPhaseStart;
            
            //  EARLY TERMINATION CHECK
            if (earlyTermination && elapsed >= MIN_PEDESTRIAN_TIME) {
                // We've already waited minimum time and queue is empty
                ColoredOutput::printInfo("🟢 Pedestrian queue empty, ending phase early after " +
                    std::to_string(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()) + "s");
                endPedestrianPhase();
            }
            // Normal max time check
            else if (elapsed >= MAX_PEDESTRIAN_TIME) {
                ColoredOutput::printInfo("🔴 Max pedestrian time reached (15s)");
                endPedestrianPhase();
            }
        } else {
            // Normal operation - check phase timing
            checkPhaseTiming();
        }
    }
}

void TrafficController::checkPhaseTiming() {
    auto elapsed = phase.elapsed();
    
    // Проверяем максимальное время (30с)
    if (elapsed >= MAX_GREEN_TIME) {
        ColoredOutput::printInfo("🔴 Max time reached (30s) - forcing switch");
        switchToOppositeGroup();
        return;
    }
    
    // Если есть отложенный запрос и прошло минимум 20с
    if (phase.switchRequested && elapsed >= MIN_GREEN_TIME) {
        ColoredOutput::printInfo("🟢 Executing queued switch request after " + 
            std::to_string(elapsed.count()) + "s");
        switchToOppositeGroup();
        phase.switchRequested = false;
    }
}

void TrafficController::switchToOppositeGroup() {
    DirectionGroup nextGroup = (phase.currentGroup == DirectionGroup::NorthSouth) 
        ? DirectionGroup::EastWest : DirectionGroup::NorthSouth;
    startPhase(nextGroup);
}

void TrafficController::registerLight(TrafficLightBase* light) {
    if (light) {
        lights.push_back(light);
        ColoredOutput::printInfo("Light " + std::to_string(light->getId()) + 
                                " registered with controller");
    }
}

// Заглушки для чисто виртуальных методов
void TrafficController::processEvents() {}
void TrafficController::handleEvent() {}
TrafficColor TrafficController::getCurrentColor() const { return TrafficColor::Red; }
int TrafficController::getQueueLength() const { return 0; }

// Update total pedestrian count
   void TrafficController::updateTotalPedestrians() {
    if (!isRunning.load()) return;
    
    int total = 0;
    for (auto* light : lights) {
        if (light->getId() >= 4) {
            total += light->getQueueLength();
        }
    }
    
    int oldTotal = totalPedestrians.load();
    totalPedestrians = total;
    
    // 🆕 CHECK FOR EARLY TERMINATION DURING PEDESTRIAN PHASE
    if (pedestrianMode) {
        auto elapsed = std::chrono::steady_clock::now() - pedestrianPhaseStart;
        
        // If minimum time passed AND queue is almost empty AND we haven't already triggered early termination
        if (elapsed >= MIN_PEDESTRIAN_TIME && total <= EMPTY_QUEUE_THRESHOLD && !earlyTermination) {
            earlyTermination = true;
            ColoredOutput::printInfo("🚶 Pedestrian queue almost empty (" + 
                std::to_string(total) + " left), will end phase soon");
        }
    }
    
    // Smart printing - only when significant changes happen
    static int lastReportedTotal = 0;
    bool significantChange = (abs(total - lastReportedTotal) >= 10);
    bool thresholdJustReached = (!pedestrianMode && oldTotal < PEDESTRIAN_THRESHOLD && total >= PEDESTRIAN_THRESHOLD);
    bool firstReport = (lastReportedTotal == 0);
    
    if (significantChange || thresholdJustReached || firstReport) {
        std::string msg = "🚶 Total pedestrians: " + std::to_string(total);
        if (thresholdJustReached) {
            msg += " - THRESHOLD REACHED!";
        }
        ColoredOutput::printInfo(msg);
        lastReportedTotal = total;
    }
    
    if (!pedestrianMode) {
        checkPedestrianThreshold();
    }
}


//  Check if pedestrian threshold is reached
void TrafficController::checkPedestrianThreshold() {
    if (totalPedestrians >= PEDESTRIAN_THRESHOLD) {
        ColoredOutput::printInfo("🚶🚶🚶 PEDESTRIAN THRESHOLD REACHED! " + 
            std::to_string(totalPedestrians.load()) + " people waiting");
        startPedestrianPhase();
    }
}

//  Start pedestrian phase (all cars red, all pedestrians green)
void TrafficController::startPedestrianPhase() {
    pedestrianMode = true;
    pedestrianPhaseStart = std::chrono::steady_clock::now();
     earlyTermination = false;  //  reset flag
    
    ColoredOutput::printInfo("🟢 Starting PEDESTRIAN phase for max " + 
        std::to_string(MAX_PEDESTRIAN_TIME.count()) + 
        " seconds (min " + std::to_string(MIN_PEDESTRIAN_TIME.count()) + 
        "s, will end early if queue empties)");

    // Send commands to ALL traffic lights
    for (auto* light : lights) {
        if (!light) continue;
        
        TrafficColor newColor;
        if (light->getId() < 4) {
            // Car lights (IDs 0-3) → RED
            newColor = TrafficColor::Red;
        } else {
            // Pedestrian lights (IDs 4-11) → GREEN
            newColor = TrafficColor::Green;
        }
        
        Event cmd(CONTROLLER_ID, light->getId(), EventType::SwitchCommand, newColor);
        TrafficLightBase::sendEvent(light->getId(), cmd);
    }
}

// End pedestrian phase, resume normal operation
void TrafficController::endPedestrianPhase() {
    pedestrianMode = false;
    
    auto actualDuration = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - pedestrianPhaseStart);
    
    // Determine who gets green next
    DirectionGroup nextGroup;
    
    if (pendingGroup.has_value()) {
        nextGroup = pendingGroup.value();
        ColoredOutput::printInfo("🔴 Pedestrian phase ended after " + 
            std::to_string(actualDuration.count()) + "s, granting green to " + 
            std::string(nextGroup == DirectionGroup::NorthSouth ? "NS" : "WE") + 
            " group (request from Light " + std::to_string(pendingLeaderId.value_or(-1)) + ")");
        
        pendingGroup.reset();
        pendingLeaderId.reset();
    } else {
        nextGroup = DirectionGroup::NorthSouth;
        ColoredOutput::printInfo("🔴 Pedestrian phase ended after " + 
            std::to_string(actualDuration.count()) + "s, resuming normal operation");
    }
    
    totalPedestrians = 0;
    startPhase(nextGroup);
}