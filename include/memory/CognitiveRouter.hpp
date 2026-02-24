#pragma once
#include <string>

namespace memory {
    // RASYONEL UYARI: MemoryTier enum sadece burada tanımlı olmalıdır (Çoklu tanımlama hatasını önlemek için)
    enum class MemoryTier { TRASH, SHORT_TERM, LONG_TERM };

    class CognitiveRouter {
    public:
        CognitiveRouter();
        ~CognitiveRouter();

        // Çöp analizi
        MemoryTier evaluate(const std::string& command);
        
        // Dinamik parametre hesaplayıcıları
        float calculate_importance(const std::string& command);
        float calculate_risk(const std::string& command);
    };
}
