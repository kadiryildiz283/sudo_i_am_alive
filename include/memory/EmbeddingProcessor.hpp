#pragma once

#include <string>
#include <vector>
#include "llama.h"

namespace memory {

    class EmbeddingProcessor {
    private:
        llama_model* model;
        llama_context* ctx;
        int32_t embedding_dim;

    public:
        // Initializes the embedding engine using the GGUF model path
        EmbeddingProcessor(const std::string& model_path);
        ~EmbeddingProcessor();

        // Transforms raw text into a dense vector (Inference)
        std::vector<float> compute_embedding(const std::string& text);

        int32_t get_dim() const { return embedding_dim; }
    };

} // namespace memory
