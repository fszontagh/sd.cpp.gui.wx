#include "ApplicationLogic.h"

ApplicationLogic::ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager)
    : sd_dll(libName), sharedMemoryManager(sharedMemoryManager) {
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    auto TempDir   = std::getenv("Temp");
    this->tempPath = std::filesystem::path(TempDir).string();
#else
    this->tempPath = std::filesystem::temp_directory_path().string();
#endif
}

ApplicationLogic::~ApplicationLogic() {
    this->unloadContext();
    this->unloadModel();
}

bool ApplicationLogic::loadLibrary() {
    try {
        this->sd_dll.load();

        LOAD_LLAMA_FUNC(ggml_backend_load_all);
        LOAD_LLAMA_FUNC(llama_backend_init);
        LOAD_LLAMA_FUNC(llama_init_from_model);
        LOAD_LLAMA_FUNC(llama_free);
        LOAD_LLAMA_FUNC(llama_model_load_from_file);
        LOAD_LLAMA_FUNC(llama_model_free);
        LOAD_LLAMA_FUNC(llama_backend_free);
        LOAD_LLAMA_FUNC(llama_sampler_chain_init);
        LOAD_LLAMA_FUNC(llama_sampler_chain_default_params);
        LOAD_LLAMA_FUNC(llama_context_default_params);
        LOAD_LLAMA_FUNC(llama_model_default_params);
        LOAD_LLAMA_FUNC(llama_sampler_init_min_p);
        LOAD_LLAMA_FUNC(llama_sampler_chain_add);
        LOAD_LLAMA_FUNC(llama_sampler_init_temp);
        LOAD_LLAMA_FUNC(llama_sampler_init_dist);
        LOAD_LLAMA_FUNC(llama_sampler_free);
        LOAD_LLAMA_FUNC(llama_model_get_vocab);
        LOAD_LLAMA_FUNC(llama_batch_get_one);
        LOAD_LLAMA_FUNC(llama_tokenize);
        LOAD_LLAMA_FUNC(llama_vocab_is_eog);
        LOAD_LLAMA_FUNC(llama_decode);
        LOAD_LLAMA_FUNC(llama_sampler_sample);
        LOAD_LLAMA_FUNC(llama_token_to_piece);
        LOAD_LLAMA_FUNC(llama_kv_self_used_cells);
        LOAD_LLAMA_FUNC(llama_n_ctx);
        LOAD_LLAMA_FUNC(llama_model_chat_template);
        LOAD_LLAMA_FUNC(llama_chat_builtin_templates);
        LOAD_LLAMA_FUNC(llama_model_meta_val_str);
        LOAD_LLAMA_FUNC(llama_chat_apply_template);
        LOAD_LLAMA_FUNC(llama_model_meta_key_by_index);
        LOAD_LLAMA_FUNC(llama_model_meta_val_str_by_index);
        LOAD_LLAMA_FUNC(llama_model_meta_count);
        LOAD_LLAMA_FUNC(llama_model_desc);
        LOAD_LLAMA_FUNC(llama_model_size);
        LOAD_LLAMA_FUNC(llama_kv_self_clear);
        LOAD_LLAMA_FUNC(llama_kv_self_seq_cp);
        LOAD_LLAMA_FUNC(llama_kv_self_seq_add);
        LOAD_LLAMA_FUNC(llama_kv_self_seq_rm);
        LOAD_LLAMA_FUNC(llama_vocab_get_add_bos);

        return true;
    } catch (const std::exception& e) {
        wxLogError("Failed to load lib: %s", e.what());
        return false;
    }
}

// Utility function to report errors consistently
void ApplicationLogic::ReportError(const std::string& message, std::string file, int line) {
    if (isDEBUG) {
        wxLogError("%s:%d: %s", file.c_str(), line, message.c_str());
    } else {
        wxLogError("%s", message.c_str());
    }
    this->currentMessage->SetStatusMessage(message);
    this->currentMessage->SetStatus(sd_gui_utils::llvmstatus::LLVM_STATUS_ERROR);
    this->unloadContext();
    this->unloadModel();
    this->UpdateCurrentSession();
}

