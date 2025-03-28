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

    // load new model
    this->model = this->llama_model_load_from_file(message.model_path.c_str(), params);
    if (!this->model) {
        this->error            = "Failed to load model: " + message.model_path;
        this->model            = nullptr;
        this->modelLoaded      = false;
        this->currentModelPath = "";
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
        this->error            = "";
    }
}

bool ApplicationLogic::loadContext(const sd_gui_utils::llvmMessage& message) {
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx                = message.n_ctx;
    ctx_params.n_batch              = message.n_ctx;

    this->ctx = this->llama_init_from_model(model, ctx_params);

    if (!ctx) {
        std::cerr << "Hiba: Nem sikerült létrehozni a modell kontextust!" << std::endl;
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
    if (this->ctx != nullptr) {
        if (this->smplr) {
            llama_sampler_free(this->smplr);
            this->smplr = nullptr;
        }
        llama_free(ctx);
        ctx = nullptr;
    }
}