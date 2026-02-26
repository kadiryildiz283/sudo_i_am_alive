#pragma once
#include <string>
#include "memory/MemoryManager.hpp" // RASYONEL BAĞLANTI: MemoryTier enum'ı artık sadece buradan okunacak

namespace memory {
    class CognitiveRouter {
    public:
        CognitiveRouter();
        ~CognitiveRouter();

        MemoryTier evaluate(const std::string& command);
        float calculate_importance(const std::string& command);
        float calculate_risk(const std::string& command);
    };
}
