#include "sensors/ResearchService.hpp"
#include <array>
#include <memory>
#include <iostream>
#include <stdexcept>

namespace sensors {

    ResearchService::ResearchService() {}
    ResearchService::~ResearchService() {}

    std::string ResearchService::execute_shell_command(const std::string& cmd) {
        std::array<char, 256> buffer;
        std::string result;
        // stdout ve stderr'i yakalayarak güvenli popen çağrısı
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen((cmd + " 2>&1").c_str(), "r"), pclose);
        if (!pipe) {
            return "[RESEARCH ERROR] Failed to open process pipe.";
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    std::string ResearchService::search_web(const std::string& query, int limit) {
        std::cout << "[RESEARCH] Executing external query: " << query << std::endl;
        
        // ddgr komutu: limitsiz, renksiz, promptsuz salt metin araması
        std::string cmd = "ddgr --num=" + std::to_string(limit) + " --noprompt --colors none \"" + query + "\"";
        
        std::string raw_output = execute_shell_command(cmd);
        
        if (raw_output.empty() || raw_output.find("No results") != std::string::npos) {
            return "No external information found for: " + query;
        }

        return "[EXTERNAL RESEARCH RESULTS]\n" + raw_output;
    }

    std::string ResearchService::read_webpage(const std::string& url) {
        std::cout << "[RESEARCH] Harvesting webpage: " << url << std::endl;
        
        // w3m komutu: sayfayı render etmeden salt text olarak dump et
        std::string cmd = "w3m -dump \"" + url + "\" | head -n 100"; // İlk 100 satırı alarak LLM contextini koru
        
        std::string content = execute_shell_command(cmd);
        if (content.empty()) {
            return "[RESEARCH ERROR] Webpage could not be accessed.";
        }
        
        return content;
    }

} // namespace sensors
