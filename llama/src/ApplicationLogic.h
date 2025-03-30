#ifndef EXTPROCESS_APPLICATIONLOGIC_H
#define EXTPROCESS_APPLICATIONLOGIC_H

#include "helpers/llvm.h"
class ApplicationLogic {
public:
    ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager);
    ~ApplicationLogic();
    bool loadLibrary();
    void processMessage(sd_gui_utils::llvmMessage& message);

private:
    SharedLibrary sd_dll;
    std::string tempPath                                      = "";
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager  = nullptr;
    std::atomic<bool> modelLoaded                             = {false};
    std::string currentModelPath                              = "";
    llama_model* model                                        = nullptr;
    llama_sampler* smplr                                      = nullptr;
    llama_context* ctx                                        = nullptr;
    llama_vocab* vocab                                        = nullptr;
    std::shared_ptr<sd_gui_utils::llvmMessage> currentMessage = nullptr;

    // llama functions
    using GgmlBackendLoadAll             = void (*)(void);
    using LlamaBackendInit               = void (*)(void);
    using LlamaInitFromModelFunction     = llama_context* (*)(llama_model*, llama_context_params);
    using LlamaFreeFunction              = void (*)(llama_context*);
    using LlamaModelLoadFromFile         = llama_model* (*)(const char* path, llama_model_params params);
    using LlamaModelFreeFunction         = void (*)(llama_model*);
    using LlamaBackendFreeFunction       = void (*)();
    using LlamaSamplerChainDefaultParams = struct llama_sampler_chain_params (*)(void);
    using LlamaSamplerChainInitFunction  = llama_sampler* (*)(struct llama_sampler_chain_params);
    using LlamaContextDefaultParams      = struct llama_context_params (*)(void);
    using LlamaModelDefaultParams        = struct llama_model_params (*)(void);
    using LlamaSamplerInitMinP           = struct llama_sampler* (*)(float p, size_t min_keep);
    using LlamaSamplerChainAdd           = void (*)(struct llama_sampler* chain, struct llama_sampler* smpl);
    using LlamaSamplerInitTemp           = struct llama_sampler* (*)(float t);
    using LlamaSamplerInitDist           = struct llama_sampler* (*)(uint32_t seed);
    using LlamaSamplerFree               = void (*)(struct llama_sampler* smpl);
    using LlamaModelGetVocab             = llama_vocab* (*)(struct llama_model* model);
    using LlamaBatchGetOne               = llama_batch (*)(llama_token* tokens, int32_t n_tokens);
    using LlamaTokenize                  = int32_t (*)(const struct llama_vocab* vocab, const char* text, int32_t text_len, llama_token* tokens, int32_t n_tokens_max, bool add_special, bool parse_special);
    using LlamaVocabIsEog                = bool (*)(const struct llama_vocab* vocab, llama_token token);
    using LlamaDecode                    = int32_t (*)(struct llama_context* ctx, struct llama_batch batch);
    using LlamaSamplerSample             = llama_token (*)(struct llama_sampler* smpl, struct llama_context* ctx, int32_t idx);
    using LlamaTokenToPiece              = int32_t (*)(const struct llama_vocab* vocab, llama_token token, char* buf, int32_t length, int32_t lstrip, bool special);
    using LlamaKvselfUsedCells           = int32_t (*)(const struct llama_context* ctx);
    using LlamaNCtx                      = int32_t (*)(const struct llama_context* ctx);
    using LlamaModelChatTemplate         = const char* (*)(const struct llama_model* model, const char* name);
    using LlamaChatApplyTemplate         = int32_t (*)(const char* tmpl, const struct llama_chat_message* chat, size_t n_msg, bool add_ass, char* buf, int32_t length);

    LlamaBackendInit llama_backend_init                               = nullptr;
    GgmlBackendLoadAll ggml_backend_load_all                          = nullptr;
    LlamaInitFromModelFunction llama_init_from_model                  = nullptr;
    LlamaFreeFunction llama_free                                      = nullptr;
    LlamaModelLoadFromFile llama_model_load_from_file                 = nullptr;
    LlamaModelFreeFunction llama_model_free                           = nullptr;
    LlamaBackendFreeFunction llama_backend_free                       = nullptr;
    LlamaSamplerChainInitFunction llama_sampler_chain_init            = nullptr;
    LlamaSamplerChainDefaultParams llama_sampler_chain_default_params = nullptr;
    LlamaContextDefaultParams llama_context_default_params            = nullptr;
    LlamaModelDefaultParams llama_model_default_params                = nullptr;
    LlamaSamplerInitMinP llama_sampler_init_min_p                     = nullptr;
    LlamaSamplerChainAdd llama_sampler_chain_add                      = nullptr;
    LlamaSamplerInitTemp llama_sampler_init_temp                      = nullptr;
    LlamaSamplerInitDist llama_sampler_init_dist                      = nullptr;
    LlamaSamplerFree llama_sampler_free                               = nullptr;
    LlamaModelGetVocab llama_model_get_vocab                          = nullptr;
    LlamaBatchGetOne llama_batch_get_one                              = nullptr;
    LlamaTokenize llama_tokenize                                      = nullptr;
    LlamaVocabIsEog llama_vocab_is_eog                                = nullptr;
    LlamaDecode llama_decode                                          = nullptr;
    LlamaSamplerSample llama_sampler_sample                           = nullptr;
    LlamaTokenToPiece llama_token_to_piece                            = nullptr;
    LlamaKvselfUsedCells llama_kv_self_used_cells                     = nullptr;
    LlamaNCtx llama_n_ctx                                             = nullptr;
    LlamaModelChatTemplate llama_model_chat_template                  = nullptr;
    LlamaChatApplyTemplate llama_chat_apply_template                  = nullptr;

    std::vector<llama_chat_message> messages = {};
    std::vector<char> formatted              = {};
    int prev_len                             = 0;
    std::mutex mutex;
    bool loadModel();
    void unloadModel();
    bool loadContext();
    void unloadContext();
    void generateText();
    void UpdateCurrentSession();

    std::string LlamaGenerate(const std::string& prompt);
};

#endif  // EXTPROCESS_APPLICATIONLOGIC_H