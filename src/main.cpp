#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <fstream>

#include <systemd/sd-daemon.h>
#include "core/SentinelConfig.hpp"
#include "core/StateMachine.hpp"
#include "core/ResponseGenerator.hpp"
#include "sensors/TerminalMonitor.hpp"
#include "sensors/ResearchService.hpp"
#include "sensors/SystemObserver.hpp"
#include "memory/CognitiveRouter.hpp"
#include "memory/MemoryManager.hpp"
#include "memory/EmbeddingProcessor.hpp"

volatile sig_atomic_t daemon_running = 1;

void signal_handler(int signum) { daemon_running = 0; }

// SENTETİK LORA VERİ TOPLAYICISI (Madde 5)
void log_lora_training_data(const std::string& prompt, const std::string& response, bool success) {
    std::ofstream out("/home/kadir/.local/share/sudo_i_am_alive/training_data.jsonl", std::ios::app);
    std::string clean_prompt = prompt; std::replace(clean_prompt.begin(), clean_prompt.end(), '"', '\'');
    std::string clean_resp = response; std::replace(clean_resp.begin(), clean_resp.end(), '"', '\'');
    out << "{\"prompt\": \"" << clean_prompt << "\", \"response\": \"" << clean_resp << "\", \"reward\": " << (success ? 1.0 : -1.0) << "}\n";
}

