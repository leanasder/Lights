// 05.03.2026

#include "traffic_light_base.h"

// defining static methods
std::map<int, TrafficLightBase*> TrafficLightBase::registry;
std::mutex TrafficLightBase::registryMutex;