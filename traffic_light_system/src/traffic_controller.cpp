// 06.03.2026
#include "traffic_controller.h"
#include "colored_output.h"

TrafficController::TrafficController() 
    : TrafficLightBase(CONTROLLER_ID) {
    ColoredOutput::printInfo("Controller created with ID " + std::to_string(CONTROLLER_ID));
}

TrafficController::~TrafficController() {
    stop();
}

void TrafficController::start() {
    TrafficLightBase::start();
    ColoredOutput::printInfo("Controller started");
}

void TrafficController::stop() {
    TrafficLightBase::stop();
    ColoredOutput::printInfo("Controller stopped");
}

void TrafficController::registerLight(TrafficLightBase* light) {
    if (light) {
        lights.push_back(light);
        ColoredOutput::printInfo("Light " + std::to_string(light->getId()) + 
                                " registered with controller");
    }
}

void TrafficController::processEvent(const Event& event) {
    switch (event.type) {
        case EventType::RequestSwitch: {
            // Лидер просит переключить фазы в его пользу
            ColoredOutput::printInfo("🔔 CONTROLLER: Switch request from Light " + 
                std::to_string(event.senderId) + 
                " (myTotal=" + std::to_string(event.queueLength) + 
                " > opponent=" + std::to_string(event.congestedId) + ")");
            
            // Определяем, кто есть кто
            int requestingLeader = event.senderId;  // кто просит (лидер с большей суммой)
            
            // Отправляем команды всем светофорам
            for (auto* light : lights) {
                if (!light) continue;
                
                // Определяем группу светофора по ID (временная логика)
                // NS группа: 0,2 | WE группа: 1,3
                bool isNSGroup = (light->getId() == 0 || light->getId() == 2);
                bool isRequesterNS = (requestingLeader == 0);  // NS лидер просит
                
                TrafficColor newColor;
                if ((isNSGroup && isRequesterNS) || (!isNSGroup && !isRequesterNS)) {
                    newColor = TrafficColor::Green;  // группе просящего - зелёный
                } else {
                    newColor = TrafficColor::Red;    // другой группе - красный
                }

                ColoredOutput::printInfo("📤 Controller sending SwitchCommand to Light " + 
                std::to_string(light->getId()) + " color=" + 
                (newColor == TrafficColor::Green ? "GREEN" : "RED"));
                
                Event cmd(CONTROLLER_ID, light->getId(), EventType::SwitchCommand, newColor);
                TrafficLightBase::sendEvent(light->getId(), cmd);
            }
            break;
        }
        
        case EventType::QueueAlert:
        case EventType::QueryPartnerQueue:
        case EventType::PartnerQueueResponse:
        case EventType::QueryOpponentGroup:
        case EventType::OpponentGroupResponse:
            // Контроллер игнорирует эти события
            break;
            
        default:
            ColoredOutput::printInfo("Controller received unknown event type");
            break;
    }
}

// Заглушки для чисто виртуальных методов
void TrafficController::processEvents() {
    // Не используется
}

void TrafficController::handleEvent() {
    // Не используется
}

TrafficColor TrafficController::getCurrentColor() const {
    return TrafficColor::Red;  // у контроллера нет цвета
}

int TrafficController::getQueueLength() const {
    return 0;  // у контроллера нет очереди
}