void ApplicationLogic::processMessage(sd_gui_utils::llvmMessage& message) {
    if (!this->currentMessage) {
        this->currentMessage = std::make_shared<sd_gui_utils::llvmMessage>(std::move(message));
    } else {
        this->currentMessage->Update(std::move(message));
    }

    this->UpdateCurrentSession();
    std::this_thread::sleep_for(std::chrono::milliseconds(LLAMA_SLEEP_TIME));

    switch (this->currentMessage->GetCommandType()) {
        case sd_gui_utils::llvmCommand::LLVM_COMMAND_MODEL_LOAD: {
            if (!this->loadModel()) {
                this->UpdateCurrentSession();
            } else {
                this->currentMessage->SetStatus(sd_gui_utils::llvmModelStatus::LLVM_MODEL_STATUS_LOADED);
                this->currentMessage->SetCommandType(sd_gui_utils::llvmCommand::LLVM_COMMAND_MODEL_LOAD);
                this->UpdateCurrentSession();
            }
        } break;
        case sd_gui_utils::llvmCommand::LLVM_COMMAND_MODEL_UNLOAD: {
            this->unloadModel();
            this->currentMessage->SetStatus(sd_gui_utils::llvmModelStatus::LLVM_MODEL_STATUS_UNLOADED);
            this->UpdateCurrentSession();
        } break;
        case sd_gui_utils::llvmCommand::LLVM_COMMAND_GENERATE_TEXT: {
            wxLogInfo("CMD: GENERATE_TEXT");
            if (this->currentMessage->GetModelPath() != this->currentModelPath || !this->model) {
                this->loadModel();
            }
            if (!this->ctx) {
                this->loadContext();
            }
            this->generateText();
        } break;
        case sd_gui_utils::llvmCommand::LLVM_COMMAND_GENERATE_TEXT_STREAM: {
            // Handle stream case if needed
        } break;
        case sd_gui_utils::llvmCommand::LLVM_COMMAND_CTX_LOAD: {
            this->loadContext();
            if (this->ctx) {
                this->currentMessage->SetStatus(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_LOADED);
            } else {
                this->currentMessage->SetStatus(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_UNLOADED);
            }
        } break;
        case sd_gui_utils::llvmCommand::LLVM_COMMAND_CTX_UNLOAD: {
            this->unloadContext();
            if (this->ctx) {
                this->currentMessage->SetStatus(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_LOADED);
            } else {
                this->currentMessage->SetStatus(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_UNLOADED);
            }
        } break;
    }
}

bool ApplicationLogic::loadModel() {
    ggml_backend_load_all();
    // unload previous model if necessary
    if (this->model != nullptr && this->currentModelPath != this->currentMessage->GetModelPath()) {
        if (this->ctx) {
            this->unloadContext();
        }
        this->unloadModel();
    }
    if (this->ctx) {
        this->unloadContext();
    }

    llama_model_params params = llama_model_default_params();
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->currentModelPath = this->currentMessage->GetModelPath();
        params.n_gpu_layers    = this->currentMessage->GetNgl();
        params.use_mmap        = true;
        params.devices         = nullptr;
    }
    wxLogInfo("Loading model: %s ngl: %d", this->currentModelPath.c_str(), params.n_gpu_layers);

    // load new model
    this->model = this->llama_model_load_from_file(this->currentModelPath.c_str(), params);
    if (!this->model) {
        this->model            = nullptr;
        this->modelLoaded      = false;
        this->currentModelPath = "";
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            REPORT_ERROR("Failed to load model: %s", this->currentMessage->GetModelPath());
        }
        return false;
    }
    this->currentMessage->ClearModelMeta();
    // load meta data from the model
    int32_t indexes = llama_model_meta_count(this->model);
    for (int32_t i = 0; i < indexes; i++) {
        int32_t key_size = llama_model_meta_key_by_index(this->model, i, nullptr, 0);
        int32_t val_size = llama_model_meta_val_str_by_index(this->model, i, nullptr, 0);

        if (key_size <= 0 || val_size <= 0) {
            continue;
        }

        std::vector<char> key(key_size + 1, '\0');
        std::vector<char> val(val_size + 1, '\0');

        llama_model_meta_key_by_index(this->model, i, key.data(), key_size + 1);
        llama_model_meta_val_str_by_index(this->model, i, val.data(), val_size + 1);
        key[key_size] = '\0';
        val[val_size] = '\0';

        std::string key_str(key.data(), key_size);
        std::string val_str(val.data(), val_size);

        this->currentMessage->SetModelMeta(key_str, val_str);
    }
    wxLogInfo("Loaded %d meta data from model", indexes);

    return true;
}

