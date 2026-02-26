#pragma once

#include <string>
#include <thread>
#include <functional>
#include <ctime>
#include <queue>
#include <mutex>
#include <atomic>

namespace sensors {

    // DATA STRUCTURE: Represents a single intercepted command event
    struct TerminalEvent {
        std::string command;
        time_t timestamp;
    };

    class TerminalMonitor {
    public:
        // CONSTRUCTOR: Initializes IPC socket path and the cognitive gatekeeper callback
        TerminalMonitor(const std::string& socket_path, std::function<bool(const std::string&)> gatekeeper_cb);
        ~TerminalMonitor();

        // LIFECYCLE: Thread management for the background socket listener
        void start();
        void stop();

        // QUEUE CONSUMER: Main thread calls this to retrieve processed events safely
        bool pop_event(TerminalEvent& event);

    private:
        // BACKGROUND TASK: Actively listens on the Unix socket and evaluates incoming commands
        void run_loop();

        std::string sock_path;
        std::atomic<bool> running;
        std::thread monitor_thread;
        
        // COGNITIVE FILTER: Lambda function injected from main.cpp to decide BLOCK or OK
        std::function<bool(const std::string&)> gatekeeper;
        
        // SYSTEM RESOURCE: File descriptor for the Unix Domain Socket
        int server_fd;

        // THREAD-SAFE MEMORY: Queue mechanism to transfer events to the main thread
        std::queue<TerminalEvent> event_queue;
        std::mutex queue_mutex;
    };

} // namespace sensors
