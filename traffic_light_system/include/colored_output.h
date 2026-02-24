#ifndef COLORED_OUTPUT_H
#define COLORED_OUTPUT_H

#include <mutex>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>

enum class TrafficColor { Red, Green, Yellow };

class ColoredOutput {
    static std::mutex cout_mutex;

public:
    static void print(int lightId, TrafficColor color, const std::string& message);
    static void printPhase(const std::string& phase);
    static void printInfo(const std::string& info);

private:
    static std::string getTimestamp();
};

#endif //COLORED_OUTPUT_H