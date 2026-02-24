#pragma once
#include <vector>
#include "core/SentinelConfig.hpp"

namespace core {

    class StateMachine {
    public:
        // Sentinel DNA parametrelerini yükleyen yapılandırıcı 
        StateMachine(const core::SentinelDNA& dna);
        
        // Parametreleri 10Hz frekansında günceller [cite: 130]
        void evaluate_tick(float delta_time, int event_count);
        
        // Müdahale Denklemi: (C_t * I_t * R_t) > O_t [cite: 169]
        bool should_intervene() const;
        
        // DNA durumuna değiştirilebilir (mutable) erişim sağlar
        core::SentinelDNA& get_mutable_state() { return current_params; }
        const core::SentinelDNA& get_state() const { return current_params; }

    private:
        core::SentinelDNA current_params;
        
        void update_boredom(int event_count); // B_t [cite: 144]
        void update_fatigue();                // F_t [cite: 171]
    };

} // namespace core
