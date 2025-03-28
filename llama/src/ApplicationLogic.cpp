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
        this->llama_init_from_model = sd_dll.getFunction<LlamaInitFromModelFunction>("llama_init_from_model");
        if (this->llama_init_from_model == nullptr) {
            std::cerr << "Failed to load function: llama_init_from_model" << std::endl;
            this->error = "Failed to load function: llama_init_from_model";
            return false;
        }
        this->llama_free = sd_dll.getFunction<LlamaFreeFunction>("llama_free");
        if (this->llama_free == nullptr) {
            std::cerr << "Failed to load function: llama_free" << std::endl;
            this->error = "Failed to load function: llama_free";
            return false;
        }
        this->llama_model_load_from_file = sd_dll.getFunction<LlamaModelLoadFromFile>("llama_model_load_from_file");
        if (this->llama_model_load_from_file == nullptr) {
            std::cerr << "Failed to load function: llama_model_load_from_file" << std::endl;
            this->error = "Failed to load function: llama_model_load_from_file";
            return false;
        }
        this->llama_model_free = sd_dll.getFunction<LlamaModelFreeFunction>("llama_model_free");
        if (this->llama_model_free == nullptr) {
            std::cerr << "Failed to load function: llama_model_free" << std::endl;
            this->error = "Failed to load function: llama_model_free";
            return false;
        }
        this->llama_backend_free = sd_dll.getFunction<LlamaBackendFreeFunction>("llama_backend_free");
        if (this->llama_backend_free == nullptr) {
            std::cerr << "Failed to load function: llama_backend_free" << std::endl;
            this->error = "Failed to load function: llama_backend_free";
            return false;
        }
        this->llama_sampler_chain_init = sd_dll.getFunction<LlamaSamplerChainInitFunction>("llama_sampler_chain_init");
        if (this->llama_sampler_chain_init == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_chain_init" << std::endl;
            this->error = "Failed to load function: llama_sampler_chain_init";
            return false;
        }
        this->llama_sampler_chain_default_params = sd_dll.getFunction<LlamaSamplerChainDefaultParams>("llama_sampler_chain_default_params");
        if (this->llama_sampler_chain_default_params == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_chain_default_params" << std::endl;
            this->error = "Failed to load function: llama_sampler_chain_default_params";
            return false;
        }
        this->llama_context_default_params = sd_dll.getFunction<LlamaContextDefaultParams>("llama_context_default_params");
        if (this->llama_context_default_params == nullptr) {
            std::cerr << "Failed to load function: llama_context_default_params" << std::endl;
            this->error = "Failed to load function: llama_context_default_params";
            return false;
        }
        this->llama_model_default_params = sd_dll.getFunction<LlamaModelDefaultParams>("llama_model_default_params");
        if (this->llama_model_default_params == nullptr) {
            std::cerr << "Failed to load function: llama_model_default_params" << std::endl;
            this->error = "Failed to load function: llama_model_default_params";
            return false;
        }
        this->llama_sampler_init_min_p = sd_dll.getFunction<LlamaSamplerInitMinP>("llama_sampler_init_min_p");
        if (this->llama_sampler_init_min_p == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_init_min_p" << std::endl;
            this->error = "Failed to load function: llama_sampler_init_min_p";
            return false;
        }
        this->llama_sampler_chain_add = sd_dll.getFunction<LlamaSamplerChainAdd>("llama_sampler_chain_add");
        if (this->llama_sampler_chain_add == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_chain_add" << std::endl;
            this->error = "Failed to load function: llama_sampler_chain_add";
            return false;
        }
        this->llama_sampler_init_temp = sd_dll.getFunction<LlamaSamplerInitTemp>("llama_sampler_init_temp");
        if (this->llama_sampler_init_temp == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_init_temp" << std::endl;
            this->error = "Failed to load function: llama_sampler_init_temp";
            return false;
        }
        this->llama_sampler_init_dist = sd_dll.getFunction<LlamaSamplerInitDist>("llama_sampler_init_dist");
        if (this->llama_sampler_init_dist == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_init_dist" << std::endl;
            this->error = "Failed to load function: llama_sampler_init_dist";
            return false;
        }
        this->llama_sampler_free = sd_dll.getFunction<LlamaSamplerFree>("llama_sampler_free");
        if (this->llama_sampler_free == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_free" << std::endl;
            this->error = "Failed to load function: llama_sampler_free";
            return false;
        }

        this->llama_model_get_vocab = sd_dll.getFunction<LlamaModelGetVocab>("llama_model_get_vocab");
        if (this->llama_model_get_vocab == nullptr) {
            std::cerr << "Failed to load function: llama_model_get_vocab" << std::endl;
            this->error = "Failed to load function: llama_model_get_vocab";
            return false;
        }

        this->llama_batch_get_one = sd_dll.getFunction<LlamaBatchGetOne>("llama_batch_get_one");
        if (this->llama_batch_get_one == nullptr) {
            std::cerr << "Failed to load function: llama_batch_get_one" << std::endl;
            this->error = "Failed to load function: llama_batch_get_one";
            return false;
        }

        this->llama_tokenize = sd_dll.getFunction<LlamaTokenize>("llama_tokenize");
        if (this->llama_tokenize == nullptr) {
            std::cerr << "Failed to load function: llama_tokenize" << std::endl;
            this->error = "Failed to load function: llama_tokenize";
            return false;
        }
        this->llama_vocab_is_eog = sd_dll.getFunction<LlamaVocabIsEog>("llama_vocab_is_eog");
        if (this->llama_vocab_is_eog == nullptr) {
            std::cerr << "Failed to load function: llama_vocab_is_eog" << std::endl;
            this->error = "Failed to load function: llama_vocab_is_eog";
            return false;
        }

        this->llama_decode = sd_dll.getFunction<LlamaDecode>("llama_decode");
        if (this->llama_decode == nullptr) {
            std::cerr << "Failed to load function: llama_decode" << std::endl;
            this->error = "Failed to load function: llama_decode";
            return false;
        }

        this->llama_sampler_sample = sd_dll.getFunction<LlamaSamplerSample>("llama_sampler_sample");
        if (this->llama_sampler_sample == nullptr) {
            std::cerr << "Failed to load function: llama_sampler_sample" << std::endl;
            this->error = "Failed to load function: llama_sampler_sample";
            return false;
        }

        this->llama_token_to_piece = sd_dll.getFunction<LlamaTokenToPiece>("llama_token_to_piece");
        if (this->llama_token_to_piece == nullptr) {
            std::cerr << "Failed to load function: llama_token_to_piece" << std::endl;
            this->error = "Failed to load function: llama_token_to_piece";
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load shared library: " << e.what() << std::endl;
        this->error = "Failed to load shared library: " + sd_dll.getLibraryPath();
        return false;
    }
}
void ApplicationLogic::processMessage(sd_gui_utils::llvmMessage& message) {
    switch (message.command) {
        case sd_gui_utils::MODEL_LOAD: {
            if (!this->loadModel(message)) {
                message.status_message = this->error;
                message.status         = sd_gui_utils::llvmstatus::ERROR;
            }
        } break;
        case sd_gui_utils::MODEL_UNLOAD: {
            this->unloadModel();
        } break;
        case sd_gui_utils::GENERATE_TEXT: {
            if (message.model_path != this->currentModelPath) {
                this->loadModel(message);
                if (!this->ctx) {
                    this->loadContext(message);
                }
            }
            std::string response;
            this->generateText(message, response);
            message.response = response;
            try {
                nlohmann::json j        = nlohmann::json(message);
                std::string json_string = j.dump();
                this->sharedMemoryManager->write(json_string.c_str(), json_string.size());
            } catch (const std::exception& e) {
                message.status_message = e.what();
                message.status         = sd_gui_utils::llvmstatus::ERROR;
            }

        } break;
        case sd_gui_utils::GENERATE_TEXT_STREAM: {
        } break;
    }
};
bool ApplicationLogic::loadModel(const sd_gui_utils::llvmMessage& message) {
    // unload previous model
    if (this->model != nullptr && this->currentModelPath != message.model_path) {
        llama_model_free(model);
        this->model = nullptr;
    }
    this->currentModelPath    = message.model_path;
    llama_model_params params = llama_model_default_params();
    params.n_gpu_layers       = message.ngl;
    std::cout << "Loading model: " << message.model_path << std::endl;

    // load new model
    this->model = this->llama_model_load_from_file(message.model_path.c_str(), params);
    if (!this->model) {
        this->error            = "Failed to load model: " + message.model_path;
        this->model            = nullptr;
        this->modelLoaded      = false;
        this->currentModelPath = "";
        std::cerr << this->error << std::endl;
        return false;
    }
    std::cout << "Loading vocab... " << std::endl;
    this->vocab = this->llama_model_get_vocab(this->model);
    return true;
};

