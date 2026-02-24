#pragma once
#include <string>
#include <vector>
#include <llama.h>

namespace memory {
    class EmbeddingProcessor {
    public:
        EmbeddingProcessor(const std::string& model_path);
        ~EmbeddingProcessor();

        std::vector<float> compute_embedding(const std::string& text);
        std::string generate_text(const std::string& prompt, int max_tokens = 64);
        int get_dim() const { return embedding_dim; }

    private:
        llama_model* model;
        llama_context* ctx;
        int embedding_dim;
        int n_past; // RASYONEL EKLENTİ: Zaman algısı ve pozisyon takibi
    };
}
