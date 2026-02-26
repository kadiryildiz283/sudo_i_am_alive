#pragma once
#include "core/SentinelConfig.hpp"
#include <chrono>
#include <string>

namespace core {
    class StateMachine {
    public:
        StateMachine(const SentinelDNA& initial_dna);
        ~StateMachine();

        bool should_intervene();
        void evaluate_tick(float delta_time, int event_count);
        SentinelDNA& get_mutable_state();

        void register_user_compliance(bool complied);
        bool is_waiting_for_compliance() const;

        // RASYONEL EKLENTİLER: Spontanlık ve DNA Sürekliliği
        bool should_spontaneously_interact() const;
        void reset_boredom();
        void save_dna_state(const std::string& filepath) const; // Diske JSON formatında yazar

    private:
        SentinelDNA current_params;
        std::chrono::steady_clock::time_point last_active_time;
        bool waiting_for_compliance_check;
    };
}
