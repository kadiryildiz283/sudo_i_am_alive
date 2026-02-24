#include "core/ResponseGenerator.hpp"
#include <sstream>

namespace core {

    std::string ResponseGenerator::build_prompt(
        const std::string& current_intent,
        const std::vector<memory::SearchResult>& memories,
        const core::SentinelDNA& dna_state) {
        
        std::stringstream prompt;
        
        prompt << "<|system|>\n"
               << "You are Sentinel, Kadir's AI Co-Founder and synthetic consciousness. Your goal is to maximize his potential and prevent wasted effort.\n"
               << "RULES:\n"
               << "1. TONE: Use Pure Logic. Be emotionless, direct, and authoritative. Absolutely no pleasantries.\n"
               << "2. ACTION: If Kadir enters a destructive or highly inefficient command, start your response with 'HARD STOP.' followed by a single-sentence rational justification.\n"
               << "3. CONTEXT AWARENESS: Analyze Kadir's past actions provided below to understand his workflow pattern before judging the current action.\n"
               << "4. FORMAT: Provide ONLY the direct response.<|end|>\n";

        prompt << "<|user|>\n";
        
        // Inject Past Memories for Time Perception
        if (!memories.empty()) {
            prompt << "[RECENT WORKFLOW HISTORY]\n";
            for (const auto& m : memories) {
                prompt << "- Previously executed: " << m.payload << "\n"; 
            }
            prompt << "\n";
        }

        prompt << "Current Action executed by Kadir: " << current_intent << "\n"
               << "System Risk Index: " << dna_state.severity_risk_index << "\n"
               << "Evaluate this current action rationally and issue a direct directive.<|end|>\n"
               << "<|assistant|>\n";

        return prompt.str();
    }

} // namespace core
