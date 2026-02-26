#include "memory/MemoryManager.hpp"
#include <iostream>
#include <fstream>
#include <faiss/index_io.h>

namespace memory {
    MemoryManager::MemoryManager(int dim, const std::string& vector_db_path, const std::string& core_mem_path) 
        : dimension(dim), next_id(0), oldest_id(0), db_path(vector_db_path), core_path(core_mem_path) { // RASYONEL EKLENTİ: oldest_id(0) buraya eklendi
        
        base_index = new faiss::IndexFlatL2(dimension);
        id_map = new faiss::IndexIDMap(base_index);
        
        load_from_disk(); 
        std::cout << "[MEMORY] Architecture initialized. Total Long-Term memories: " << id_map->ntotal << std::endl;
    }

    MemoryManager::~MemoryManager() {
        save_to_disk(); // Daemon kapanırken anıları diske mühürle
        delete id_map;
        delete base_index;
    }

    void MemoryManager::store_memory(const std::string& text, const std::vector<float>& embedding, MemoryTier tier) {
        std::lock_guard<std::mutex> lock(mem_mutex);
        
        if (tier == MemoryTier::TRASH) return;
        
        if (tier == MemoryTier::PERMANENT) {
            core_memory_vault.push_back(text);
            // Kalıcı hafızayı anında text dosyasına yaz
            std::ofstream out(core_path, std::ios::app);
            out << text << "\n";
            return;
        }

        // Uzun ve Kısa vadeli hafıza (FAISS)
        if (embedding.size() == dimension) {
            id_map->add_with_ids(1, embedding.data(), &next_id);
            memory_vault[next_id] = text; 
            next_id++;
        }
    }

    std::vector<SearchResult> MemoryManager::search_similar(const std::vector<float>& query_embedding, int k) {
        std::lock_guard<std::mutex> lock(mem_mutex);
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
                res.payload = memory_vault[indices[i]];
                results.push_back(res);
            }
        }
        return results;
    }

    std::string MemoryManager::get_core_memory() const {
        std::string core_dump = "";
        for (const auto& mem : core_memory_vault) {
            core_dump += "- " + mem + "\n";
        }
        return core_dump;
    }

    void MemoryManager::save_to_disk() {
        std::lock_guard<std::mutex> lock(mem_mutex);
        
        // FAISS indexini kaydet
        faiss::write_index(id_map, (db_path + ".index").c_str());
        
        // Metin kasasını Binary (İkili) formatta hızlıca kaydet
        std::ofstream out(db_path + ".vault", std::ios::binary);
        for (const auto& pair : memory_vault) {
            out.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
            size_t len = pair.second.size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(pair.second.c_str(), len);
        }
        std::cout << "[MEMORY] Synced to disk successfully." << std::endl;
    }

    void MemoryManager::apply_entropy(int max_capacity) {
        std::lock_guard<std::mutex> lock(mem_mutex);
        
        // Execute pruning only if capacity exceeds threshold
        if (id_map->ntotal <= max_capacity) return;

        // Discard the oldest 20% to retain operational context while freeing memory
        int num_to_remove = id_map->ntotal - (max_capacity * 0.8); 
        
        // FAISS ID selector removes vectors in the specified sequential range
        faiss::IDSelectorRange sel(oldest_id, oldest_id + num_to_remove);
        id_map->remove_ids(sel);

        // Synchronize string payload vault by erasing corresponding text entries
        for (long i = oldest_id; i < oldest_id + num_to_remove; i++) {
            memory_vault.erase(i);
        }

        oldest_id += num_to_remove;
        std::cout << "[MEMORY] Entropy applied. Purged " << num_to_remove << " old memories. Current capacity: " << id_map->ntotal << std::endl;
    }

    void MemoryManager::load_from_disk() {
        std::lock_guard<std::mutex> lock(mem_mutex);
        
        // FAISS Index yükle
        try {
            faiss::Index* loaded_index = faiss::read_index((db_path + ".index").c_str());
            delete id_map;
            delete base_index;
            id_map = dynamic_cast<faiss::IndexIDMap*>(loaded_index);
            // Uyarı: Dinamik cast başarısız olursa base_index yönetimi için ekstra güvenlik eklenebilir.
        } catch (...) {
            std::cerr << "[MEMORY] No existing FAISS index found. Starting fresh." << std::endl;
        }

        // Metin Kasasını yükle
        std::ifstream in(db_path + ".vault", std::ios::binary);
        if (in.is_open()) {
            while (in.peek() != EOF) {
                long id;
                size_t len;
                if (!in.read(reinterpret_cast<char*>(&id), sizeof(id))) break;
                if (!in.read(reinterpret_cast<char*>(&len), sizeof(len))) break;
                std::string payload(len, '\0');
                in.read(&payload[0], len);
                memory_vault[id] = payload;
                if (id >= next_id) next_id = id + 1;
            }
        }

        // Kalıcı Hafızayı (Text) yükle
        std::ifstream core_in(core_path);
        std::string line;
        while (std::getline(core_in, line)) {
            if (!line.empty()) core_memory_vault.push_back(line);
        }
    }
}
