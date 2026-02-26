#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <faiss/IndexFlat.h>
#include <faiss/IndexIDMap.h>
#include <mutex>

namespace memory {
    enum class MemoryTier { TRASH, SHORT_TERM, LONG_TERM, PERMANENT };

    struct SearchResult {
        long id;
        float distance;
        std::string payload;
    };

    class MemoryManager {
    public:
        // RASYONEL DEĞİŞİKLİK: Disk yolları (paths) parametre olarak eklendi
        MemoryManager(int dim, const std::string& vector_db_path, const std::string& core_mem_path);
        ~MemoryManager();

        void store_memory(const std::string& text, const std::vector<float>& embedding, MemoryTier tier);
        std::vector<SearchResult> search_similar(const std::vector<float>& query_embedding, int k = 3);
        void apply_entropy(int max_capacity = 2000);
        // Kalıcı (Permanent) hafızayı tek parça halinde getirir
        std::string get_core_memory() const;

        // Daemon kapanırken/açılırken diske yazma/okuma
        void save_to_disk();
        void load_from_disk();

    private:
        int dimension;
        long next_id;
        long oldest_id;
        std::string db_path;
        std::string core_path;

        faiss::IndexFlatL2* base_index;
        faiss::IndexIDMap* id_map;
        std::unordered_map<long, std::string> memory_vault; 
        std::vector<std::string> core_memory_vault; // Asla silinmeyenler

        std::mutex mem_mutex;
    };
}
