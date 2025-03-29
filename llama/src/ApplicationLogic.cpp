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
        sd_dll.load();

        this->llama_chat_apply_template = sd_dll.getFunction<LlamaChatApplyTemplate>("llama_chat_apply_template");
        if (this->llama_chat_apply_template == nullptr) {
            wxLogError("Failed to load function: llama_chat_apply_template");
            return false;
        }

        this->llama_model_chat_template = sd_dll.getFunction<LlamaModelChatTemplate>("llama_model_chat_template");
        if (this->llama_model_chat_template == nullptr) {
            wxLogError("Failed to load function: llama_model_chat_template");
            return false;
        }

        this->llama_kv_self_used_cells = sd_dll.getFunction<LlamaKvselfUsedCells>("llama_kv_self_used_cells");
        if (this->llama_kv_self_used_cells == nullptr) {
            wxLogError("Failed to load function: llama_kv_self_used_cells");
            return false;
        }
        this->llama_n_ctx = sd_dll.getFunction<LlamaNCtx>("llama_n_ctx");
        if (this->llama_n_ctx == nullptr) {
            wxLogError("Failed to load function: llama_n_ctx");
            return false;
        }

        this->llama_backend_init = sd_dll.getFunction<LlamaBackendInit>("llama_backend_init");
        if (this->llama_backend_init == nullptr) {
            wxLogError("Failed to load function: llama_backend_init");
            return false;
        }
        // this->llama_backend_init();

        this->ggml_backend_load_all = (GgmlBackendLoadAll)sd_dll.getFunction<GgmlBackendLoadAll>("ggml_backend_load_all");
        if (this->ggml_backend_load_all == nullptr) {
            wxLogError("Failed to load function: ggml_backend_load_all");
            return false;
        }

        this->llama_init_from_model = sd_dll.getFunction<LlamaInitFromModelFunction>("llama_init_from_model");
        if (this->llama_init_from_model == nullptr) {
            wxLogError("Failed to load function: llama_init_from_model");
            return false;
        }
        this->llama_free = sd_dll.getFunction<LlamaFreeFunction>("llama_free");
        if (this->llama_free == nullptr) {
            wxLogError("Failed to load function: llama_free");
            return false;
        }
        this->llama_model_load_from_file = sd_dll.getFunction<LlamaModelLoadFromFile>("llama_model_load_from_file");
        if (this->llama_model_load_from_file == nullptr) {
            wxLogError("Failed to load function: llama_model_load_from_file");
            return false;
        }
        this->llama_model_free = sd_dll.getFunction<LlamaModelFreeFunction>("llama_model_free");
        if (this->llama_model_free == nullptr) {
            wxLogError("Failed to load function: llama_model_free");
            return false;
        }
        this->llama_backend_free = sd_dll.getFunction<LlamaBackendFreeFunction>("llama_backend_free");
        if (this->llama_backend_free == nullptr) {
            wxLogError("Failed to load function: llama_backend_free");
            return false;
        }
        this->llama_sampler_chain_init = sd_dll.getFunction<LlamaSamplerChainInitFunction>("llama_sampler_chain_init");
        if (this->llama_sampler_chain_init == nullptr) {
            wxLogError("Failed to load function: llama_sampler_chain_init");
            return false;
        }
        this->llama_sampler_chain_default_params = sd_dll.getFunction<LlamaSamplerChainDefaultParams>("llama_sampler_chain_default_params");
        if (this->llama_sampler_chain_default_params == nullptr) {
            wxLogError("Failed to load function: llama_sampler_chain_default_params");
            return false;
        }
        this->llama_context_default_params = sd_dll.getFunction<LlamaContextDefaultParams>("llama_context_default_params");
        if (this->llama_context_default_params == nullptr) {
            wxLogError("Failed to load function: llama_context_default_params");
            return false;
        }
        this->llama_model_default_params = sd_dll.getFunction<LlamaModelDefaultParams>("llama_model_default_params");
        if (this->llama_model_default_params == nullptr) {
            wxLogError("Failed to load function: llama_model_default_params");
            return false;
        }
        this->llama_sampler_init_min_p = sd_dll.getFunction<LlamaSamplerInitMinP>("llama_sampler_init_min_p");
        if (this->llama_sampler_init_min_p == nullptr) {
            wxLogError("Failed to load function: llama_sampler_init_min_p");
            return false;
        }
        this->llama_sampler_chain_add = sd_dll.getFunction<LlamaSamplerChainAdd>("llama_sampler_chain_add");
        if (this->llama_sampler_chain_add == nullptr) {
            wxLogError("Failed to load function: llama_sampler_chain_add");
            return false;
        }
        this->llama_sampler_init_temp = sd_dll.getFunction<LlamaSamplerInitTemp>("llama_sampler_init_temp");
        if (this->llama_sampler_init_temp == nullptr) {
            wxLogError("Failed to load function: llama_sampler_init_temp");
            return false;
        }
        this->llama_sampler_init_dist = sd_dll.getFunction<LlamaSamplerInitDist>("llama_sampler_init_dist");
        if (this->llama_sampler_init_dist == nullptr) {
            wxLogError("Failed to load function: llama_sampler_init_dist");
            return false;
        }
        this->llama_sampler_free = sd_dll.getFunction<LlamaSamplerFree>("llama_sampler_free");
        if (this->llama_sampler_free == nullptr) {
            wxLogError("Failed to load function: llama_sampler_free");
            return false;
        }

        this->llama_model_get_vocab = sd_dll.getFunction<LlamaModelGetVocab>("llama_model_get_vocab");
        if (this->llama_model_get_vocab == nullptr) {
            wxLogError("Failed to load function: llama_model_get_vocab");
            return false;
        }

        this->llama_batch_get_one = sd_dll.getFunction<LlamaBatchGetOne>("llama_batch_get_one");
        if (this->llama_batch_get_one == nullptr) {
            wxLogError("Failed to load function: llama_batch_get_one");
            return false;
        }

        this->llama_tokenize = sd_dll.getFunction<LlamaTokenize>("llama_tokenize");
        if (this->llama_tokenize == nullptr) {
            wxLogError("Failed to load function: llama_tokenize");
            return false;
        }
        this->llama_vocab_is_eog = sd_dll.getFunction<LlamaVocabIsEog>("llama_vocab_is_eog");
        if (this->llama_vocab_is_eog == nullptr) {
            wxLogError("Failed to load function: llama_vocab_is_eog");
            return false;
        }

        this->llama_decode = sd_dll.getFunction<LlamaDecode>("llama_decode");
        if (this->llama_decode == nullptr) {
            wxLogError("Failed to load function: llama_decode");
            return false;
        }

        this->llama_sampler_sample = sd_dll.getFunction<LlamaSamplerSample>("llama_sampler_sample");
        if (this->llama_sampler_sample == nullptr) {
            wxLogError("Failed to load function: llama_sampler_sample");
            return false;
        }

        this->llama_token_to_piece = sd_dll.getFunction<LlamaTokenToPiece>("llama_token_to_piece");
        if (this->llama_token_to_piece == nullptr) {
            wxLogError("Failed to load function: llama_token_to_piece");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        wxLogError("Failed to load lib: %s", e.what());
        return false;
    }
}
void ApplicationLogic::processMessage(sd_gui_utils::llvmMessage& message) {
    this->currentMessage = std::make_shared<sd_gui_utils::llvmMessage>(std::move(message));
    switch (this->currentMessage->GetCommandType()) {
        case sd_gui_utils::MODEL_LOAD: {
            wxLogInfo("CMD MODEL_LOAD: %s", this->currentMessage->GetModelPath().c_str());
            if (!this->loadModel()) {
                this->UpdateCurrentSession();
            }
        } break;
        case sd_gui_utils::MODEL_UNLOAD: {
            wxLogInfo("CMD MODEL_UNLOAD: %s", this->currentMessage->GetModelPath().c_str());
            this->unloadModel();
        } break;
        case sd_gui_utils::GENERATE_TEXT: {
            wxLogInfo("CMD: GENERATE_TEXT");
            if (this->currentMessage->GetModelPath() != this->currentModelPath) {
                this->loadModel();
                if (!this->ctx) {
                    this->loadContext();
                }
            }
            this->generateText();
            this->UpdateCurrentSession();
        } break;
        case sd_gui_utils::GENERATE_TEXT_STREAM: {
        } break;
    }
};
bool ApplicationLogic::loadModel() {
    ggml_backend_load_all();
    //  unload previous model
    if (this->model != nullptr && this->currentModelPath != this->currentMessage->GetModelPath()) {
        llama_model_free(model);
        this->model = nullptr;
    }
    this->currentModelPath    = this->currentMessage->GetModelPath();
    llama_model_params params = llama_model_default_params();
    params.n_gpu_layers       = this->currentMessage->GetNgl();
    params.use_mmap           = false;
    params.devices            = nullptr;
    wxLogInfo("Loading model: %s ngl: %d", this->currentMessage->GetModelPath().c_str(), this->currentMessage->GetNgl());

    // load new model
    this->model = this->llama_model_load_from_file(this->currentMessage->GetModelPath().c_str(), params);
    if (!this->model) {
        this->model            = nullptr;
        this->modelLoaded      = false;
        this->currentModelPath = "";
        this->currentMessage->SetStatusMessage("Failed to load model: " + this->currentMessage->GetModelPath());
        wxLogError(this->currentMessage->GetStatusMessage().c_str());
        this->UpdateCurrentSession();
        return false;
    }
    return true;
};

