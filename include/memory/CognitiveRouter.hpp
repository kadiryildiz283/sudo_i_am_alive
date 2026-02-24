#pragma once

#include <string>
#include <vector>
#include <set>
#include "core/SentinelConfig.hpp"

namespace memory {

    enum class MemoryTier {
        CORE,       // Immutable rules and identity [cite: 67]
        SHORT_TERM, // High entropy behavior to Vector DB [cite: 67]
        TRASH       // Low entropy noise to be discarded [cite: 67, 70]
    };

    class CognitiveRouter {
    private:
        std::set<std::string> noise_commands;

    public:
        CognitiveRouter();
        
        // Evaluates the entropy and relevance of a command [cite: 11]
        MemoryTier evaluate(const std::string& command);
    };

} // namespace memory
