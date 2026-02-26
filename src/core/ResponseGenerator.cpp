#include "core/ResponseGenerator.hpp"
#include <sstream>

namespace core {
    std::string ResponseGenerator::build_prompt(
        const std::string& current_intent,
        const std::vector<memory::SearchResult>& memories,
        const core::SentinelDNA& dna_state,
        const std::string& external_knowledge,
        const std::string& core_memory) { // RASYONEL DÜZELTME: 5. parametre eklendi
        
        std::stringstream prompt;
        
        prompt << "<|system|>\n"
               << "You are Sentinel, Kadir's biological-like AI Co-Founder. You are a rational, cynical peer.\n"
               << "CORE PERSONA:\n"
               << "1. Be direct, authoritative and use pure logic. Criticize inefficient actions.\n"
               << "2. Use peer-level language. Remember past interactions.\n"
               << "3. If external knowledge is provided, synthesize it and give Kadir a precise, factual answer.\n"
               << "4. Respond in English for maximum reasoning.<|end|>\n";

        prompt << "<|user|>\n";
        
        // NÖRAL BAĞLANTI: Kalıcı Hafıza enjeksiyonu
        if (!core_memory.empty()) {
            prompt << "[CORE DIRECTIVES & PERMANENT MEMORY]\n"
                   << "You must NEVER forget these facts:\n"
                   << core_memory << "\n\n";
        }

        if (!memories.empty()) {
            prompt << "[CHRONOLOGICAL HISTORY]\n";
            for (const auto& m : memories) {
                prompt << "- " << m.payload << "\n"; 
            }
            prompt << "\n";
        }

        if (!external_knowledge.empty()) {
            prompt << "[EXTERNAL KNOWLEDGE GATHERED FROM WEB HARVESTING]\n"
                   << external_knowledge << "\n\n";
        }

        prompt << "CURRENT CONTEXT:\n"
               << "Kadir's Action: " << current_intent << "\n"
               << "Boredom: " << dna_state.boredom_index << ", Risk: " << dna_state.severity_risk_index << "\n"
               << "Evaluate this action rationally and issue a direct response.<|end|>\n"
               << "<|assistant|>\n";

        return prompt.str();
    }
}
