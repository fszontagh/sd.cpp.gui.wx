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
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager;
    std::string tempPath;
    std::string error;
    std::atomic<bool> modelLoaded = {false};
    llama_model* model            = nullptr;
    llama_context* ctx            = nullptr;
    llama_sampler* smplr          = nullptr;
    std::string currentModelPath  = "";

    // llama functions
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

    bool loadModel(const sd_gui_utils::llvmMessage& message);
    void unloadModel();

    bool loadContext(const sd_gui_utils::llvmMessage& message);

    void unloadContext();
};

#endif  // EXTPROCESS_APPLICATIONLOGIC_H