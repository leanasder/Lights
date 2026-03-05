//27.02.2026
// 05.03.2026

#pragma once

#include "traffic_light_base.h"
class Camera {
    int cameraId;
    TrafficLightBase* observedLight; 
    
public:
    // ctor: the camera receives an ID and a pointer to the traffic light it is monitoring
    Camera(int id, TrafficLightBase* target)
        : cameraId(id), observedLight(target) {}

    // get the queue length from the monitored traffic light
    int getQueueLength() const {
        return observedLight->getQueueLength();
    }

    // for debugging and identify
    int getId() const { return cameraId; }    
};