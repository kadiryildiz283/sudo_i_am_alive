#include "sensors/TerminalMonitor.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <algorithm>

namespace sensors {

    TerminalMonitor::TerminalMonitor(const std::string& socket_path, std::function<bool(const std::string&)> gatekeeper_cb) 
        : sock_path(socket_path), running(false), gatekeeper(gatekeeper_cb), server_fd(-1) {}

    TerminalMonitor::~TerminalMonitor() { stop(); }

    void TerminalMonitor::start() {
        if (running) return;
        running = true;
        monitor_thread = std::thread(&TerminalMonitor::run_loop, this);
    }

    void TerminalMonitor::stop() {
        running = false;
        if (server_fd != -1) {
            close(server_fd);
            unlink(sock_path.c_str());
            server_fd = -1;
        }
        if (monitor_thread.joinable()) {
            monitor_thread.join();
        }
    }

    // RASYONEL KUYRUK: Thread-safe veri aktarımı
    bool TerminalMonitor::pop_event(TerminalEvent& event) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (event_queue.empty()) {
            return false;
        }
        event = event_queue.front();
        event_queue.pop();
        return true;
    }

    void TerminalMonitor::run_loop() {
        server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cerr << "[SENSOR] FATAL: Unix socket creation failed." << std::endl;
            return;
        }

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, sock_path.c_str(), sizeof(addr.sun_path) - 1);
        
        unlink(sock_path.c_str()); 
        if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "[SENSOR] FATAL: Socket bind failed on " << sock_path << std::endl;
            return;
        }
        
        listen(server_fd, 10);
        std::cout << "[SENSOR] Pre-exec gatekeeper active on " << sock_path << std::endl;

        struct pollfd pfd;
        pfd.fd = server_fd;
        pfd.events = POLLIN;

        while (running) {
            // 100ms timeout to allow graceful shutdown checking
            int ret = poll(&pfd, 1, 100); 
            
            if (ret > 0 && (pfd.revents & POLLIN)) {
                int client_fd = accept(server_fd, nullptr, nullptr);
                if (client_fd >= 0) {
                    char buf[2048] = {0};
                    int bytes_read = read(client_fd, buf, sizeof(buf) - 1);
                    
                    if (bytes_read > 0) {
                        std::string clean_cmd(buf);
                        clean_cmd.erase(std::remove(clean_cmd.begin(), clean_cmd.end(), '\n'), clean_cmd.end());

                        // 1. GATEKEEPER MÜDAHALESİ (Anlık Risk Analizi)
                        bool should_block = gatekeeper(clean_cmd);

                        // 2. YANITI TERMİNALE DÖNDÜR
                        std::string reply = should_block ? "BLOCK" : "OK";
                        write(client_fd, reply.c_str(), reply.length());

                        // 3. OLAYI ANA BİLİNÇ İÇİN KUYRUĞA AL
                        TerminalEvent event;
                        event.command = clean_cmd;
                        event.timestamp = std::time(nullptr);
                        
                        {
                            std::lock_guard<std::mutex> lock(queue_mutex);
                            event_queue.push(event);
                        }
                    }
                    close(client_fd);
                }
            }
        }
    }

} // namespace sensors
