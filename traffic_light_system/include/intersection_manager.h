// 11.03.2026
#pragma once

#include "car_traffic_light.h"
#include "pedestrian_traffic_light.h"
#include <vector>
#include <memory>

class IntersectionManager {
    // Car lights
    CarTrafficLight* ns;  // North-South (id 0)
    CarTrafficLight* sn;  // South-North (id 2)
    CarTrafficLight* we;  // West-East (id 1)
    CarTrafficLight* ew;  // East-West (id 3)
    
    // Pedestrian lights by direction
    std::vector<PedestrianTrafficLight*> northPeds;   // N1, N2 (ids 4,5)
    std::vector<PedestrianTrafficLight*> southPeds;   // S1, S2 (ids 6,7)
    std::vector<PedestrianTrafficLight*> westPeds;    // W1, W2 (ids 8,9)
    std::vector<PedestrianTrafficLight*> eastPeds;    // E1, E2 (ids 10,11)
    
    // Constants for decision making
    static constexpr float PEDESTRIAN_ADVANTAGE = 1.5f;  // pedestrians win if they have 1.5x more
    
    // Helper methods
    void checkSNConflict();  // SN right turn vs east pedestrians
    void checkNSConflict();  // NS right turn vs west pedestrians
    void checkWEConflict();  // WE right turn vs south pedestrians
    void checkEWConflict();  // EW right turn vs north pedestrians
    
    int getTotalQueue(const std::vector<PedestrianTrafficLight*>& peds);
    
public:
    IntersectionManager(
        CarTrafficLight* nsPtr, CarTrafficLight* snPtr, 
        CarTrafficLight* wePtr, CarTrafficLight* ewPtr,
        std::vector<PedestrianTrafficLight*> north, 
        std::vector<PedestrianTrafficLight*> south,
        std::vector<PedestrianTrafficLight*> west, 
        std::vector<PedestrianTrafficLight*> east);
    
    void update();  // call this every second
};