#include "memory/CognitiveRouter.hpp"

namespace memory {

    CognitiveRouter::CognitiveRouter() {
        // Routine commands that offer low information value [cite: 70]
        noise_commands = {"ls", "cd", "pwd", "clear", "history", "exit"};
    }

    MemoryTier CognitiveRouter::evaluate(const std::string& command) {
        // Simple entropy check: Filter out common noise [cite: 70]
        // In the future, this will use vector similarity for precision [cite: 69]
        
        std::string base_cmd = command.substr(0, command.find(' '));
        
        if (noise_commands.find(base_cmd) != noise_commands.end()) {
            return MemoryTier::TRASH;
        }

        // Potential destructive or complex commands go higher
        if (command.find("rm -rf") != std::string::npos || command.find("sudo") != std::string::npos) {
            return MemoryTier::SHORT_TERM; // High severity/interest [cite: 156]
        }

        return MemoryTier::SHORT_TERM;
    }

} // namespace memory
