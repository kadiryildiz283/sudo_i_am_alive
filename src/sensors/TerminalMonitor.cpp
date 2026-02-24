#include "sensors/TerminalMonitor.hpp"
#include <iostream>
#include <fstream>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

namespace sensors {

    TerminalMonitor::TerminalMonitor(const std::string& path) 
        : history_path(path), running(false) {}

    TerminalMonitor::~TerminalMonitor() {
        stop();
    }

    void TerminalMonitor::start() {
        running = true;
        monitor_thread = std::thread(&TerminalMonitor::run_loop, this);
    }

    void TerminalMonitor::stop() {
        running = false;
        if (monitor_thread.joinable()) {
            monitor_thread.join();
        }
    }

    bool TerminalMonitor::pop_event(TerminalEvent& event) {
        return event_queue.pop(event);
    }

    void TerminalMonitor::run_loop() {
        // Initialize inotify for file tracking 
        int fd = inotify_init1(IN_NONBLOCK);
        if (fd < 0) return;

        int wd = inotify_add_watch(fd, history_path.c_str(), IN_MODIFY);
        if (wd < 0) {
            close(fd);
            return;
        }

        // Use epoll to sleep efficiently until data arrives [cite: 33, 34, 58]
        int epoll_fd = epoll_create1(0);
        struct epoll_event ev, events[1];
        ev.events = EPOLLIN | EPOLLET; // Edge-triggered for performance [cite: 33]
        ev.data.fd = fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);

        std::ifstream file(history_path);
        file.seekg(0, std::ios::end);
        std::streampos last_pos = file.tellg();

        while (running) {
            int nfds = epoll_wait(epoll_fd, events, 1, 500); // 500ms timeout
            if (nfds > 0) {
                char buffer[4096];
                read(fd, buffer, sizeof(buffer)); // Clear inotify events

                // File has changed, read new lines [cite: 34]
                file.clear();
                file.seekg(last_pos);
                std::string line;
                while (std::getline(file, line)) {
                    if (!line.empty()) {
                        TerminalEvent event;
                        event.command = line;
                        event.timestamp = time(nullptr);
                        event_queue.push(event); // Push to lock-free queue [cite: 37]
                    }
                }
                last_pos = file.tellg();
            }
        }

        inotify_rm_watch(fd, wd);
        close(fd);
        close(epoll_fd);
    }

} // namespace sensors
