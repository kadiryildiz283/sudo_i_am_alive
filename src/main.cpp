#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <vector>

// Linux Systemd and Core headers
#include <systemd/sd-daemon.h>
#include "core/SentinelConfig.hpp"
#include "core/StateMachine.hpp"
#include "sensors/TerminalMonitor.hpp"
#include "memory/CognitiveRouter.hpp"
#include "memory/MemoryManager.hpp"
#include "memory/EmbeddingProcessor.hpp"

// Global atomic flag for graceful shutdown
volatile sig_atomic_t daemon_running = 1;

void signal_handler(int signum) {
    // Initiate graceful shutdown
    daemon_running = 0;
}

int main() {
    // Register signal handlers for systemd service management
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "[SYSTEM] sudo_i_am_alive daemon initializing..." << std::endl;

    try {
        // 1. Load DNA and System configuration
        core::SentinelConfig config = core::load_configuration("config/default_dna.json");
        std::cout << "[SYSTEM] DNA Configuration loaded successfully." << std::endl;

        // 2. Initialize Embedding Engine
        // Must be initialized before MemoryManager to obtain vector dimensions
        memory::EmbeddingProcessor embed_proc("models/phi3-mini.gguf");
        int model_dim = embed_proc.get_dim();
        std::cout << "[SYSTEM] Embedding engine ready. Dimension: " << model_dim << std::endl;

        // 3. Initialize Memory Manager with model-specific dimensions
        memory::MemoryManager mem_manager(model_dim);
        
        // 4. Initialize Logic and Routing components
        core::StateMachine state_machine(config.dna);
        memory::CognitiveRouter router;
        std::cout << "[SYSTEM] Cognitive State Machine and Router initialized." << std::endl;

        // 5. Start Sensory Mesh (Terminal Monitor)
        sensors::TerminalMonitor monitor("/home/kadir/.zsh_history");
        monitor.start();
        std::cout << "[SENSOR] Terminal observer attached to .zsh_history" << std::endl;

        // 6. Notify Systemd that the service is READY
        sd_notify(0, "READY=1");
        std::cout << "[SYSTEM] Daemon is fully operational." << std::endl;

        sensors::TerminalEvent ev;
        const std::chrono::milliseconds loop_interval(1000 / config.system.update_frequency_hz);
        int watchdog_counter = 0;

        // --- MAIN DETERMINISTIC CONTROL LOOP ---
        while (daemon_running) {
            auto loop_start = std::chrono::steady_clock::now();

            // SENSOR INGESTION & PROCESSING
            int event_count = 0;
            while (monitor.pop_event(ev)) {
                event_count++;
                
                // COGNITIVE ROUTING: Filter noise and identify critical events
                auto tier = router.evaluate(ev.command);
                
                if (tier == memory::MemoryTier::TRASH) {
                    continue; // Discard non-essential commands (ls, cd, etc.)
                }

                // VECTORIZATION & SEMANTIC STORAGE
                // Convert raw command text to a dense vector
                auto vector = embed_proc.compute_embedding(ev.command);
                
                // Store in FAISS database using the 64-bit Timestamp+Hash ID schema
                mem_manager.store_short_term(ev.command, vector);

                std::cout << "[ROUTER] Important Event: " << ev.command 
                          << " -> Vectorized & Stored in Short-Term Memory" << std::endl;
            }

            // STATE CALCULATION: Update internal DNA parameters
            state_machine.evaluate_tick(0.1, event_count);

            // SYSTEMD WATCHDOG: Send keep-alive heartbeats
            if (++watchdog_counter >= (5 * config.system.update_frequency_hz)) {
                sd_notify(0, "WATCHDOG=1");
                watchdog_counter = 0;
            }

            // DYNAMIC SLEEP: Maintain consistent execution frequency
            auto loop_end = std::chrono::steady_clock::now();
            auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);
            if (execution_time < loop_interval) {
                std::this_thread::sleep_for(loop_interval - execution_time);
            }
        } 

        // --- SHUTDOWN SEQUENCE ---
        monitor.stop();
        sd_notify(0, "STOPPING=1");
        std::cout << "[SYSTEM] All sensors detached. Terminated securely." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        sd_notify(0, "STATUS=Fatal error. Shutting down.");
        return 1;
    }

    return 0;
}
