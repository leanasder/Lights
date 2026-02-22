//28.01.2026
//09.02.2026
//19.02.2026


#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <iomanip>

using namespace std::chrono_literals;
enum class TrafficColor { Red, Green, Yellow };

// Класс для потокобезопасного вывода с цветом
class ColoredOutput {
    static std::mutex cout_mutex;
    
public:
    static void print(int lightId, TrafficColor color, const std::string& message) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        
        const char* colorCode = "";
        const char* colorName = "";
        const char* symbol = "";
        
        switch (color) {
            case TrafficColor::Green:
                colorCode = "\033[1;32m"; // Ярко-зеленый
                colorName = "GREEN";
                symbol = "🟢";
                break;
            case TrafficColor::Yellow:
                colorCode = "\033[1;33m"; // Ярко-желтый
                colorName = "YELLOW";
                symbol = "🟡";
                break;
            case TrafficColor::Red:
                colorCode = "\033[1;31m"; // Ярко-красный
                colorName = "RED";
                symbol = "🔴";
                break;
        }
        
        // Форматированный вывод с цветом
        std::cout << "[" << getTimestamp() << "] "
                  << symbol << " Light " << lightId << ": "
                  << colorCode << colorName << "\033[0m"
                  << " - " << message << std::endl;
    }
    
    static void printPhase(const std::string& phase) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\n\033[1;36m" << "=== " << phase << " ===\033[0m" << std::endl;
    }
    
    static void printInfo(const std::string& info) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\033[1;37m" << info << "\033[0m" << std::endl;
    }
    
private:
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        auto timer = std::chrono::system_clock::to_time_t(now);
        std::tm bt = *std::localtime(&timer);
        
        std::ostringstream oss;
        oss << std::put_time(&bt, "%H:%M:%S") << '.' 
            << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }
};

std::mutex ColoredOutput::cout_mutex;

class TrafficController;

class SynchronizedTrafficLight {
    std::atomic<TrafficColor> currentColor{TrafficColor::Red};
    std::atomic<bool> isRunning{false};
    std::unique_ptr<std::thread> workerThread;
    
    TrafficController* controller;
    int lightId;
    
    void cycleColors();
    
public:
    SynchronizedTrafficLight(int id, TrafficController* ctrl)
        : lightId(id), controller(ctrl) {
    }
    
    ~SynchronizedTrafficLight() { stop(); }
    
    void start();
    void stop();
    
    TrafficColor getCurrentColor() const { return currentColor.load(); }
    void setColor(TrafficColor color) { 
        currentColor = color; 
        // Немедленно отображаем изменение цвета
        ColoredOutput::print(lightId, color, "Color changed");
    }
    int getId() const { return lightId; }
    
    SynchronizedTrafficLight(const SynchronizedTrafficLight&) = delete;
    SynchronizedTrafficLight& operator=(const SynchronizedTrafficLight&) = delete;
};

class TrafficController {
    std::vector<SynchronizedTrafficLight*> lights;
    std::atomic<bool> isRunning{false};
    std::atomic<bool> isStopping{false};
    std::thread controllerThread;
    
    std::mutex mtx;
    std::condition_variable cv;
    bool allLightsReady{false};
    
    std::chrono::seconds greenDuration{5};
    std::chrono::seconds yellowDuration{2};
    
    void controlCycle(){
        using namespace std::chrono;
       
        // Инициализация
        ColoredOutput::printInfo("Initializing traffic lights...");
        lights[0]->setColor(TrafficColor::Green);
        lights[1]->setColor(TrafficColor::Red);
        
        // Первое уведомление
        {
            std::lock_guard<std::mutex> lock(mtx);
            allLightsReady = true;
        }
        cv.notify_all();
        std::this_thread::sleep_for(200ms);
        
        int cycleCount = 0;
            while (isRunning.load() && !isStopping.load()) {
                cycleCount++;
                
                // ===== ФАЗА 1 =====
                ColoredOutput::printPhase("PHASE " + std::to_string(cycleCount) + 
                                        ": Light 0 GREEN / Light 1 RED");
                
                lights[0]->setColor(TrafficColor::Green);
                lights[1]->setColor(TrafficColor::Red);
                
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    allLightsReady = true;
                }
                cv.notify_all();

                        size_t duration_seconds_green = static_cast<size_t>(greenDuration.count());
                        for (size_t i = duration_seconds_green; i > 0; --i) {
                            if (isRunning.load() && ! isStopping.load()) { 
                            std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                            std::this_thread::sleep_for(1s);
                            }
                        }
                // } 
                
                if (!isRunning.load() || isStopping.load()) break;
                
                // ===== ПЕРЕХОД 1 =====
                ColoredOutput::printPhase("TRANSITION: Light 0 YELLOW / Light 1 RED");
                
                lights[0]->setColor(TrafficColor::Yellow);
                lights[1]->setColor(TrafficColor::Red);
                
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    allLightsReady = true;
                }
                cv.notify_all();
                
                size_t duration_seconds = static_cast<size_t>(yellowDuration.count());
                    for (size_t i = duration_seconds; i > 0; --i) {
                        if (isRunning.load() && ! isStopping.load()) { 
                        std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                        std::this_thread::sleep_for(1s);
                        }
                    }
               
                
                if (!isRunning.load() || isStopping.load()) break;
                
                // ===== ФАЗА 2 =====
                ColoredOutput::printPhase("PHASE " + std::to_string(cycleCount) + 
                                        ": Light 0 RED / Light 1 GREEN");
                
