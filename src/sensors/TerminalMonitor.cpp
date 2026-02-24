#include "sensors/TerminalMonitor.hpp"
#include <iostream>
#include <fstream>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>

namespace sensors {

    TerminalMonitor::TerminalMonitor(const std::string& path) 
        : history_path(path), running(false) {}

    TerminalMonitor::~TerminalMonitor() { stop(); }

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
        int fd = inotify_init1(IN_NONBLOCK);
        if (fd < 0) return;

        int wd = inotify_add_watch(fd, history_path.c_str(), IN_MODIFY | IN_ATTRIB);

        int epoll_fd = epoll_create1(0);
        struct epoll_event ev_cfg, events[1];
        ev_cfg.events = EPOLLIN | EPOLLET; 
        ev_cfg.data.fd = fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev_cfg);

        std::streampos last_pos = 0;
        {
            std::ifstream initial_file(history_path, std::ios::binary);
            if (initial_file.is_open()) {
                initial_file.seekg(0, std::ios::end);
                last_pos = initial_file.tellg();
            }
        }

        while (running) {
            int nfds = epoll_wait(epoll_fd, events, 1, 1000); 
            
            if (nfds > 0) {
                // RATIONAL: Properly parse inotify events to catch IN_IGNORED
                // Align buffer for struct inotify_event
                char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
                ssize_t len;
                bool needs_rewatch = false;

                while ((len = read(fd, buf, sizeof(buf))) > 0) {
                    const struct inotify_event *event;
                    for (char *ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
                        event = (const struct inotify_event *) ptr;
                        // Zsh replaced the file (atomic rename/delete)
                        if (event->mask & IN_IGNORED) {
                            needs_rewatch = true; 
                        }
                    }
                }

                if (needs_rewatch) {
                    wd = inotify_add_watch(fd, history_path.c_str(), IN_MODIFY | IN_ATTRIB);
                }

                std::ifstream file(history_path, std::ios::binary);
                if (!file.is_open()) continue;

                // Check for file truncation (history cleared)
                file.seekg(0, std::ios::end);
                if (file.tellg() < last_pos) last_pos = 0;

                file.seekg(last_pos);
                std::string line;
                
                while (std::getline(file, line)) {
                    if (!line.empty()) {
                        size_t semi = line.find(';');
                        std::string clean = (semi != std::string::npos) ? line.substr(semi + 1) : line;

                        TerminalEvent event;
                        event.command = clean;
                        event.timestamp = std::time(nullptr);
                        event_queue.push(event); 
                        
                        std::cout << "[SENSOR] Observed Intent: " << clean << std::endl;
                    }
                }
                
                // CRITICAL FIX: Clear EOF/FAIL flags before calling tellg()
                // Otherwise tellg() returns -1 and breaks the next read cycle.
                file.clear(); 
                last_pos = file.tellg();
                file.close();
            }
        }

        inotify_rm_watch(fd, wd);
        close(fd);
        close(epoll_fd);
    }

} // namespace sensors
