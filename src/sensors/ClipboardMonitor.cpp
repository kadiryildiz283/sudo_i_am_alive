#include "sensors/ClipboardMonitor.hpp"
#include <iostream>
#include <cstdlib>
#include <xcb/xcb.h>
#include <xcb/xfixes.h>

namespace sensors {

    ClipboardMonitor::ClipboardMonitor() : running(false) {}

    ClipboardMonitor::~ClipboardMonitor() { stop(); }

    void ClipboardMonitor::start() {
        running = true;
        monitor_thread = std::thread([this]() {
            if (std::getenv("WAYLAND_DISPLAY")) {
                std::cout << "[SENSOR] Wayland detected. (Using XWayland fallback for Phase 1)\n";
                run_x11(); // Simplified for Phase 1 stability [cite: 49]
            } else {
                run_x11();
            }
        });
    }

    void ClipboardMonitor::stop() {
        running = false;
        if (monitor_thread.joinable()) monitor_thread.join();
    }

    bool ClipboardMonitor::pop_event(ClipboardEvent& event) {
        return event_queue.pop(event);
    }

    void ClipboardMonitor::run_x11() {
        xcb_connection_t* conn = xcb_connect(NULL, NULL);
        if (xcb_connection_has_error(conn)) return;

        const xcb_setup_t* setup = xcb_get_setup(conn);
        xcb_screen_t* screen = xcb_setup_roots_iterator(setup).data;

        // Register for XFixes selection notify events [cite: 50, 51]
        xcb_xfixes_query_version(conn, XCB_XFIXES_MAJOR_VERSION, XCB_XFIXES_MINOR_VERSION);
        xcb_xfixes_select_selection_input(conn, screen->root, 
            XCB_ATOM_PRIMARY, XCB_XFIXES_SELECTION_EVENT_MASK_SET_SELECTION_OWNER);
        
        xcb_flush(conn);

        while (running) {
            xcb_generic_event_t* event = xcb_poll_for_event(conn);
            if (event) {
                // When clipboard ownership changes [cite: 51]
                ClipboardEvent ce;
                ce.content = "[Clipboard Content Changed]"; // Real content retrieval requires async pipe
                ce.timestamp = time(nullptr);
                event_queue.push(ce);
                free(event);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        xcb_disconnect(conn);
    }
}
