#include "memory/EmbeddingProcessor.hpp"
#include <stdexcept>
#include <iostream>

namespace memory {
    EmbeddingProcessor::EmbeddingProcessor(const std::string& model_path) {
        llama_backend_init();
        auto m_params = llama_model_default_params();
        model = llama_model_load_from_file(model_path.c_str(), m_params);

        // KRİTİK: Pointer kontrolü ekle
        if (model == nullptr) {
            throw std::runtime_error("CRITICAL: Model file not found at " + model_path);
        }

        auto c_params = llama_context_default_params();
        c_params.embeddings = true;
        ctx = llama_init_from_model(model, c_params);
        if (ctx == nullptr) {
            throw std::runtime_error("CRITICAL: Failed to create llama context");
        }
        embedding_dim = llama_model_n_embd(model);
        std::cout << "[MEMORY] Embedding Processor ready. Dim: " << embedding_dim << std::endl;

        embedding_dim = llama_model_n_embd(model);
    }

    std::vector<float> EmbeddingProcessor::compute_embedding(const std::string& text) {
        const struct llama_vocab* vocab = llama_model_get_vocab(model);
        std::vector<llama_token> tokens(text.size() + 2);
        // Yeni API'de ilk argüman vocab'dir
        int n_tokens = llama_tokenize(vocab, text.c_str(), text.size(), tokens.data(), tokens.size(), true, false);
        tokens.resize(n_tokens);

        llama_batch batch = llama_batch_get_one(tokens.data(), n_tokens); // Argüman sayısı güncellendi
        if (llama_decode(ctx, batch) != 0) return {};

        const float* embd = llama_get_embeddings(ctx);
        return std::vector<float>(embd, embd + embedding_dim);
    }
    
    EmbeddingProcessor::~EmbeddingProcessor() {
        llama_free(ctx);
        llama_model_free(model);
        llama_backend_free();
    }
}
