#include "core/StateMachine.hpp"
#include <algorithm>

namespace core {

    StateMachine::StateMachine(const core::SentinelDNA& dna) : current_params(dna) {}

    void StateMachine::evaluate_tick(float delta_time, int event_count) {
        if (event_count == 0) {
            current_params.boredom_index = std::min(1.0f, current_params.boredom_index + 0.01f);
        } else {
            current_params.boredom_index = std::max(0.0f, current_params.boredom_index - 0.2f);
        }
        
        current_params.severity_risk_index = std::max(0.0f, current_params.severity_risk_index - (delta_time * 0.1f));
    }

    bool StateMachine::should_intervene() const {
        // RASYONEL KURAL: Eğer risk indeksi ekstrem seviyedeyse, Sentinel matematiği ezer ve anında müdahale eder.
        if (current_params.severity_risk_index > 0.9f) {
            return true;
        }

        // Normal rutin kontroller için çarpım denklemi
        float product = current_params.confidence_level * current_params.importance_score * current_params.severity_risk_index;
                      
        return product > current_params.override_threshold;
    }

} // namespace core
