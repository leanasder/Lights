// 11.03.2026
#include "intersection_manager.h"
#include "colored_output.h"
#include "traffic_controller.h"
#include <algorithm>

IntersectionManager::IntersectionManager(
    CarTrafficLight* nsPtr, CarTrafficLight* snPtr, 
    CarTrafficLight* wePtr, CarTrafficLight* ewPtr,
    std::vector<PedestrianTrafficLight*> north, 
    std::vector<PedestrianTrafficLight*> south,
    std::vector<PedestrianTrafficLight*> west, 
    std::vector<PedestrianTrafficLight*> east)
    : ns(nsPtr), sn(snPtr), we(wePtr), ew(ewPtr),
      northPeds(north), southPeds(south), westPeds(west), eastPeds(east) {}

int IntersectionManager::getTotalQueue(const std::vector<PedestrianTrafficLight*>& peds) {
    int total = 0;
    for (auto* ped : peds) {
        total += ped->getQueueLength();
    }
    return total;
}

void IntersectionManager::checkSNConflict() {
    // SN (South-North) turning right → conflicts with east pedestrians (E1, E2)
    if (sn->getCurrentColor() == TrafficColor::Green) {
        int rightCars = sn->getRightCount();
        int eastPedsTotal = getTotalQueue(eastPeds);
        
        if (eastPedsTotal > rightCars * PEDESTRIAN_ADVANTAGE && eastPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ SN right-turn: " + 
                std::to_string(eastPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - PEDESTRIANS WIN");
            // Here we would send an event to controller to give pedestrians green
            // For now, just print
        } else if (rightCars > 0 && eastPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ SN right-turn: " + 
                std::to_string(eastPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - CARS WIN");
        }
    }
}

void IntersectionManager::checkNSConflict() {
    // NS (North-South) turning right → conflicts with west pedestrians (W1, W2)
    if (ns->getCurrentColor() == TrafficColor::Green) {
        int rightCars = ns->getRightCount();
        int westPedsTotal = getTotalQueue(westPeds);
        
        if (westPedsTotal > rightCars * PEDESTRIAN_ADVANTAGE && westPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ NS right-turn: " + 
                std::to_string(westPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - PEDESTRIANS WIN");
        } else if (rightCars > 0 && westPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ NS right-turn: " + 
                std::to_string(westPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - CARS WIN");
        }
    }
}

void IntersectionManager::checkWEConflict() {
    // WE (West-East) turning right → conflicts with south pedestrians (S1, S2)
    if (we->getCurrentColor() == TrafficColor::Green) {
        int rightCars = we->getRightCount();
        int southPedsTotal = getTotalQueue(southPeds);
        
        if (southPedsTotal > rightCars * PEDESTRIAN_ADVANTAGE && southPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ WE right-turn: " + 
                std::to_string(southPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - PEDESTRIANS WIN");
        } else if (rightCars > 0 && southPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ WE right-turn: " + 
                std::to_string(southPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - CARS WIN");
        }
    }
}

void IntersectionManager::checkEWConflict() {
    // EW (East-West) turning right → conflicts with north pedestrians (N1, N2)
    if (ew->getCurrentColor() == TrafficColor::Green) {
        int rightCars = ew->getRightCount();
        int northPedsTotal = getTotalQueue(northPeds);
        
        if (northPedsTotal > rightCars * PEDESTRIAN_ADVANTAGE && northPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ EW right-turn: " + 
                std::to_string(northPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - PEDESTRIANS WIN");
        } else if (rightCars > 0 && northPedsTotal > 0) {
            ColoredOutput::printInfo("⚖️ EW right-turn: " + 
                std::to_string(northPedsTotal) + " pedestrians vs " + 
                std::to_string(rightCars) + " right-turning cars - CARS WIN");
        }
    }
}

void IntersectionManager::update() {
    // Call this every second to check for right-turn conflicts
    checkSNConflict();
    checkNSConflict();
    checkWEConflict();
    checkEWConflict();
}