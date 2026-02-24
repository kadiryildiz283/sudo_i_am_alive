#include "core/SentinelConfig.hpp"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace core {

    SentinelConfig load_configuration(const std::string& config_filepath) {
        std::ifstream file_stream(config_filepath);
        if (!file_stream.is_open()) {
            throw std::runtime_error("CRITICAL: Failed to open configuration file: " + config_filepath);
        }

        json j;
        try {
            file_stream >> j;
        } catch (const json::parse_error& e) {
            throw std::runtime_error(std::string("CRITICAL: JSON parse error: ") + e.what());
        }

        SentinelConfig config;

        // Map System Settings
        config.system.daemon_name          = j["system"]["daemon_name"];
        config.system.update_frequency_hz  = j["system"]["update_frequency_hz"];
        config.system.watchdog_timeout_sec = j["system"]["watchdog_timeout_sec"];
        config.system.path_core_memory     = j["system"]["path_core_memory"];
        config.system.path_vector_db       = j["system"]["path_vector_db"];
        config.system.path_model           = j["system"]["path_model"];

        // Map LLM Parameters
        config.llm.context_size             = j["llm"]["context_size"];
        config.llm.thread_count             = j["llm"]["thread_count"];
        config.llm.gpu_layers               = j["llm"]["gpu_layers"];
        config.llm.temperature              = j["llm"]["temperature"];
        config.llm.kv_cache_shift_threshold = j["llm"]["kv_cache_shift_threshold"];

        // Map DNA State
        config.dna.confidence_level                = j["dna"]["confidence_level"];
        config.dna.self_critique_score             = j["dna"]["self_critique_score"];
        config.dna.boredom_index                   = j["dna"]["boredom_index"];
        config.dna.memory_decay_rate               = j["dna"]["memory_decay_rate"];
        config.dna.importance_score                = j["dna"]["importance_score"];
        config.dna.severity_risk_index             = j["dna"]["severity_risk_index"];
        config.dna.chain_of_thought_depth          = j["dna"]["chain_of_thought_depth"];
        config.dna.override_threshold              = j["dna"]["override_threshold"];
        config.dna.system_fatigue_level            = j["dna"]["system_fatigue_level"];
        config.dna.spontaneous_insight_probability = j["dna"]["spontaneous_insight_probability"];
        config.dna.digital_sleep_cycle_depth       = j["dna"]["digital_sleep_cycle_depth"];
        config.dna.devil_advocate_trigger          = j["dna"]["devil_advocate_trigger"];

        return config;
    }

} // namespace core
