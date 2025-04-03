#ifndef EXTPROCESS_APPLICATIONLOGIC_H
#define EXTPROCESS_APPLICATIONLOGIC_H

#include "helpers/llvm.h"

#define REPORT_ERROR(fmt, ...) ReportError(wxString::Format(fmt, ##__VA_ARGS__).ToStdString(), __FILE__, __LINE__)
#define DECLARE_LLAMA_FUNC(varname, type) \
    using varname##_t   = type;           \
    varname##_t varname = nullptr

#define LOAD_LLAMA_FUNC(func)                               \
    this->func = this->sd_dll.getFunction<func##_t>(#func); \
    if (this->func == nullptr) {                            \
        wxLogError("Failed to load function: " #func);      \
        return false;                                       \
    }

class ApplicationLogic {
public:
    ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager);
    ~ApplicationLogic();
    bool loadLibrary();
    void processMessage(sd_gui_utils::llvmMessage& message);

private:
    SharedLibrary sd_dll;
    std::string tempPath                                     = "";
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager = nullptr;
    std::atomic<bool> modelLoaded                            = {false};
    std::string currentModelPath                             = "";
    llama_model* model                                       = nullptr;
    llama_sampler* smplr                                     = nullptr;
    llama_context* ctx                                       = nullptr;
    llama_vocab* vocab                                       = nullptr;
    llama_token currToken;

    std::string tmpl                                          = "";
    std::shared_ptr<sd_gui_utils::llvmMessage> currentMessage = nullptr;
    std::vector<char> formatted_messages                      = {};
    std::vector<char> formatted_prompt                        = {};
    std::vector<llama_token> promptTokens                     = {};
    llama_batch batch;
    int prevLen = 0;

    // llama functions
    DECLARE_LLAMA_FUNC(ggml_backend_load_all, void (*)());
    DECLARE_LLAMA_FUNC(llama_backend_init, void (*)());
    DECLARE_LLAMA_FUNC(llama_init_from_model, llama_context* (*)(llama_model*, llama_context_params));
    DECLARE_LLAMA_FUNC(llama_free, void (*)(llama_context*));
    DECLARE_LLAMA_FUNC(llama_model_load_from_file, llama_model* (*)(const char* path, llama_model_params params));
    DECLARE_LLAMA_FUNC(llama_model_free, void (*)(llama_model*));
    DECLARE_LLAMA_FUNC(llama_backend_free, void (*)());
    DECLARE_LLAMA_FUNC(llama_sampler_chain_default_params, struct llama_sampler_chain_params (*)(void));
    DECLARE_LLAMA_FUNC(llama_sampler_chain_init, llama_sampler* (*)(struct llama_sampler_chain_params));
    DECLARE_LLAMA_FUNC(llama_context_default_params, struct llama_context_params (*)(void));
    DECLARE_LLAMA_FUNC(llama_model_default_params, struct llama_model_params (*)(void));
    DECLARE_LLAMA_FUNC(llama_sampler_init_min_p, struct llama_sampler* (*)(float p, size_t min_keep));
    DECLARE_LLAMA_FUNC(llama_sampler_chain_add, void (*)(struct llama_sampler* chain, struct llama_sampler* smpl));
    DECLARE_LLAMA_FUNC(llama_sampler_init_temp, struct llama_sampler* (*)(float t));
    DECLARE_LLAMA_FUNC(llama_sampler_init_dist, struct llama_sampler* (*)(uint32_t seed));
    DECLARE_LLAMA_FUNC(llama_sampler_free, void (*)(struct llama_sampler* smpl));
    DECLARE_LLAMA_FUNC(llama_model_get_vocab, llama_vocab* (*)(struct llama_model* model));
    DECLARE_LLAMA_FUNC(llama_batch_get_one, llama_batch (*)(llama_token* tokens, int32_t n_tokens));
    DECLARE_LLAMA_FUNC(llama_tokenize, int32_t (*)(const struct llama_vocab* vocab, const char* text, int32_t text_len, llama_token* tokens, int32_t n_tokens_max, bool add_special, bool parse_special));
    DECLARE_LLAMA_FUNC(llama_vocab_is_eog, bool (*)(const struct llama_vocab* vocab, llama_token token));
    DECLARE_LLAMA_FUNC(llama_decode, int32_t (*)(struct llama_context* ctx, struct llama_batch batch));
    DECLARE_LLAMA_FUNC(llama_sampler_sample, llama_token (*)(struct llama_sampler* smpl, struct llama_context* ctx, int32_t idx));
    DECLARE_LLAMA_FUNC(llama_token_to_piece, int32_t (*)(const struct llama_vocab* vocab, llama_token token, char* buf, int32_t length, int32_t lstrip, bool special));
    DECLARE_LLAMA_FUNC(llama_kv_self_used_cells, int32_t (*)(const struct llama_context* ctx));
    DECLARE_LLAMA_FUNC(llama_n_ctx, int32_t (*)(const struct llama_context* ctx));
    DECLARE_LLAMA_FUNC(llama_model_chat_template, const char* (*)(const struct llama_model* model, const char* name));
    DECLARE_LLAMA_FUNC(llama_chat_apply_template, int32_t (*)(const char* tmpl, const struct llama_chat_message* chat, size_t n_msg, bool add_ass, char* buf, int32_t length));
    DECLARE_LLAMA_FUNC(llama_chat_builtin_templates, int32_t (*)(const char** output, size_t len));
    DECLARE_LLAMA_FUNC(llama_model_meta_val_str, int32_t (*)(const struct llama_model* model, const char* key, char* buf, size_t buf_size));
    DECLARE_LLAMA_FUNC(llama_model_meta_key_by_index, int32_t (*)(const llama_model* model, int i, char* buf, size_t buf_size));
    DECLARE_LLAMA_FUNC(llama_model_meta_val_str_by_index, int32_t (*)(const llama_model* model, int i, char* buf, size_t buf_size));
    DECLARE_LLAMA_FUNC(llama_model_meta_count, int32_t (*)(const llama_model* model));
    DECLARE_LLAMA_FUNC(llama_model_desc, int32_t (*)(const llama_model* model, char* buf, size_t buf_size));
    DECLARE_LLAMA_FUNC(llama_model_size, uint64_t (*)(const llama_model* model));

    typedef std::vector<llama_chat_message> llama_message_list;
    std::mutex mutex;
    bool loadModel();
    void unloadModel();
    bool loadContext();
    void unloadContext();
    void generateText();
    void UpdateCurrentSession();
    inline void FindAChatTemplate() {
        const std::string template_str = this->currentMessage->GetPromptTemplate();

        if (!template_str.empty()) {
            this->tmpl = template_str;
            wxLogInfo("User's template: %s", tmpl);
            return;
        }

        const std::string meta_key_name = "tokenizer.chat_template";
        const std::string meta_value    = this->currentMessage->GetModelMeta(meta_key_name);

        if (!meta_value.empty()) {
            this->tmpl = meta_value;
        }
    }

    void ReportError(const std::string& message, std::string file = __FILE__, int line = __LINE__);

    std::string LlamaGenerate();
    inline void CleanUp() {
        this->unloadModel();
        this->unloadContext();
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->currentMessage = nullptr;
        }
    }
};

#endif  // EXTPROCESS_APPLICATIONLOGIC_H