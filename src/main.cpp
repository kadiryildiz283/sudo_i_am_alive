#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cstdlib> // std::system kullanımı için

#include <systemd/sd-daemon.h>
#include "core/SentinelConfig.hpp"
#include "core/StateMachine.hpp"
#include "core/ResponseGenerator.hpp"
#include "sensors/TerminalMonitor.hpp"
#include "memory/CognitiveRouter.hpp"
#include "memory/MemoryManager.hpp"
#include "memory/EmbeddingProcessor.hpp"

volatile sig_atomic_t daemon_running = 1;

void signal_handler(int signum) {
    daemon_running = 0;
}

int main() {
    // RATIONAL: Disable buffering for instant log visibility in journalctl
    std::setvbuf(stdout, NULL, _IONBF, 0);

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "[SYSTEM] sudo_i_am_alive daemon initializing..." << std::endl;

    try {
        // 1. Configuration Loading [cite: 64]
        core::SentinelConfig config = core::load_configuration("config/default_dna.json");
        std::cout << "[SYSTEM] DNA Configuration loaded successfully." << std::endl;

        // 2. Embedding Engine Initialization [cite: 188]
        memory::EmbeddingProcessor embed_proc(config.system.path_model);
        int model_dim = embed_proc.get_dim();
        std::cout << "[SYSTEM] Embedding engine ready. Dim: " << model_dim << std::endl;

        // 3. Memory & Logic Initialization [cite: 12, 16]
        memory::MemoryManager mem_manager(model_dim);
        core::StateMachine state_machine(config.dna);
        memory::CognitiveRouter router;

        // 4. Sensory Hooking [cite: 30]
        sensors::TerminalMonitor monitor("/home/kadir/.zsh_history");
        monitor.start();
        std::cout << "[SENSOR] Terminal observer attached to .zsh_history" << std::endl;

        sd_notify(0, "READY=1");
        std::cout << "[SYSTEM] Daemon is fully operational." << std::endl;

        sensors::TerminalEvent ev;
        const std::chrono::milliseconds loop_interval(1000 / config.system.update_frequency_hz);
        int watchdog_counter = 0;

        // --- MAIN DETERMINISTIC CONTROL LOOP  ---
        while (daemon_running) {
            auto loop_start = std::chrono::steady_clock::now();
            int event_count = 0;

            // Process all pending sensory events in the queue [cite: 37]
// Process all pending sensory events in the queue
            while (monitor.pop_event(ev)) {
                event_count++;
                
                size_t semi = ev.command.find(';');
                std::string clean_cmd = (semi != std::string::npos) ? ev.command.substr(semi + 1) : ev.command;

                // 1. RASYONEL FİLTRE: Çöp imhası (TRASH commands are ignored completely)
                auto tier = router.evaluate(clean_cmd);
                if (tier == memory::MemoryTier::TRASH) {
                    continue; // Döngüyü kır, log atma, vektörleştirme, unut.
                }

                auto current_vector = embed_proc.compute_embedding(clean_cmd);
                auto similar_events = mem_manager.search_similar(current_vector, 3);
                
                auto& dna = state_machine.get_mutable_state();
                
                // 2. DNA GÜNCELLEMESİ (Dinamik Router üzerinden)
                if (!similar_events.empty()) {
                    dna.confidence_level = std::max(0.0f, 1.0f - similar_events[0].distance); 
                } else {
                    dna.confidence_level = 0.5f; 
                }

                // İlkel hardcoded if blokları yerine CognitiveRouter matematiği kullanılıyor
                dna.importance_score = router.calculate_importance(clean_cmd);
                dna.severity_risk_index = router.calculate_risk(clean_cmd);

                // INTERVENTION EQUATION: (C_t * I_t * R_t) > 0.85 OR Risk > 0.9 (Acil Durum)
                if (state_machine.should_intervene()) {
                    std::cout << "[SENTINEL] Intervention Validated. Product: " 
                              << (dna.confidence_level * dna.importance_score * dna.severity_risk_index) 
                              << " | Action: " << clean_cmd << std::endl;
                    
                    core::ResponseGenerator resp_gen;
                    std::string reasoning_prompt = resp_gen.build_prompt(clean_cmd, similar_events, dna);

                    std::cout << "[SYSTEM] Generating proactive insight..." << std::endl;

                    std::string ai_response = embed_proc.generate_text(reasoning_prompt, 96); // Gevezelik sınırı biraz esnetildi
                    
                    // RASYONEL GİYOTİN
                    size_t stop_pos1 = ai_response.find("<|end|>");
                    if (stop_pos1 != std::string::npos) ai_response = ai_response.substr(0, stop_pos1);
                    size_t stop_pos2 = ai_response.find("Action executed");
                    if (stop_pos2 != std::string::npos) ai_response = ai_response.substr(0, stop_pos2);
                    size_t stop_pos3 = ai_response.find("<|user|>");
                    if (stop_pos3 != std::string::npos) ai_response = ai_response.substr(0, stop_pos3);

                    std::replace(ai_response.begin(), ai_response.end(), '\'', ' ');

                    std::string notify_cmd = "notify-send -u critical 'SENTINEL: AI Co-Founder' '" + ai_response + "'";
                    std::system(notify_cmd.c_str());
                    
                    std::cout << "[SENTINEL] Dispatched insight: " << ai_response << std::endl;
                }

                mem_manager.store_short_term(clean_cmd, current_vector);
                std::cout << "[ROUTER] Vectorized Intent: " << clean_cmd << std::endl;
                           
            } // END OF pop_event WHILE LOOP 
              // Sync state at 10Hz
            state_machine.evaluate_tick(0.1, event_count);

            // Systemd Watchdog Keep-alive
            if (++watchdog_counter >= (5 * config.system.update_frequency_hz)) {
                sd_notify(0, "WATCHDOG=1");
                watchdog_counter = 0;
            }

            auto loop_end = std::chrono::steady_clock::now();
            auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);
            if (execution_time < loop_interval) {
                std::this_thread::sleep_for(loop_interval - execution_time);
            }
        } // END OF daemon_running WHILE LOOP

        monitor.stop();
        sd_notify(0, "STOPPING=1");
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} // DOSYA BURADA BİTMELİ, ALTINDA BAŞKA PARANTEZ OLMAMALI
