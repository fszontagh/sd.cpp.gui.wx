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
        return true;
    } catch (const std::exception& e) {
        wxLogError("Failed to load lib: %s", e.what());
        return false;
    }
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

    // Get the latest user prompt from llvmMessage
    std::string prompt = this->currentMessage->GetLatestUserPrompt();

    // Tokenize input prompt
    const bool is_first       = llama_kv_self_used_cells(this->ctx) == 0;
    const int n_prompt_tokens = -this->llama_tokenize(this->vocab, prompt.c_str(), prompt.size(), nullptr, 0, is_first, true);
    const auto ctx_size       = llama_n_ctx(this->ctx);

    wxLogInfo("n_prompt_tokens: %d  ctx_size: %d n_batch: %d", n_prompt_tokens, ctx_size, this->currentMessage->GetNBatch());
    // Check if the number of tokens exceeds the batch size
    if (n_prompt_tokens > ctx_size) {
        REPORT_ERROR("Prompt too long for the batch size");
        return;
    }

    std::vector<llama_token> prompt_tokens(n_prompt_tokens);
    if (this->llama_tokenize(this->vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
        REPORT_ERROR("Failed to tokenize prompt");
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

    // check if template is a minja template
    std::string formatted_prompt = "";

    // const std::string minja = this->currentMessage->MessagesFromNinja(this->tmpl);
    // if (!minja.empty()) {
    //   formatted_prompt = minja;
    //} else {
    // Format the message history
    std::vector<char> formatted(1024);
    int new_len = -1;

    auto messages = this->currentMessage->GetChatMessages();

    new_len = llama_chat_apply_template(this->tmpl.c_str(), messages.data(), messages.size(), true, formatted.data(), formatted.size());

    if (new_len > static_cast<int>(formatted.size())) {
        formatted.resize(new_len);
        new_len = llama_chat_apply_template(this->tmpl.c_str(), messages.data(), messages.size(), true, formatted.data(), formatted.size());
    }

    if (new_len < 0) {
        REPORT_ERROR("Failed to apply the chat template");
        return;
    }

    formatted_prompt = std::string(formatted.begin(), formatted.begin() + new_len);
    //}

    wxLogInfo("Formatted prompt: %s", formatted_prompt.c_str());

    if (formatted_prompt.empty()) {
        REPORT_ERROR("Failed to apply the chat template");
        return;
    }

    // Generate response
    auto response = this->LlamaGenerate(formatted_prompt);

    this->currentMessage->UpdateOrCreateAssistantAnswer(response);
    wxLogInfo("Generated response: %s", response.c_str());
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

std::string ApplicationLogic::LlamaGenerate(const std::string& prompt) {
    std::string response;

    const bool is_first = llama_kv_self_used_cells(ctx) == 0;
    wxLogInfo("Generating response for prompt: %s", prompt.c_str());
    // tokenize the prompt
    const int n_prompt_tokens = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, is_first, true);
    std::vector<llama_token> prompt_tokens(n_prompt_tokens);

    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
        REPORT_ERROR("Failed to tokenize prompt");
        return "";
    }

    // prepare a batch for the prompt
    llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
    llama_token new_token_id;
    while (true) {
        // check if we have enough space in the context to evaluate this batch
        int n_ctx      = llama_n_ctx(ctx);
        int n_ctx_used = llama_kv_self_used_cells(ctx);

        if (n_ctx_used + batch.n_tokens > n_ctx) {
            int first_keep_tokens = std::ceil(n_ctx * 0.2);
            int keep_tokens       = n_ctx - batch.n_tokens;

            if (keep_tokens < first_keep_tokens) {
                keep_tokens = first_keep_tokens;
            }

            // Keep only the first few and last few tokens
            if ((int)prompt_tokens.size() > keep_tokens) {
                std::vector<llama_token> trimmed_tokens;

                // keep first N tokens
                trimmed_tokens.insert(trimmed_tokens.end(),
                                      prompt_tokens.begin(),
                                      prompt_tokens.begin() + first_keep_tokens);

                // Keep the last (keep_tokens - first_keep_tokens) tokens as well
                trimmed_tokens.insert(trimmed_tokens.end(),
                                      prompt_tokens.end() - (keep_tokens - first_keep_tokens),
                                      prompt_tokens.end());

                prompt_tokens = std::move(trimmed_tokens);
            }

            batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

            wxLogWarning("Context truncated: keeping first %d and last %d tokens (total kept: %d)",
                         first_keep_tokens,
                         keep_tokens - first_keep_tokens,
                         prompt_tokens.size());
        }

        if (llama_decode(ctx, batch)) {
            REPORT_ERROR("Failed to decode the prompt");
            return response;
        }

        // sample the next token
        new_token_id = llama_sampler_sample(this->smplr, ctx, -1);

        // is it an end of generation?
        if (llama_vocab_is_eog(vocab, new_token_id)) {
            break;
        }

        // convert the token to a string, print it and add it to the response
        char buf[256];
        int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            REPORT_ERROR("Failed to convert the token to a string");
            return "";
        }
        std::string piece(buf, n);
        response += piece;

        this->currentMessage->UpdateOrCreateAssistantAnswer(piece);
        this->UpdateCurrentSession();

        // prepare the next batch with the sampled token
        batch = llama_batch_get_one(&new_token_id, 1);
    }

    return response;
}
void ApplicationLogic::UpdateCurrentSession() {
    const auto msg = this->currentMessage->toString();
    this->sharedMemoryManager->write(msg);
    wxLogInfo("Sending: %s", msg.c_str());
}
