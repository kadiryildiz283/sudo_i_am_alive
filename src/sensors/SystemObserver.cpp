#include "sensors/SystemObserver.hpp"
#include <iostream>
#include <array>
#include <memory>

namespace sensors {
    SystemObserver::SystemObserver() : running(false) {}
    SystemObserver::~SystemObserver() { stop_journal_monitor(); }

    std::string SystemObserver::scan_environment(const std::string& home_dir) {
        // RASYONEL TARAMA: Çok derinlere inmeden (maxdepth 3) aktif projeleri sayar.
        std::string cmd = "find " + home_dir + " -maxdepth 3 -type d -name '.git' 2>/dev/null | wc -l";
        std::array<char, 128> buffer;
        std::string result = "0";
        
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (pipe && fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result = std::string(buffer.data());
            result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        }
        
        return "ENVIRONMENT: Kadir has " + result + " active software projects (git repositories) in his workspace. He focuses on continuous development.";
    }

    void SystemObserver::start_journal_monitor() {
        if (running) return;
        running = true;
        monitor_thread = std::thread(&SystemObserver::journal_loop, this);
    }

    void SystemObserver::stop_journal_monitor() {
        running = false;
        if (monitor_thread.joinable()) monitor_thread.join();
    }

    bool SystemObserver::pop_error(std::string& error_msg) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (error_queue.empty()) return false;
        error_msg = error_queue.front();
        error_queue.pop();
        return true;
    }

    void SystemObserver::journal_loop() {
        // Sadece en kritik hataları (err, crit, alert, emerg) gerçek zamanlı dinler
        FILE* pipe = popen("journalctl -f -n 0 -p err..emerg 2>/dev/null", "r");
        if (!pipe) return;
        
        char buffer[1024];
        while (running && fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string err(buffer);
            if (err.length() > 15) { // Çok kısa anlamsız logları atla
                std::lock_guard<std::mutex> lock(queue_mutex);
                error_queue.push(err);
            }
        }
        pclose(pipe);
    }
}
