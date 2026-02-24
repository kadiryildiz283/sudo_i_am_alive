#include "memory/MemoryManager.hpp"
#include <iostream>

namespace memory {
    MemoryManager::MemoryManager(int dim) : dimension(dim), next_id(0) {
        base_index = new faiss::IndexFlatL2(dimension);
        id_map = new faiss::IndexIDMap(base_index);
        std::cout << "[MEMORY] FAISS Index initialized with dimension: " << dimension << std::endl;
    }

    MemoryManager::~MemoryManager() {
        delete id_map;
        delete base_index;
    }

    void MemoryManager::store_short_term(const std::string& text, const std::vector<float>& embedding) {
        if (embedding.size() != dimension) return;
        
        // Add to FAISS Vector DB
        id_map->add_with_ids(1, embedding.data(), &next_id);
        
        // Map ID to String payload
        memory_vault[next_id] = text; 
        next_id++;
    }

    std::vector<SearchResult> MemoryManager::search_similar(const std::vector<float>& query_embedding, int k) {
        if (id_map->ntotal == 0) return {};

        int valid_k = std::min(k, (int)id_map->ntotal);
        std::vector<float> distances(valid_k);
        std::vector<faiss::idx_t> indices(valid_k);

        id_map->search(1, query_embedding.data(), valid_k, distances.data(), indices.data());

        std::vector<SearchResult> results;
        for (int i = 0; i < valid_k; i++) {
            if (indices[i] >= 0) {
                SearchResult res;
                res.id = indices[i];
                res.distance = distances[i];
                res.payload = memory_vault[indices[i]]; // Retrieve original text
                results.push_back(res);
            }
        }
        return results;
    }
}
