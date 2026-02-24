#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>

namespace sensors {

    struct TerminalEvent {
        std::string command;
        long timestamp;
    };

    class TerminalMonitor {
    private:
        std::string history_path;
        std::atomic<bool> running;
        std::thread monitor_thread;
        
        // Lock-free queue for high-performance event passing [cite: 37, 38]
        boost::lockfree::spsc_queue<TerminalEvent, boost::lockfree::capacity<1024>> event_queue;

        void run_loop(); // Internal epoll loop [cite: 30, 34]

    public:
        explicit TerminalMonitor(const std::string& path);
        ~TerminalMonitor();

        void start();
        void stop();
        
        // Used by Cognitive Router to fetch captured data
        bool pop_event(TerminalEvent& event);
    };

} // namespace sensors
