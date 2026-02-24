#include "memory/MemoryManager.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <faiss/IndexFlat.h>

namespace memory { // Mutlaka bu blok içinde olmalı

    MemoryManager::MemoryManager(int dimension) {
        auto inner_index = new faiss::IndexFlatL2(dimension); 
        index = new faiss::IndexIDMap(inner_index);
        std::cout << "[MEMORY] FAISS Index initialized with dimension: " << dimension << std::endl;
}   uint32_t MemoryManager::fast_hash(const std::string& data) {
        std::hash<std::string> hasher;
        return static_cast<uint32_t>(hasher(data));
    }

    uint64_t MemoryManager::generate_id(const std::string& payload) {
        uint64_t timestamp = static_cast<uint64_t>(std::time(nullptr));
        uint64_t hash_part = static_cast<uint64_t>(fast_hash(payload));
        return (timestamp << 32) | (hash_part & 0xFFFFFFFF);
    }

    void MemoryManager::store_short_term(const std::string& payload, const std::vector<float>& embedding) {
        uint64_t id = generate_id(payload);
        faiss::idx_t faiss_id = static_cast<faiss::idx_t>(id);
        index->add_with_ids(1, embedding.data(), &faiss_id);
    }

} // namespace memory
