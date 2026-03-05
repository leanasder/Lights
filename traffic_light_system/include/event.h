// 05.03.2026

#pragma once

#include " colored_output.h"

// types of events
enum class EventType {
    QueueAlert, // "My queue has reached the threshold!" (from the traffic light to the perpendicular one)
    RequestSwitch, // "I have few cars, switch to the traffic jam!" (from traffic light to controller)
    SwitchCommand // "Switch to this color!" (from controller to traffic lights)
};

// the structure of event
struct Event {
    // required fields for all types
    int senderId; // ID sender (who has created the event)
    int targetId; // ID reciever ( who intended for)
    Event type; // type of event

    // Data for QueueAlert and RequestSwitch
    int queueLength; // the sender's length queue 

    // for RequestSwitch: ID of the traffic light with the traffic jam (sender of the original QueueAlert)
    int congestedId; // to whom to give the green light (the one with the most cars)

    // for SwitchCommand: what color to turn on
    TrafficColor color; // GREEN or RED

    // ctor for QueueAlert (4 parameters)
    Event(int from, int to, EventType t, int q)
        : senderId(from), targetId(to), type(t), queueLength(q), congesedId(-1), color(TrafficColor::Red) {}

    // ctor for RequestSwitch (5 parameters)
    Event(int from, int to, EventType t, int q, int congested)
        : senderId(from), targetId(to), type(t), queueLength(q), congestedId(congested), color (TrafficColor::Red) {}

    // ctor for SwitchCommand (4 parameters: from, to, type, color)
    Event(int from, int to, EventType t, TrafficColor c)
        : senderId(from), targerId(to), type(t), queueLength(0), congestedId(-1), color(c) {}
}