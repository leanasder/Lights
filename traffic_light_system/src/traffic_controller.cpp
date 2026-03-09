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
    phase.currentGroup = group;
    phase.phaseStartTime = std::chrono::steady_clock::now();
    phase.switchRequested = false;
    
    ColoredOutput::printInfo("🟢 Starting " + 
        std::string(group == DirectionGroup::NorthSouth ? "NS" : "WE") + 
        " phase (min=20s, max=30s)");
    
    // Отправляем команды всем светофорам
    for (auto* light : lights) {
        if (!light) continue;
        
        bool isNS = (light->getId() == 0 || light->getId() == 2);
        TrafficColor newColor = (isNS == (group == DirectionGroup::NorthSouth)) 
            ? TrafficColor::Green : TrafficColor::Red;
        
        Event cmd(CONTROLLER_ID, light->getId(), EventType::SwitchCommand, newColor);
        TrafficLightBase::sendEvent(light->getId(), cmd);
    }
}

void TrafficController::processEvent(const Event& event) {
    if (event.type == EventType::RequestSwitch) {
        processSwitchRequest(event.senderId);
    }
}

void TrafficController::processSwitchRequest(int requestingLeader) {
    auto elapsed = phase.elapsed();
    
    ColoredOutput::printInfo("📨 Switch request from Light " + 
        std::to_string(requestingLeader) + " at " + 
        std::to_string(elapsed.count()) + "s");
    
    if (elapsed < MIN_GREEN_TIME) {
        // Рано переключаться - запоминаем запрос
        phase.switchRequested = true;
        phase.requestingLeader = requestingLeader;
        ColoredOutput::printInfo("⏳ Request queued - need " + 
            std::to_string((MIN_GREEN_TIME - elapsed).count()) + 
            "s more of minimum time");
    } else {
        // Уже прошло минимум 20с - можно переключаться
        ColoredOutput::printInfo("🔄 Immediate switch after " + 
            std::to_string(elapsed.count()) + "s");
        switchToOppositeGroup();
    }
}

void TrafficController::timingLoop() {
    while (isRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        checkPhaseTiming();
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