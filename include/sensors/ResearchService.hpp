#pragma once
#include <string>
#include <vector>

namespace sensors {
    struct SearchResult {
        std::string title;
        std::string url;
        std::string snippet;
    };

    class ResearchService {
    public:
        ResearchService();
        ~ResearchService();

        // ddgr kullanarak terminal üzerinden DuckDuckGo araması yapar
        std::string search_web(const std::string& query, int limit = 3);
        
        // w3m kullanarak bir web sayfasının salt metnini (dump) çeker
        std::string read_webpage(const std::string& url);

    private:
        // Linux shell komutlarını çalıştırıp çıktılarını okuyan çekirdek fonksiyon
        std::string execute_shell_command(const std::string& cmd);
    };
}
