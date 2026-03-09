// 06.03.2026

#ifndef TRAFFIC_CONTROLLER_H
#define TRAFFIC_CONTROLLER_H

#include "traffic_light_base.h"
#include "event.h"
#include "car_traffic_light.h"
#include <vector>

class TrafficController : public TrafficLightBase {
    std::vector<TrafficLightBase*> lights;

   // Новые константы
    static constexpr std::chrono::seconds MIN_GREEN_TIME{20};
    static constexpr std::chrono::seconds MAX_GREEN_TIME{30};

    // 09.03.2026
    // Структура для хранения состояния фазы
    struct PhaseInfo {
        DirectionGroup currentGroup;
        std::chrono::steady_clock::time_point phaseStartTime;
        bool switchRequested;
        int requestingLeader;
        
        std::chrono::seconds elapsed() const {
            return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - phaseStartTime);
        }
    } phase;

    // Новый поток для контроля времени
    std::thread timingThread;
    void timingLoop();

    // Новые методы
    void startPhase(DirectionGroup group);
    void checkPhaseTiming();
    void processSwitchRequest(int requestingLeader);
    void switchToOppositeGroup();

protected:
    void processEvent(const Event& event) override;

public:
    static constexpr int CONTROLLER_ID = 1000;

    TrafficController();
    ~TrafficController();
    void start() override;
    void stop();


    void registerLight(TrafficLightBase* light);

    void processEvents() override;
    void handleEvent() override;
    TrafficColor getCurrentColor() const override ;
    int getQueueLength() const override ;
};

#endif