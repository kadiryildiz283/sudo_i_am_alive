#pragma once

#include "core/SentinelConfig.hpp"
#include <algorithm>

namespace core {

    class StateMachine {
    private:
        SentinelDNA current_state;

    public:
        explicit StateMachine(const SentinelDNA& initial_dna) : current_state(initial_dna) {}

        // Evaluates internal metrics based on sensory events.
        // Runs inside the main event loop.
        void evaluate_tick(double delta_time, int recent_events_count) {
            // Increase boredom if Kadir is inactive
            if (recent_events_count == 0) {
                current_state.boredom_index = std::min(1.0f, current_state.boredom_index + static_cast<float>(0.005 * delta_time));
            } else {
                current_state.boredom_index = 0.0f; // Reset boredom on activity
            }

            // Sleep cycle calculation based on prolonged boredom
            if (current_state.boredom_index > 0.9f) {
                current_state.digital_sleep_cycle_depth = std::min(1.0f, current_state.digital_sleep_cycle_depth + static_cast<float>(0.01 * delta_time));
            } else {
                current_state.digital_sleep_cycle_depth = std::max(0.0f, current_state.digital_sleep_cycle_depth - static_cast<float>(0.05 * delta_time));
            }
        }

        const SentinelDNA& get_current_state() const {
            return current_state;
        }
    };

} // namespace core