void ApplicationLogic::unloadModel() {
    if (this->model != nullptr) {
        llama_model_free(model);
        this->model            = nullptr;
        this->modelLoaded      = false;
        this->currentModelPath = "";
    }
    this->currentMessage->SetStatus(sd_gui_utils::llvmModelStatus::LLVM_MODEL_STATUS_UNLOADED);
}

bool ApplicationLogic::loadContext() {
    this->currentMessage->SetStatus(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_LOADING);
    this->UpdateCurrentSession();
    if (!this->model) {
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            REPORT_ERROR("Model not loaded");
        }
        return false;
    }

    wxLogInfo("Model loaded: %s, loading vocab", this->currentMessage->GetModelPath().c_str());
    this->vocab = this->llama_model_get_vocab(this->model);
    if (!this->vocab) {
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            REPORT_ERROR("Failed to load vocab");
        }
        return false;
    }
    this->add_bos_token = llama_vocab_get_add_bos(this->vocab);

    const auto n_batch = this->currentMessage->GetNBatch();

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx                = this->currentMessage->GetNctx();
    if (n_batch > 0) {
        ctx_params.n_batch = n_batch;  //
    } else {
        if (ctx_params.n_batch == 0) {
            ctx_params.n_batch = ctx_params.n_ctx;  // https://github.com/ggml-org/llama.cpp/blob/7242dd96756472f90ba41db4e5ebb3969dfc7e7c/examples/simple-chat/simple-chat.cpp#L83C16-L83C23
        }
        this->currentMessage->SetNBatch(ctx_params.n_batch);
    }
    ctx_params.n_threads = this->currentMessage->GetNThreads();

    wxLogInfo("Loading context for model: %s, n_batch: %d, n_ubatch: %d, n_ctx: %d", this->currentMessage->GetModelPath().c_str(), ctx_params.n_batch, ctx_params.n_ubatch, ctx_params.n_ctx);
    this->ctx = this->llama_init_from_model(this->model, ctx_params);

    if (!this->ctx) {
        {
            REPORT_ERROR("Failed to load context");
            return false;
        }
    }

    if (this->smplr) {
        this->llama_sampler_free(this->smplr);
        this->smplr = nullptr;
    }
    // initialize the sampler
    this->smplr = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(this->smplr, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(this->smplr, llama_sampler_init_temp(0.75f));
    llama_sampler_chain_add(this->smplr, llama_sampler_init_dist(1.0f));
    this->currentMessage->SetStatus(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_LOADED);
    this->UpdateCurrentSession();
    wxLogInfo("Context loaded successfully.");
    return true;
}

void ApplicationLogic::unloadContext() {
    if (this->ctx) {
        if (this->smplr) {
            this->llama_sampler_free(this->smplr);
            this->smplr = nullptr;
        }
        this->llama_free(this->ctx);
        this->ctx = nullptr;
    }
    this->currentMessage->SetStatus(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_UNLOADED);
}
void ApplicationLogic::generateText() {
    std::lock_guard<std::mutex> lock(this->mutex);

    if (!this->model) {
        REPORT_ERROR("Model not loaded");
        return;
    }
    if (!this->ctx) {
        REPORT_ERROR("Context not loaded");
        return;
    }
    if (!this->smplr) {
        REPORT_ERROR("Sampler not loaded");
        return;
    }

    if (this->tmpl.empty()) {
        this->FindAChatTemplate();
        if (this->tmpl.empty()) {
            REPORT_ERROR("Failed to find a chat template");
            this->unloadContext();
            this->unloadModel();
            return;
        }
    }

    const auto messages = this->currentMessage->GetChatMessages();

    wxLogInfo("Number of messages: %" PRIi64, messages.size());

    const auto newLen = this->ApplyTemplate(this->tmpl, messages, this->formatted_messages);

    if (newLen < 0) {
        REPORT_ERROR("Failed to apply chat template");
        this->unloadContext();
        this->unloadModel();
        return;
    }

    wxLogInfo("Prev len: %d, new len: %d", this->prevLen, newLen);
    // only add the new chars
    this->formatted_prompt.clear();
    this->formatted_prompt.assign(this->formatted_messages.begin() + this->prevLen, this->formatted_messages.begin() + newLen);

    // get the size first
    const bool is_first    = llama_kv_self_used_cells(ctx) == 0;
    const auto tokens_size = -llama_tokenize(this->vocab, this->formatted_prompt.data(), this->formatted_prompt.size(), nullptr, 0, is_first, true);
    if (tokens_size < 0) {
        REPORT_ERROR("Failed to tokenize. Tokens size: %d, prompt.size = %" PRIi64, tokens_size, this->formatted_prompt.size());
        this->unloadContext();
        this->unloadModel();
        return;
    }

    this->promptTokens.resize(tokens_size);

    if (llama_tokenize(this->vocab, this->formatted_prompt.data(), this->formatted_prompt.size(), this->promptTokens.data(), tokens_size, is_first, true) < 0) {
        REPORT_ERROR("Failed to tokenize");
        this->unloadContext();
        this->unloadModel();
        return;
    }
    wxLogInfo("New prompt's formatted tokens size: %" PRIi64, this->promptTokens.size());

    // Generate response
    auto response = this->LlamaGenerate();

    this->currentMessage->UpdateOrCreateAssistantAnswer(response);
}

