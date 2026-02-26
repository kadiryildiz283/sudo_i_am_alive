#pragma once
#include <string>
#include <vector>
#include "memory/MemoryManager.hpp"
#include "core/SentinelConfig.hpp"

namespace core {
    class ResponseGenerator {
    public:
        ResponseGenerator() = default;
        ~ResponseGenerator() = default;

        // RASYONEL EKLENTİ: external_knowledge (Dış Bilgi) parametresi eklendi
std::string build_prompt(
            const std::string& current_intent,
            const std::vector<memory::SearchResult>& memories,
            const core::SentinelDNA& dna_state,
            const std::string& external_knowledge = "",
            const std::string& core_memory = "" // RASYONEL EKLENTİ
        ); 
    };
}
