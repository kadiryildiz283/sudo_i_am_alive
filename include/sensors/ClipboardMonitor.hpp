#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>

namespace sensors {

    struct ClipboardEvent {
        std::string content;
        long timestamp;
    };

    class ClipboardMonitor {
    private:
        std::atomic<bool> running;
        std::thread monitor_thread;
        boost::lockfree::spsc_queue<ClipboardEvent, boost::lockfree::capacity<512>> event_queue;

        void run_wayland(); // zwlr_data_control_manager_v1 logic [cite: 43]
        void run_x11();     // XCB XFixes logic [cite: 50]

    public:
        ClipboardMonitor();
        ~ClipboardMonitor();

        void start();
        void stop();
        bool pop_event(ClipboardEvent& event);
    };

} // namespace sensors
