#pragma once

#include <string>
#include <cstdint>

namespace core {

    struct SystemSettings {
        std::string daemon_name;
        int32_t update_frequency_hz;
        int32_t watchdog_timeout_sec;
        std::string path_core_memory;
        std::string path_vector_db;
        std::string path_model;
    };

    struct LLMParams {
        int32_t context_size;
        int32_t thread_count;
        int32_t gpu_layers;
        float temperature;
        float kv_cache_shift_threshold;
    };

    struct SentinelDNA {
        float confidence_level;
        float self_critique_score;
        float boredom_index;
        float memory_decay_rate;
        float importance_score;
        float severity_risk_index;
        int32_t chain_of_thought_depth;
        float override_threshold;
        float system_fatigue_level;
        float spontaneous_insight_probability;
        float digital_sleep_cycle_depth;
        float devil_advocate_trigger;
    };

    struct SentinelConfig {
        SystemSettings system;
        LLMParams llm;
        SentinelDNA dna;
    };

    // Parses JSON and returns the memory-safe struct
    SentinelConfig load_configuration(const std::string& config_filepath);

} // namespace core