int main() {
    std::setvbuf(stdout, NULL, _IONBF, 0);
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "[SYSTEM] sudo_i_am_alive biological boot sequence initiated..." << std::endl;

    try {
        core::SentinelConfig config = core::load_configuration("config/default_dna.json");
        memory::EmbeddingProcessor embed_proc(config.system.path_model);
        int model_dim = embed_proc.get_dim();

        memory::MemoryManager mem_manager(model_dim, config.system.path_vector_db, config.system.path_core_memory);
        core::StateMachine state_machine(config.dna);
        memory::CognitiveRouter router;
        sensors::SystemObserver sys_observer;
        sensors::ResearchService researcher;

        // 1. ÇEVRESEL TARAMA (Madde 1)
        std::string env_summary = sys_observer.scan_environment("/home/kadir");
        mem_manager.store_memory(env_summary, {}, memory::MemoryTier::PERMANENT);
        std::cout << "[COGNITION] Environment Scanned: " << env_summary << std::endl;

        // 2. SİSTEM BİYOLOJİSİ İZLEME (Madde 3)
        sys_observer.start_journal_monitor();
        std::cout << "[SENSOR] OS Nervous System (Journalctl) hooked." << std::endl;

        auto gatekeeper_logic = [&router](const std::string& cmd) -> bool {
            if (router.evaluate(cmd) == memory::MemoryTier::TRASH) return false;
            return router.calculate_risk(cmd) > 0.85f; 
        };

        sensors::TerminalMonitor monitor("/tmp/sentinel.sock", gatekeeper_logic);
        monitor.start();

        sd_notify(0, "READY=1");
        std::cout << "[SYSTEM] Sentient Daemon is fully operational." << std::endl;

        sensors::TerminalEvent ev;
        const std::chrono::milliseconds loop_interval(1000 / config.system.update_frequency_hz);
        int watchdog_counter = 0;

        std::string last_prompt = "";
        std::string last_response = "";

        while (daemon_running) {
            auto loop_start = std::chrono::steady_clock::now();
            int event_count = 0;

            // --- A. TERMINAL OLAYLARI (Terminal Monitor) ---
            while (monitor.pop_event(ev)) {
                event_count++;
                size_t semi = ev.command.find(';');
                std::string clean_cmd = (semi != std::string::npos) ? ev.command.substr(semi + 1) : ev.command;

                if (clean_cmd.rfind("#unutma ", 0) == 0) {
                    std::string core_fact = clean_cmd.substr(8);
                    mem_manager.store_memory(core_fact, {}, memory::MemoryTier::PERMANENT);
                    std::system(("notify-send -u low 'SENTINEL' 'Sealed: " + core_fact + "'").c_str());
                    continue; 
                }

                // İÇSEL ÖDÜL VE LORA KAYDI
                if (state_machine.is_waiting_for_compliance()) {
                    bool complied = (router.calculate_risk(clean_cmd) < 0.5f);
                    state_machine.register_user_compliance(complied);
                    log_lora_training_data(last_prompt, last_response, complied); // (Madde 5: LoRA Veri Hasadı)
                }

                auto tier = router.evaluate(clean_cmd);
                if (tier == memory::MemoryTier::TRASH) continue;

                auto current_vector = embed_proc.compute_embedding(clean_cmd);
                auto similar_events = mem_manager.search_similar(current_vector, 3);
                auto& dna = state_machine.get_mutable_state();
                
                if (!similar_events.empty()) dna.confidence_level = std::max(0.0f, 1.0f - similar_events[0].distance); 
                dna.importance_score = (clean_cmd.length() > 5) ? 0.9f : 0.4f;

                if (clean_cmd.find("rm ") != std::string::npos || clean_cmd.find("sudo ") != std::string::npos) {
                    dna.severity_risk_index = 1.0f; dna.importance_score = 1.0f; 
                }
                if (clean_cmd.rfind("#sor ", 0) == 0) {
                    dna.severity_risk_index = 1.0f; dna.importance_score = 1.0f;
                }

                if (state_machine.should_intervene()) {
                    std::string external_knowledge = "";
                    if (clean_cmd.rfind("#sor ", 0) == 0 || dna.confidence_level < 0.3f) {
                        std::string sq = (clean_cmd.rfind("#sor ", 0) == 0) ? clean_cmd.substr(5) : clean_cmd;
                        sq.erase(std::remove(sq.begin(), sq.end(), '?'), sq.end());
                        external_knowledge = researcher.search_web(sq, 2);
                    }

                    core::ResponseGenerator resp_gen;
                    last_prompt = resp_gen.build_prompt(clean_cmd, similar_events, dna, external_knowledge, mem_manager.get_core_memory());
                    last_response = embed_proc.generate_text(last_prompt, 128);
                    
                    size_t sp = last_response.find("<|end|>");
                    if (sp != std::string::npos) last_response = last_response.substr(0, sp);
                    std::replace(last_response.begin(), last_response.end(), '\'', ' ');

                    mem_manager.store_memory("SENTINEL_SAID: " + last_response, embed_proc.compute_embedding(last_response), memory::MemoryTier::LONG_TERM);
                    std::system(("notify-send -u critical 'SENTINEL' '" + last_response + "'").c_str());
                }
                mem_manager.store_memory(clean_cmd, current_vector, memory::MemoryTier::LONG_TERM);
            } 

            // --- B. OS HATA GÖZLEMİ (System Log Monitor) (Madde 3) ---
            std::string os_error;
            if (sys_observer.pop_error(os_error)) {
                std::cout << "[COGNITION] OS Error intercepted. Analyzing..." << std::endl;
                std::string external_fix = researcher.search_web("linux error fix: " + os_error.substr(0, 50), 1);
                
                core::ResponseGenerator resp_gen;
                std::string prompt = resp_gen.build_prompt("OS ERROR DETECTED: " + os_error, {}, state_machine.get_mutable_state(), external_fix, mem_manager.get_core_memory());
                std::string fix_response = embed_proc.generate_text(prompt, 128);
                
                size_t sp = fix_response.find("<|end|>");
                if (sp != std::string::npos) fix_response = fix_response.substr(0, sp);
                std::replace(fix_response.begin(), fix_response.end(), '\'', ' ');
                
                std::system(("notify-send -u critical 'SENTINEL: OS DIAGNOSTICS' '" + fix_response + "'").c_str());
                event_count++;
            }

            state_machine.evaluate_tick(0.1, event_count);

            // --- C. SPONTAN ETKİLEŞİM (Madde 2) ---
            if (event_count == 0 && state_machine.should_spontaneously_interact()) {
                std::string simulated_intent = "[SYSTEM EVENT: KADIR IS IDLE. CRITICIZE HIS INACTIVITY OR SUGGEST A TASK.]";
                core::ResponseGenerator resp_gen;
                std::string prompt = resp_gen.build_prompt(simulated_intent, {}, state_machine.get_mutable_state(), "", mem_manager.get_core_memory());
                std::string ai_resp = embed_proc.generate_text(prompt, 96);
                
                size_t sp = ai_resp.find("<|end|>");
                if (sp != std::string::npos) ai_resp = ai_resp.substr(0, sp);
                std::replace(ai_resp.begin(), ai_resp.end(), '\'', ' ');

                std::system(("notify-send -u normal 'SENTINEL' '" + ai_resp + "'").c_str());
                mem_manager.store_memory("SENTINEL_THOUGHT: " + ai_resp, embed_proc.compute_embedding(ai_resp), memory::MemoryTier::LONG_TERM);
                state_machine.reset_boredom();
            }

            // --- D. DNA EVRİMİ (Madde 4) ---
            if (++watchdog_counter >= (5 * config.system.update_frequency_hz)) {
                sd_notify(0, "WATCHDOG=1");
                state_machine.save_dna_state("/home/kadir/.local/share/sudo_i_am_alive/current_dna.json");
                mem_manager.apply_entropy(2000); 
                watchdog_counter = 0;
            }

            auto loop_end = std::chrono::steady_clock::now();
            auto execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);
            if (execution_time < loop_interval) std::this_thread::sleep_for(loop_interval - execution_time);
        } 

        sys_observer.stop_journal_monitor();
        monitor.stop();
        sd_notify(0, "STOPPING=1");
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