void ApplicationLogic::unloadModel() {
    if (this->model != nullptr) {
        llama_model_free(model);
        this->model            = nullptr;
        this->modelLoaded      = false;
        this->currentModelPath = "";
    }
}

bool ApplicationLogic::loadContext() {
    if (!this->model) {
        this->currentMessage->SetStatusMessage("Model not loaded");
        this->UpdateCurrentSession();
        return false;
    }

    wxLogInfo("Model loaded: %s, loading vocab", this->currentMessage->GetModelPath().c_str());
    this->vocab = this->llama_model_get_vocab(this->model);
    if (!this->vocab) {
        this->currentMessage->SetStatusMessage("Failed to load vocab");
        this->UpdateCurrentSession();
        return false;
    }

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx                = this->currentMessage->GetNctx();
    ctx_params.n_batch              = this->currentMessage->GetNBatch();
    ctx_params.n_threads            = this->currentMessage->GetNThreads();

    wxLogInfo("Loading context: %s", this->currentMessage->GetModelPath().c_str());
    this->ctx = this->llama_init_from_model(this->model, ctx_params);

    if (!this->ctx) {
        this->currentMessage->SetStatusMessage("Failed to load context");
        this->UpdateCurrentSession();
        return false;
    }

    if (this->smplr) {
        this->llama_sampler_free(this->smplr);
        this->smplr = nullptr;
    }
    // initialize the sampler
    this->smplr = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(this->smplr, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(this->smplr, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(this->smplr, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

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
}
void ApplicationLogic::generateText() {
    if (this->model == nullptr) {
        this->currentMessage->SetStatusMessage("Model not loaded");
        this->UpdateCurrentSession();
        return;
    }
    if (this->ctx == nullptr) {
        this->currentMessage->SetStatusMessage("Context not loaded");
        this->UpdateCurrentSession();
        return;
    }
    if (this->smplr == nullptr) {
        this->currentMessage->SetStatusMessage("Sampler not loaded");
        this->UpdateCurrentSession();
        return;
    }
    // Tokenize the input prompt.
    const bool is_first       = llama_kv_self_used_cells(this->ctx) == 0;
    const int n_prompt_tokens = -this->llama_tokenize(this->vocab, this->currentMessage->GetLatestUserPrompt().c_str(), this->currentMessage->GetLatestUserPrompt().size(), NULL, 0, is_first, true);

    std::vector<llama_token> prompt_tokens(n_prompt_tokens);
    if (this->llama_tokenize(this->vocab, this->currentMessage->GetLatestUserPrompt().c_str(), this->currentMessage->GetLatestUserPrompt().size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
        wxLogError("Failed to tokenize prompt");
        this->currentMessage->SetStatusMessage("Failed to tokenize prompt");
        this->UpdateCurrentSession();
        return;
    }

    const char* tmpl = llama_model_chat_template(model, /* name */ nullptr);
    this->messages.push_back({"user", strdup(this->currentMessage->GetLatestUserPrompt().c_str())});
    int new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
    if (new_len > (int)formatted.size()) {
        formatted.resize(new_len);
        new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
    }
    if (new_len < 0) {
        wxLogError("Failed to apply the chat template");
        this->currentMessage->SetStatusMessage("Failed to apply the chat template");
        this->UpdateCurrentSession();
        return;
    }
    std::string prompt(formatted.begin() + prev_len, formatted.begin() + new_len);
    auto response = this->LlamaGenerate(prompt);
    messages.push_back({"assistant", strdup(response.c_str())});
    prev_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), false, nullptr, 0);
    if (prev_len < 0) {
        wxLogError("Failed to apply the chat template");
        this->currentMessage->SetStatusMessage("Failed to apply the chat template");
        this->UpdateCurrentSession();
        return;
    }
}
std::string ApplicationLogic::LlamaGenerate(const std::string& prompt) {
    std::string response;

    const bool is_first = llama_kv_self_used_cells(ctx) == 0;
    wxLogInfo("Generating response for prompt: %s", prompt.c_str());
    // tokenize the prompt
    const int n_prompt_tokens = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, is_first, true);
    std::vector<llama_token> prompt_tokens(n_prompt_tokens);
    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), is_first, true) < 0) {
        wxLogError("Failed to tokenize the prompt");
        this->currentMessage->SetStatusMessage("Failed to tokenize the prompt");
        this->UpdateCurrentSession();
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
            wxLogError("Failed to evaluate the prompt");
            this->currentMessage->SetStatusMessage("Failed to evaluate the prompt");
            this->UpdateCurrentSession();
            return "";
        }

        if (llama_decode(ctx, batch)) {
            wxLogError("Failed to decode the prompt");
            this->currentMessage->SetStatusMessage("Failed to decode the prompt");
            this->UpdateCurrentSession();
            return "";
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
            wxLogError("Failed to convert the token to a string");
            this->currentMessage->SetStatusMessage("Failed to convert the token to a string");
            this->UpdateCurrentSession();
            return "";
        }
        std::string piece(buf, n);
        printf("%s", piece.c_str());
        fflush(stdout);
        response += piece;
        this->currentMessage->UpdateOrCreateAssistantAnswer(piece);
        this->UpdateCurrentSession();

        // prepare the next batch with the sampled token
        batch = llama_batch_get_one(&new_token_id, 1);
    }

    return response;
}
void ApplicationLogic::UpdateCurrentSession() {
    this->sharedMemoryManager->write(this->currentMessage->toString());
}
