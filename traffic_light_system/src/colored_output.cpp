#include "colored_output.h"

std::mutex ColoredOutput::cout_mutex;
void ColoredOutput::print(int lightId< TrafficColor color, const std::string& message) {
    std::lock_guard<std::mutex> lock(cout_mutex);

    const char* colorCode = "";
    const char* colorName = "";
    const char* symbol = "";

    switch (color) {
        case TrafficColor::Green:
            colorCode = "\033[1;32m";
            colorName = "GREEN";
            symbol = "🟢";
            break;
        case TrafficColor::Yellow:
            colorCode = "\033[1;33m";
            colorName = "EELLOW";
            symbol = "🟡";
            break;
        case TrafficColor::Red:
            colorCode = "\033[1;31m";
            colorName = "RED";
            symbol = "🔴";
            break;
    }

    std::cout << "[" << getTimestamp() << "] "
              << symbol << " Light " << lightId << ": "
              << colorCode << colorName << "\033[0m"
              << " - " << message << std::endl;
}

void ColoredOutput::printPhase(const std::string& phase) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "\n\033[1;36m" << "=== " phase << " ===\033[0m" << std::endl;
}

void ColoredOutput::printInfo(const std::string& info) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "\033[1;37m" << info << "\033[0m" <<std::endl;
}

std::string ColoredOutput::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%H:%M:%S") << '.'
        <<std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
}