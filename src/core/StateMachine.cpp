#include "core/StateMachine.hpp"
#include <algorithm>
#include <iostream>

namespace core {
    StateMachine::StateMachine(const SentinelDNA& initial_dna) 
        : current_params(initial_dna), waiting_for_compliance_check(false) {
        last_active_time = std::chrono::steady_clock::now();
    }

    StateMachine::~StateMachine() {}

    SentinelDNA& StateMachine::get_mutable_state() {
        return current_params;
    }

    bool StateMachine::is_waiting_for_compliance() const {
        return waiting_for_compliance_check;
    }

    bool StateMachine::should_intervene() {
        // INTERVENTION EQUATION: (C_t * I_t * R_t) > 0.85
        float product = current_params.confidence_level * current_params.importance_score * current_params.severity_risk_index;
        
        if (product > 0.85f || current_params.severity_risk_index > 0.9f) {
            waiting_for_compliance_check = true; // Müdahale edildi, Kadir'in tepkisi bekleniyor
            return true;
        }
        return false;
    }

    void StateMachine::register_user_compliance(bool complied) {
        if (!waiting_for_compliance_check) return;

        if (complied) {
            // INTERNAL REWARD: Kadir complied with the warning.
            current_params.confidence_level = std::min(1.0f, current_params.confidence_level + 0.1f);
            current_params.system_fatigue_level = std::max(0.0f, current_params.system_fatigue_level - 0.05f);
            std::cout << "[COGNITION] Internal Reward. User complied. Confidence: " << current_params.confidence_level << std::endl;
        } else {
            // PENALTY: Kadir ignored the warning.
            current_params.confidence_level = std::max(0.1f, current_params.confidence_level - 0.1f);
            current_params.system_fatigue_level = std::min(1.0f, current_params.system_fatigue_level + 0.15f);
            std::cout << "[COGNITION] User defied intervention. Fatigue: " << current_params.system_fatigue_level << std::endl;
        }
        waiting_for_compliance_check = false;
    }

    bool StateMachine::should_spontaneously_interact() const {
        // EQUATION: Sıkıntı > 0.8 VE rastgele sayı < %5 (spontaneous_insight_probability)
        if (current_params.boredom_index > 0.8f) {
            float chance = (rand() % 100) / 100.0f;
            if (chance < current_params.spontaneous_insight_probability) {
                return true;
            }
        }
        return false;
    }

    void StateMachine::reset_boredom() {
        current_params.boredom_index = 0.0f;
        last_active_time = std::chrono::steady_clock::now(); // Sayacı sıfırla
    }

    void StateMachine::save_dna_state(const std::string& filepath) const {
        std::ofstream out(filepath);
        if (!out.is_open()) return;

        // Dependency gerektirmeyen, salt mantıkla JSON inşası
        out << "{\n";
        out << "    \"confidence_level\": " << current_params.confidence_level << ",\n";
        out << "    \"self_critique_score\": " << current_params.self_critique_score << ",\n";
        out << "    \"boredom_index\": " << current_params.boredom_index << ",\n";
        out << "    \"memory_decay_rate\": " << current_params.memory_decay_rate << ",\n";
        out << "    \"importance_score\": " << current_params.importance_score << ",\n";
        out << "    \"severity_risk_index\": " << current_params.severity_risk_index << ",\n";
        out << "    \"chain_of_thought_depth\": " << current_params.chain_of_thought_depth << ",\n";
        out << "    \"override_threshold\": " << current_params.override_threshold << ",\n";
        out << "    \"system_fatigue_level\": " << current_params.system_fatigue_level << ",\n";
        out << "    \"spontaneous_insight_probability\": " << current_params.spontaneous_insight_probability << ",\n";
        out << "    \"digital_sleep_cycle_depth\": " << current_params.digital_sleep_cycle_depth << ",\n";
        out << "    \"devil_advocate_trigger\": " << current_params.devil_advocate_trigger << "\n";
        out << "}\n";
    }

    void StateMachine::evaluate_tick(float delta_time, int event_count) {
        auto now = std::chrono::steady_clock::now();
        
        if (event_count > 0) {
            last_active_time = now;
            current_params.boredom_index = std::max(0.0f, current_params.boredom_index - 0.1f); 
        } else {
            auto idle_duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_active_time).count();
            if (idle_duration > 30) { 
                current_params.boredom_index = std::min(1.0f, current_params.boredom_index + (0.05f * delta_time));
            }
        }

        // Natural rest (Decreasing fatigue over time)
        current_params.system_fatigue_level = std::max(0.0f, current_params.system_fatigue_level - (0.01f * delta_time));
    }
}
