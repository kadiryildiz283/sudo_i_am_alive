#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <faiss/IndexIDMap.h>
#include "memory/CognitiveRouter.hpp"

namespace memory {

    class MemoryManager {
      public:
        MemoryManager(int dimension = 3072);
        uint64_t generate_id(const std::string& payload);
        void store_short_term(const std::string& payload, const std::vector<float>& embedding);

    private:
        faiss::IndexIDMap* index;
        uint32_t fast_hash(const std::string& data);
    };

} // namespace memory