void ApplicationLogic::unloadModel() {
    if (this->model != nullptr) {
        llama_model_free(model);
        this->model            = nullptr;
        this->modelLoaded      = false;
        this->currentModelPath = "";
        this->error            = "";
    }
}

bool ApplicationLogic::loadContext(const sd_gui_utils::llvmMessage& message) {
    if (!this->model) {
        this->error = "Model not loaded";
        return false;
    }
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx                = message.n_ctx;
    ctx_params.n_batch              = message.n_ctx;

    this->ctx = this->llama_init_from_model(model, ctx_params);

    if (!this->ctx) {
        this->error = "Failed to load context: " + message.model_path;
        std::cerr << this->error << std::endl;
        this->llama_model_free(model);
        this->llama_backend_free();
        return false;
    }

    this->smplr = this->llama_sampler_chain_init(llama_sampler_chain_default_params());
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
void ApplicationLogic::generateText(const sd_gui_utils::llvmMessage& message, std::string& response) {
    if (!this->model) {
        this->error = "Model is null";
        return;
    }
    if (!this->ctx) {
        this->error = "Context is null";
        return;
    }
    if (!this->smplr) {
        this->error = "Sampler is null";
        return;
    }

    const int n_prompt = -llama_tokenize(this->vocab, message.prompt.c_str(), message.prompt.size(), NULL, 0, true, true);

    std::vector<llama_token> prompt_tokens(n_prompt);
    if (llama_tokenize(vocab, message.prompt.c_str(), message.prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0) {
        this->error = "Failed to tokenize prompt";
        std::cerr << this->error << std::endl;
        return;
    }

    for (auto id : prompt_tokens) {
        char buf[128];
        int n = llama_token_to_piece(vocab, id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
            return;
        }
        std::string s(buf, n);
        printf("%s", s.c_str());
    }

    llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

    int n_decode = 0;
    llama_token new_token_id;

    for (int n_pos = 0; n_pos + batch.n_tokens < n_prompt + message.n_ctx;) {
        if (llama_decode(this->ctx, batch)) {
            this->error = "Failed to eval";
            return;
        }

        n_pos += batch.n_tokens;

        // sample the next token
        {
            new_token_id = llama_sampler_sample(this->smplr, this->ctx, -1);

            // is it an end of generation?
            if (llama_vocab_is_eog(vocab, new_token_id)) {
                break;
            }

            char buf[128];
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0) {
                this->error = "Failed to convert token to piece";
                return;
            }
            std::string s(buf, n);
            response.append(s);

            // prepare the next batch with the sampled token
            batch = llama_batch_get_one(&new_token_id, 1);

            n_decode += 1;
        }
    }
    std::cout << "Generated text: " << response << std::endl;
}