                lights[0]->setColor(TrafficColor::Red);
                lights[1]->setColor(TrafficColor::Green);
                
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    allLightsReady = true;
                }
                cv.notify_all();
                
                        if (isRunning.load() && !isStopping.load()) { 
                        
        
                        size_t duration_seconds = static_cast<size_t>(greenDuration.count());
                        for (size_t i = duration_seconds; i > 0; --i) {
                            if (isRunning.load() && ! isStopping.load()) { 
                                std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                                std::this_thread::sleep_for(1s);
                            }
                        } 
                        } 
                if (!isRunning.load() || isStopping.load()) break;
                
                // ===== ПЕРЕХОД 2 =====
                ColoredOutput::printPhase("TRANSITION: Light 0 RED / Light 1 YELLOW");
                
                lights[0]->setColor(TrafficColor::Red);
                lights[1]->setColor(TrafficColor::Yellow);
                
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    allLightsReady = true;
                }
                cv.notify_all();
                
                if (isRunning.load() && ! isStopping.load()) { 
                   for (size_t i = duration_seconds; i > 0; --i) {
                            if (isRunning.load() && ! isStopping.load()) { 
                        std::cout << std::to_string(i) + " seconds remaining" << std::endl;
                        std::this_thread::sleep_for(1s);
                    }
                    }
                    // std::this_thread::sleep_for(100ms);
                }
            }
        
        // Завершение
        {
            std::lock_guard<std::mutex> lock(mtx);
            allLightsReady = true;
        }
        cv.notify_all();
        ColoredOutput::printInfo("Controller stopped");
    }    
    
public:

    TrafficController(std::chrono::seconds greenTime = 5s, 
                     std::chrono::seconds yellowTime = 2s)
        : greenDuration(greenTime), yellowDuration(yellowTime) {
    }
    
    ~TrafficController() { stop(); }
    
    void registerLight(SynchronizedTrafficLight* light) {
        lights.push_back(light);
    }
    
    void start() {
        if (isRunning.load()) return;
        
        isRunning = true;
        isStopping = false;
        controllerThread = std::thread([this]() { controlCycle(); });
    }
    
    void stop() {
        if (!isRunning.load()) return;
        
        isStopping = true;
        {
            std::lock_guard<std::mutex> lock(mtx);
            allLightsReady = true;
        }
        cv.notify_all();
        
        if (controllerThread.joinable()) {
            controllerThread.join();
        }
        
        isRunning = false;
        ColoredOutput::printInfo("Controller fully stopped");
    }
    
    bool waitForSignal(SynchronizedTrafficLight* light) {
        std::unique_lock<std::mutex> lock(mtx);
        
        cv.wait(lock, [this]() { 
            return allLightsReady || isStopping.load(); 
        });
        
        if (isStopping.load()) {
            return false;
        }
        
        if (allLightsReady && !isStopping.load()) {
            allLightsReady = false;
        }
        
        return true;
    }
    
    bool isControllerRunning() const { 
        return isRunning.load() && !isStopping.load(); 
    }
};

void SynchronizedTrafficLight::cycleColors() {
    ColoredOutput::print(lightId, TrafficColor::Red, "Light started");
    
    using namespace std::chrono_literals;

    while (isRunning.load() && controller->isControllerRunning()) {
        if (!controller->waitForSignal(this)) {
            break;
        }
        
        // Короткая пауза для красивого вывода
        std::this_thread::sleep_for(100ms);
    }
    
    ColoredOutput::print(lightId, currentColor.load(), "Light stopped");
}

void SynchronizedTrafficLight::start() {
    if (isRunning.load()) return;
    
    isRunning = true;
    workerThread = std::make_unique<std::thread>([this]() {
        cycleColors();
    });
}

void SynchronizedTrafficLight::stop() {
    isRunning = false;
    if (workerThread && workerThread->joinable()) {
        workerThread->join();
    }
}

int main() {
    using namespace std::chrono_literals;
    
    // Яркое приветствие
    std::cout << "\033[1;35m" 
              << "========================================" << std::endl
              << "   SYNCHRONIZED TRAFFIC LIGHTS SYSTEM   " << std::endl
              << "========================================" 
              << "\033[0m" << std::endl;
    
    // Создание системы
    auto GREEN_RED_DUR = 10s;
    auto YELLOW_DUR = 2s;
    auto WORK_DUR = 60s;
    TrafficController controller(GREEN_RED_DUR, YELLOW_DUR); // 4 сек зеленый, 1 сек желтый
    
    SynchronizedTrafficLight light0(0, &controller);
    SynchronizedTrafficLight light1(1, &controller);
    
    controller.registerLight(&light0);
    controller.registerLight(&light1);
    
    // Информация о системе
    ColoredOutput::printInfo("System configuration:");
    ColoredOutput::printInfo("  • Green duration: " + std::to_string(GREEN_RED_DUR.count()) + " seconds");
    ColoredOutput::printInfo("  • Yellow duration: " + std::to_string(YELLOW_DUR.count()) + " seconds");
    ColoredOutput::printInfo("  • Light 0: North-South direction");
    ColoredOutput::printInfo("  • Light 1: East-West direction");
    ColoredOutput::printInfo("Starting in 2 seconds...");
    
    std::this_thread::sleep_for(2s);
    
    // Запуск
    light0.start();
    light1.start();
    std::this_thread::sleep_for(100ms);
    controller.start();
    
    // Работа системы
    std::this_thread::sleep_for(WORK_DUR);
    
    // Остановка
    ColoredOutput::printInfo("\nInitiating system shutdown...");
    controller.stop();
    light0.stop();
    light1.stop();
    
    // Завершение
    std::cout << "\033[1;32m" 
              << "========================================" << std::endl
              << "      SYSTEM STOPPED SUCCESSFULLY       " << std::endl
              << "========================================" 
              << "\033[0m" << std::endl;
    
    return 0;
}