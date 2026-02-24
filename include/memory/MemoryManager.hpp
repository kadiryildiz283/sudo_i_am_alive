#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <faiss/IndexFlat.h>
#include <faiss/IndexIDMap.h>

namespace memory {
    // ÇAKILMAYI ÖNLEMEK İÇİN ENUM BURADAN KALDIRILDI

    struct SearchResult {
        long id;
        float distance;
        std::string payload;
    };

    class MemoryManager {
    public:
        MemoryManager(int dim);
        ~MemoryManager();

        void store_short_term(const std::string& text, const std::vector<float>& embedding);
        std::vector<SearchResult> search_similar(const std::vector<float>& query_embedding, int k = 3);

    private:
        int dimension;
        long next_id;
        faiss::IndexFlatL2* base_index;
        faiss::IndexIDMap* id_map;
        std::unordered_map<long, std::string> memory_vault; 
    };
}