std::string ApplicationLogic::LlamaGenerate() {
    std::string response;

    this->batch = llama_batch_get_one(this->promptTokens.data(), this->promptTokens.size());

    wxLogInfo("Batch n_tokens: %d", this->batch.n_tokens);

    std::vector<llama_token> generatedTokens;
    int n_ctx      = llama_n_ctx(ctx);
    int n_ctx_used = llama_kv_self_used_cells(ctx);
    wxLogInfo("n_ctx: %d, n_ctx_used: %d", n_ctx, n_ctx_used);

    bool overflowed = false;
    while (true) {
        const int n_ctx      = llama_n_ctx(ctx);
        const int n_ctx_used = llama_kv_self_used_cells(ctx);
        const int n_keep     = static_cast<int>(std::round(n_ctx * 0.9)) + this->add_bos_token;
        const int n_left     = n_ctx - n_ctx_used;
        wxLogInfo("n_ctx: %d, n_ctx_used: %d", n_ctx, n_ctx_used);

        if (n_ctx_used >= n_keep) {
            const int n_discard = n_ctx - n_keep;
            // copy the current context intpo another sequence
            llama_kv_self_seq_cp(ctx, 0, 1, -1, -1);  // copy everything from current seq to tmp seq
            llama_kv_self_seq_rm(ctx, 0, n_keep, n_keep + n_discard);
            llama_kv_self_seq_add(ctx, 0, n_keep + n_discard, n_left, -n_discard);  // allign
            overflowed = true;
        }

        // run the model
        if (llama_decode(this->ctx, this->batch) < 0) {
            REPORT_ERROR("Failed to decode");
            return response;
        }

        this->currToken = llama_sampler_sample(this->smplr, ctx, -1);
        if (llama_vocab_is_eog(this->vocab, this->currToken)) {
            break;
        }

        generatedTokens.push_back(this->currToken);

        // Convert the token to a string and add it to the response
        char buf[256];
        int n = llama_token_to_piece(vocab, this->currToken, buf, sizeof(buf), 0, true);
        if (n < 0) {
            REPORT_ERROR("Failed to convert token to string");
            return "";
        }
        std::string piece(buf, n);
        response += piece;

        this->currentMessage->UpdateOrCreateAssistantAnswer(piece);
        this->UpdateCurrentSession();
        this->batch = llama_batch_get_one(&this->currToken, 1);
    }
    if (overflowed) {
        llama_kv_self_seq_cp(ctx, 1, 0, 0, -1);
        llama_kv_self_seq_rm(ctx, 1, -1, -1);
        overflowed = false;
    }

    if (this->prevLen == 0) {
        const auto messages = this->currentMessage->GetChatMessages();
        const auto plen     = llama_chat_apply_template(this->tmpl.c_str(), messages.data(), messages.size(), false, nullptr, 0);
        if (plen < 0) {
            wxLogWarning("Failed to apply chat template on the end");
            this->prevLen = 0;
        } else {
            this->prevLen = plen;
        }
    }

    return response;
}

void ApplicationLogic::UpdateCurrentSession() {
    const auto msg = this->currentMessage->toString();
    this->sharedMemoryManager->write(msg);
    // wxLogInfo("Sending: %s", msg.c_str());
}
