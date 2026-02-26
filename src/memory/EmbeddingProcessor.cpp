#include "memory/EmbeddingProcessor.hpp"
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace memory {
        EmbeddingProcessor::EmbeddingProcessor(const std::string& model_path) {
           llama_log_set([](enum ggml_log_level level, const char * text, void * user_data) {
        }, nullptr);

        llama_backend_init();
        auto m_params = llama_model_default_params();
        model = llama_model_load_from_file(model_path.c_str(), m_params);

        if (model == nullptr) {
            throw std::runtime_error("CRITICAL: Model file not found at " + model_path);
        }

        auto c_params = llama_context_default_params();
        c_params.n_ctx = 4096; // RASYONEL GENİŞLETME: Zihin kapasitesi 4096 tokense çıkarıldı
        c_params.embeddings = true;
        ctx = llama_init_from_model(model, c_params);
        if (ctx == nullptr) {
            throw std::runtime_error("CRITICAL: Failed to create llama context");
        }
        
        embedding_dim = llama_model_n_embd(model);
        std::cout << "[MEMORY] Embedding Processor ready. Dim: " << embedding_dim << std::endl;
    }

    std::vector<float> EmbeddingProcessor::compute_embedding(const std::string& text) {
        llama_free(ctx);
        auto c_params = llama_context_default_params();
        c_params.n_ctx = 4096; // RASYONEL GENİŞLETME
        c_params.embeddings = true;
        ctx = llama_init_from_model(model, c_params);

        const struct llama_vocab* vocab = llama_model_get_vocab(model);
        std::vector<llama_token> tokens(text.size() + 2);
        
        int n_tokens = llama_tokenize(vocab, text.c_str(), text.size(), tokens.data(), tokens.size(), true, false);
        if (n_tokens < 0) {
            tokens.resize(-n_tokens);
            n_tokens = llama_tokenize(vocab, text.c_str(), text.size(), tokens.data(), tokens.size(), true, false);
        }
        tokens.resize(n_tokens);

        llama_batch batch = llama_batch_get_one(tokens.data(), n_tokens);
        if (llama_decode(ctx, batch) != 0) return {};

        const float* embd = llama_get_embeddings(ctx);
        if (!embd) return {};
        return std::vector<float>(embd, embd + embedding_dim);
    }

    std::string EmbeddingProcessor::generate_text(const std::string& prompt, int max_tokens) {
        llama_free(ctx);
        auto c_params = llama_context_default_params();
        c_params.n_ctx = 4096; // RASYONEL GENİŞLETME
        c_params.embeddings = true;
        ctx = llama_init_from_model(model, c_params);

        const struct llama_vocab* vocab = llama_model_get_vocab(model);

        std::vector<llama_token> tokens_list(prompt.size() + 2);
        int n_tokens = llama_tokenize(vocab, prompt.c_str(), prompt.length(), tokens_list.data(), tokens_list.size(), true, true);
        
        if (n_tokens < 0) {
            tokens_list.resize(-n_tokens);
            n_tokens = llama_tokenize(vocab, prompt.c_str(), prompt.length(), tokens_list.data(), tokens_list.size(), true, true);
        }
        tokens_list.resize(n_tokens);

        // GÜVENLİK KİLİDİ: Eğer prompt çok büyükse Context Window'a sığdırmak için kırp
        if (n_tokens > 4096 - max_tokens) {
            n_tokens = 4096 - max_tokens;
            std::cout << "[COGNITION] Warning: Context limit reached. Truncating input." << std::endl;
        }

        // RASYONEL DÜZELTME: Hardcoded 512 yerine dinamik bellek tahsisi
        llama_batch batch = llama_batch_init(n_tokens + max_tokens, 0, 1);
        batch.n_tokens = 0; 
        
        for (int i = 0; i < n_tokens; i++) {
            batch.token[batch.n_tokens] = tokens_list[i];
            batch.pos[batch.n_tokens] = i; 
            batch.n_seq_id[batch.n_tokens] = 1;
            batch.seq_id[batch.n_tokens][0] = 0;
            batch.logits[batch.n_tokens] = false;
            batch.n_tokens++;
        }
        batch.logits[batch.n_tokens - 1] = true;

        if (llama_decode(ctx, batch) != 0) {
            llama_batch_free(batch);
            return "ERROR: Cognitive engine failed to decode prompt.";
        }

        std::string response = "";
        int n_cur = batch.n_tokens;
        int n_vocab = llama_vocab_n_tokens(vocab);

        for (int i = 0; i < max_tokens; i++) {
            auto* logits = llama_get_logits_ith(ctx, batch.n_tokens - 1);
            
            llama_token new_token_id = 0;
            float max_logit = -1e9;
            for (llama_token tid = 0; tid < n_vocab; tid++) {
                if (logits[tid] > max_logit) {
                    max_logit = logits[tid];
                    new_token_id = tid;
                }
            }

            if (new_token_id == llama_vocab_eos(vocab)) break;

            char buf[128];
            int piece_len = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, false); 
            if (piece_len > 0) {
                response += std::string(buf, piece_len);
            }

            batch.n_tokens = 0; 
            batch.token[batch.n_tokens] = new_token_id;
            batch.pos[batch.n_tokens] = n_cur;
            batch.n_seq_id[batch.n_tokens] = 1;
            batch.seq_id[batch.n_tokens][0] = 0;
            batch.logits[batch.n_tokens] = true;
            batch.n_tokens++;
            n_cur++;

            if (llama_decode(ctx, batch) != 0) break;
        }

        llama_batch_free(batch);
        return response;
    }

    EmbeddingProcessor::~EmbeddingProcessor() {
        llama_free(ctx);
        llama_model_free(model);
        llama_backend_free();
    }
}
