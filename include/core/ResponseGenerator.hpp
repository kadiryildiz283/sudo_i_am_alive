#pragma once
#include <string>
#include <vector>
#include "core/SentinelConfig.hpp"
#include "memory/MemoryManager.hpp"

namespace core {

    class ResponseGenerator {
    public:
        // Constructs the strictly logical prompt for Phi-3 inference
        std::string build_prompt(
            const std::string& current_intent,
            const std::vector<memory::SearchResult>& memories,
            const core::SentinelDNA& dna_state
        );
    };

} // namespace core
