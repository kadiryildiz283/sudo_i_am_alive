#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

namespace sensors {
    class SystemObserver {
    public:
        SystemObserver();
        ~SystemObserver();

        // 1. Çevresel Otonom Keşif (Proje Tarama)
        std::string scan_environment(const std::string& home_dir);

        // 3. Sistem Biyolojisi İzleme (journalctl - err)
        void start_journal_monitor();
        void stop_journal_monitor();
        bool pop_error(std::string& error_msg);

    private:
        void journal_loop();
        std::atomic<bool> running;
        std::thread monitor_thread;
        std::queue<std::string> error_queue;
        std::mutex queue_mutex;
    };
}
