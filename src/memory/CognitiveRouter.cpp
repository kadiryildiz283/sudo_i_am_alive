#include "memory/CognitiveRouter.hpp"
#include <vector>
#include <algorithm>

namespace memory {
    CognitiveRouter::CognitiveRouter() {}
    CognitiveRouter::~CognitiveRouter() {}

    MemoryTier CognitiveRouter::evaluate(const std::string& command) {
        // RASYONEL FİLTRE: Sistem durumunu değiştirmeyen önemsiz komutlar
        std::vector<std::string> trash_cmds = {
            "ls", "pwd", "cd", "echo", "cat", "clear", "whoami", "history", "top", "htop", "ping"
        };
        
        // Komutun ana gövdesini al (örn: "ls -la" -> "ls")
        std::string base_cmd = command.substr(0, command.find(' '));
        
        if (std::find(trash_cmds.begin(), trash_cmds.end(), base_cmd) != trash_cmds.end()) {
            return MemoryTier::TRASH;
        }
        return MemoryTier::SHORT_TERM; // Saklanmaya ve vektörleştirilmeye değer
    }

    float CognitiveRouter::calculate_importance(const std::string& command) {
        // Komut uzunluğu ve karmaşıklığına göre önem derecesi
        if (command.length() > 20) return 0.9f;
        if (command.length() > 10) return 0.6f;
        return 0.3f;
    }

    float CognitiveRouter::calculate_risk(const std::string& command) {
        float risk = 0.1f; // Temel risk
        
        // Deterministik risk çarpanları
        if (command.find("sudo ") != std::string::npos) risk += 0.4f;
        if (command.find("rm ") != std::string::npos) risk += 0.5f;
        if (command.find("chmod ") != std::string::npos) risk += 0.3f;
        if (command.find("chown ") != std::string::npos) risk += 0.3f;
        if (command.find("dd ") != std::string::npos) risk += 0.8f;   // Disk yazma
        if (command.find("mkfs") != std::string::npos) risk += 0.8f;  // Formatlama
        if (command.find("pacman -R") != std::string::npos) risk += 0.5f; // CachyOS Paket silme
        
        return std::min(1.0f, risk); // Risk 1.0'ı geçemez
    }
}
