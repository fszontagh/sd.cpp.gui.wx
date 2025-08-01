#include "ApplicationLogic.h"
#include "wx/filefn.h"

ApplicationLogic::ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager)
    : sd_dll(libName), sharedMemoryManager(sharedMemoryManager) {
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    auto TempDir   = std::getenv("Temp");
    this->tempPath = std::filesystem::path(TempDir).string();
#else
    this->tempPath = std::filesystem::temp_directory_path().string();
#endif
    sd_dll.addSearchPath(wxGetCwd().ToStdString());
}

ApplicationLogic::~ApplicationLogic() {
    if (sd_ctx) {
        freeSdCtxPtr(sd_ctx);
        sd_ctx = nullptr;
    }
}

bool ApplicationLogic::loadLibrary() {
    try {
        sd_dll.load();

        // Load new API functions
        this->sdCtxParamsInitPtr = (SdCtxParamsInitFunction)sd_dll.getFunction<SdCtxParamsInitFunction>("sd_ctx_params_init");
        if (this->sdCtxParamsInitPtr == nullptr) {
            std::cerr << "Failed to load function: sd_ctx_params_init" << std::endl;
            this->error = "Failed to load function: sd_ctx_params_init";
            return false;
        }

        this->freeSdCtxPtr = (FreeSdCtxFunction)sd_dll.getFunction<FreeSdCtxFunction>("free_sd_ctx");
        if (this->freeSdCtxPtr == nullptr) {
            std::cerr << "Failed to load function: free_sd_ctx" << std::endl;
            this->error = "Failed to load function: free_sd_ctx";
            return false;
        }

        // Load new API generate_image function
        this->generateImageFuncPtr = (GenerateImageFunction)sd_dll.getFunction<GenerateImageFunction>("generate_image");
        if (this->generateImageFuncPtr == nullptr) {
            std::cerr << "Failed to load function: generate_image" << std::endl;
            this->error = "Failed to load function: generate_image";
            return false;
        }

        // Load sd_img_gen_params_init for new API
        this->sdImgGenParamsInitPtr = (SdImgGenParamsInitFunction)sd_dll.getFunction<SdImgGenParamsInitFunction>("sd_img_gen_params_init");
        if (this->sdImgGenParamsInitPtr == nullptr) {
            std::cerr << "Failed to load function: sd_img_gen_params_init" << std::endl;
            this->error = "Failed to load function: sd_img_gen_params_init";
            return false;
        }

        this->upscalerFuncPtr = (UpscalerFunction)sd_dll.getFunction<UpscalerFunction>("upscale");
        if (this->upscalerFuncPtr == nullptr) {
            std::cerr << "Failed to load function: upscaler" << std::endl;
            this->error = "Failed to load function: upscaler";
            return false;
        }

        this->newSdCtxFuncPtr = (NewSdCtxFunction)sd_dll.getFunction<NewSdCtxFunction>("new_sd_ctx");
        if (this->newSdCtxFuncPtr == nullptr) {
            std::cerr << "Failed to load function: new_sd_ctx" << std::endl;
            this->error = "Failed to load function: new_sd_ctx";
            return false;
        }

        this->newUpscalerCtxPtr = (NewUpscalerCtxFunction)sd_dll.getFunction<NewUpscalerCtxFunction>("new_upscaler_ctx");
        if (this->newUpscalerCtxPtr == nullptr) {
            std::cerr << "Failed to load function: new_upscaler_ctx" << std::endl;
            this->error = "Failed to load function: new_upscaler_ctx";
            return false;
        }

        this->convertFuncPtr = (ConvertFunction)sd_dll.getFunction<ConvertFunction>("convert");
        if (this->convertFuncPtr == nullptr) {
            std::cerr << "Failed to load function: convert" << std::endl;
            this->error = "Failed to load function: convert";
            return false;
        }

        this->preprocessCannyFuncPtr = (PreprocessCannyFunction)sd_dll.getFunction<PreprocessCannyFunction>("preprocess_canny");
        if (this->preprocessCannyFuncPtr == nullptr) {
            std::cerr << "Failed to load function: preprocess_canny" << std::endl;
            this->error = "Failed to load function: preprocess_canny";
            return false;
        }

        this->freeUpscalerCtxPtr = (FreeUpscalerCtxFunction)sd_dll.getFunction<FreeUpscalerCtxFunction>("free_upscaler_ctx");
        if (this->freeUpscalerCtxPtr == nullptr) {
            std::cerr << "Failed to load function: free_upscaler_ctx" << std::endl;
            this->error = "Failed to load function: free_upscaler_ctx";
            return false;
        }

        this->sdSetProgressCallbackFuncPtr = (SdSetProgressCallbackFunction)sd_dll.getFunction<SdSetProgressCallbackFunction>("sd_set_progress_callback");
        if (this->sdSetProgressCallbackFuncPtr == nullptr) {
            std::cerr << "Failed to load function: sd_set_progress_callback" << std::endl;
            this->error = "Failed to load function: sd_set_progress_callback";
            return false;
        }

        this->sdSetProgressCallbackFuncPtr(ApplicationLogic::HandleSDProgress, (void*)this);

        this->sdSetLogCallbackFuncPtr = (SdSetLogCallbackFunction)sd_dll.getFunction<SdSetLogCallbackFunction>("sd_set_log_callback");
        if (this->sdSetLogCallbackFuncPtr == nullptr) {
            std::cerr << "Failed to load function: sd_set_log_callback" << std::endl;
            this->error = "Failed to load function: sd_set_log_callback";
            return false;
        }

        this->sdSetLogCallbackFuncPtr(ApplicationLogic::HandleSDLog, (void*)this);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load shared library: " << e.what() << std::endl;
        this->error = "Failed to load shared library: " + sd_dll.getLibraryPath();
        return false;
    }
}

void ApplicationLogic::processMessage(QueueItem& item) {
    if (item.id == 0) {
        wxLogError("Invalid item id: %" PRIu64, item.id);
        return;
    }

    this->currentItem             = std::make_shared<QueueItem>(item);
    this->currentItem->started_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    wxLogInfo("Processing item: %" PRIu64, this->currentItem->id);

    if (this->currentItem->need_sha256 == true) {
        this->sendStatus(QueueStatus::HASHING, QueueEvents::ITEM_MODEL_HASH_START);
        this->currentItem->hash_fullsize = std::filesystem::file_size(this->currentItem->params.model_path.c_str());

        this->currentItem->generated_sha256 = sd_gui_utils::sha256_file_openssl(
            this->currentItem->params.model_path.c_str(),
            (void*)this,
            ApplicationLogic::HandleHashCallback);

        this->sendStatus(QueueStatus::HASHING, QueueEvents::ITEM_MODEL_HASH_DONE, "", EPROCESS_SLEEP_TIME);
    }

    // on mode convert always return true, because no model loading
    if (this->loadSdModel() == false) {
        if (this->currentItem->mode == SDMode::TXT2IMG || this->currentItem->mode == SDMode::IMG2IMG) {
            wxLogError("Failed to load model: %s", this->currentItem->params.model_path.c_str());
            this->currentItem->status_message = _("Failed to load model: ") + this->currentItem->model;
        }

        if (this->currentItem->mode == SDMode::UPSCALE) {
            wxLogError("Failed to load model: %s", this->currentItem->params.esrgan_path.c_str());
        }
        this->sendStatus(QueueStatus::MODEL_FAILED, QueueEvents::ITEM_MODEL_FAILED);
        this->currentItem = nullptr;
        this->lastItem    = nullptr;
        return;
    }

    // set the steps
    this->currentItem->step  = 0;
    this->currentItem->steps = 1;

    this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_MODEL_LOADED, "", EPROCESS_SLEEP_TIME);

    // handle the convert differently
    if (this->currentItem->mode == SDMode::CONVERT) {
        this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_GENERATION_STARTED, "", EPROCESS_SLEEP_TIME);
        bool status = this->convertFuncPtr(this->currentItem->params.model_path.c_str(), this->currentItem->params.vae_path.c_str(), this->currentItem->params.output_path.c_str(), this->currentItem->params.wtype, nullptr);
        if (status == false) {
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
            this->currentItem = nullptr;
            this->lastItem    = nullptr;
            return;
        }
        this->sendStatus(QueueStatus::DONE, QueueEvents::ITEM_FINISHED, "", EPROCESS_SLEEP_TIME);
        this->currentItem = nullptr;
        this->lastItem    = nullptr;
        return;
    }

    wxLogInfo("Starting item: %" PRIu64 " type: %s", this->currentItem->id, GenerationMode_str.at(this->currentItem->mode).c_str());
    switch (this->currentItem->mode) {
        case SDMode::TXT2IMG: {
            wxLogInfo("Running txt2img");
            Txt2Img();
        } break;
        case SDMode::IMG2IMG: {
            wxLogInfo("Running img2img");
            Img2img();
        } break;
        case SDMode::UPSCALE: {
            wxLogInfo("Running upscale");
            Upscale();
        } break;

        default: {
            std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
            this->currentItem->status_message = wxString::Format(_("Unknown mode: %s"), GenerationMode_str.at(this->currentItem->mode).c_str());
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
            wxLogError("Unknown mode: %s", GenerationMode_str.at(this->currentItem->mode).c_str());
            this->lastItem    = nullptr;
            this->currentItem = nullptr;
            return;
        } break;
    }
    this->lastItem    = this->currentItem;
    this->currentItem = nullptr;
}

void ApplicationLogic::Txt2Img() {
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME * 2));
        this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_GENERATION_STARTED);

        sd_image_t* control_image = NULL;
        sd_image_t* results;
        // prepare control image, if we have one
        if (this->currentItem->params.control_image_path.length() > 0) {
            if (std::filesystem::exists(this->currentItem->params.control_image_path)) {
                int c = 0;
                int w, h;
                stbi_uc* input_image_buffer = stbi_load(this->currentItem->params.control_image_path.c_str(), &w, &h, &c, 3);
                control_image               = new sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
                input_image_buffer          = NULL;
                delete input_image_buffer;
            } else {
                wxLogError("Control image not found: %s", this->currentItem->params.control_image_path.c_str());
            }
        }

        // Use new API
        sd_img_gen_params_t gen_params;
        this->sdImgGenParamsInitPtr(&gen_params);

        gen_params.prompt                      = this->currentItem->params.prompt.c_str();
        gen_params.negative_prompt             = this->currentItem->params.negative_prompt.c_str();
        gen_params.clip_skip                   = this->currentItem->params.clip_skip;
        gen_params.guidance.txt_cfg            = this->currentItem->params.cfg_scale;
        gen_params.guidance.img_cfg            = this->currentItem->params.guidance;
        gen_params.guidance.min_cfg            = this->currentItem->params.min_cfg;
        gen_params.guidance.distilled_guidance = 0.0f;
        gen_params.guidance.slg.layers         = this->currentItem->params.skip_layers.data();
        gen_params.guidance.slg.layer_count    = this->currentItem->params.skip_layers.size();
        gen_params.guidance.slg.layer_start    = this->currentItem->params.skip_layer_start;
        gen_params.guidance.slg.layer_end      = this->currentItem->params.skip_layer_end;
        gen_params.guidance.slg.scale          = this->currentItem->params.slg_scale;
        gen_params.init_image                  = {0, 0, 0, nullptr};
        gen_params.ref_images                  = nullptr;
        gen_params.ref_images_count            = 0;
        gen_params.mask_image                  = {0, 0, 0, nullptr};
        gen_params.width                       = this->currentItem->params.width;
        gen_params.height                      = this->currentItem->params.height;
        gen_params.sample_method               = this->currentItem->params.sample_method;
        gen_params.sample_steps                = this->currentItem->params.sample_steps;
        gen_params.eta                         = this->currentItem->params.eta;
        gen_params.strength                    = this->currentItem->params.strength;
        gen_params.seed                        = this->currentItem->params.seed;
        gen_params.batch_count                 = this->currentItem->params.batch_count;
        gen_params.control_cond                = control_image;
        gen_params.control_strength            = this->currentItem->params.control_strength;
        gen_params.style_strength              = this->currentItem->params.style_ratio;
        gen_params.normalize_input             = this->currentItem->params.normalize_input;
        gen_params.input_id_images_path        = this->currentItem->params.input_id_images_path.c_str();

        results = this->generateImageFuncPtr(this->sd_ctx, &gen_params);

        // free up the control image
        control_image = NULL;
        delete control_image;

        if (results == NULL) {
            wxLogError("Failed to generate images");
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
            return;
        }

        wxLogInfo("Generated %d images in batch", this->currentItem->params.batch_count);
        for (int i = 0; i < this->currentItem->params.batch_count; i++) {
            if (results[i].data == NULL) {
                continue;
            }
            std::string filepath = this->handleSdImage(results[i]);
            this->currentItem->rawImages.push_back(filepath);
            free(results[i].data);
            results[i].data = NULL;
        }

        auto finished_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME * 2));
        this->currentItem->finished_at = finished_at;
        this->sendStatus(QueueStatus::DONE, QueueEvents::ITEM_FINISHED);
        delete results;

    } catch (const std::exception& e) {
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
        this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
        wxLogError("Error calling txt2img: '%s'", e.what());
    }
}

void ApplicationLogic::Img2img() {
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME * 2));
        this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_GENERATION_STARTED);
        unsigned char* input_image_buffer   = NULL;
        unsigned char* control_image_buffer = NULL;
        unsigned char* mask_image_buffer    = NULL;
        sd_image_t input_image;
        sd_image_t* control_image = NULL;
        sd_image_t mask_image;
        sd_image_t* results;

        int input_w, input_h, input_c       = 0;
        int control_w, control_h, control_c = 0;

        wxLogInfo("Checking initial image: %s", this->currentItem->initial_image.c_str());

        if (this->currentItem->initial_image.length() > 0) {
            if (std::filesystem::exists(this->currentItem->initial_image)) {
                input_image_buffer = stbi_load(this->currentItem->initial_image.c_str(), &input_w, &input_h, &input_c, 3);
                input_image        = sd_image_t{(uint32_t)input_w, (uint32_t)input_h, 3, input_image_buffer};
                wxLogInfo(" input image loaded: %s width: %d height: %d channels: %d", this->currentItem->initial_image.c_str(), input_w, input_h, input_c);
            } else {
                wxLogError("Initial image not found: '%s'", this->currentItem->initial_image.c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
                this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED, "Initial image not found:" + this->currentItem->initial_image);
                return;
            }
        } else {
            wxLogError("Missing input image: '%s'", this->currentItem->initial_image.c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED, "Missing input image");
            return;
        }
        if (this->currentItem->mask_image.length() > 0 && std::filesystem::exists(this->currentItem->mask_image)) {
            mask_image_buffer = stbi_load(this->currentItem->mask_image.c_str(), &input_w, &input_h, &input_c, 1);
            wxLogInfo(" - mask image loaded: %s width: %d height: %d channels: %d", this->currentItem->mask_image.c_str(), input_w, input_h, input_c);
        } else {
            std::vector<uint8_t> arr(this->currentItem->params.width * this->currentItem->params.height, 255);
            mask_image_buffer = arr.data();
        }
        mask_image = sd_image_t{(uint32_t)input_w, (uint32_t)input_h, 3, mask_image_buffer};

        // prepare control image, if we have one
        if (this->currentItem->params.control_image_path.length() > 0) {
            std::cout << "Checking control image: " << this->currentItem->params.control_image_path << std::endl;
            if (std::filesystem::exists(this->currentItem->params.control_image_path)) {
                control_image_buffer = stbi_load(this->currentItem->params.control_image_path.c_str(), &control_w, &control_h, &control_c, 3);
                control_image        = new sd_image_t{(uint32_t)control_w, (uint32_t)control_h, 3, control_image_buffer};
                wxLogInfo(" - control image loaded: %s width: %d height: %d channels: %d", this->currentItem->params.control_image_path.c_str(), control_w, control_h, control_c);
            } else {
                wxLogError("Control image not found: '%s'", this->currentItem->params.control_image_path.c_str());
                this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED, "Missing controlnet image");
                return;
            }
        }

        // Use new API
        sd_img_gen_params_t gen_params;
        this->sdImgGenParamsInitPtr(&gen_params);

        gen_params.prompt                      = this->currentItem->params.prompt.c_str();
        gen_params.negative_prompt             = this->currentItem->params.negative_prompt.c_str();
        gen_params.clip_skip                   = this->currentItem->params.clip_skip;
        gen_params.guidance.txt_cfg            = this->currentItem->params.cfg_scale;
        gen_params.guidance.img_cfg            = this->currentItem->params.guidance;
        gen_params.guidance.min_cfg            = this->currentItem->params.min_cfg;
        gen_params.guidance.distilled_guidance = 0.0f;
        gen_params.guidance.slg.layers         = this->currentItem->params.skip_layers.data();
        gen_params.guidance.slg.layer_count    = this->currentItem->params.skip_layers.size();
        gen_params.guidance.slg.layer_start    = this->currentItem->params.skip_layer_start;
        gen_params.guidance.slg.layer_end      = this->currentItem->params.skip_layer_end;
        gen_params.guidance.slg.scale          = this->currentItem->params.slg_scale;
        gen_params.init_image                  = input_image;
        gen_params.ref_images                  = nullptr;
        gen_params.ref_images_count            = 0;
        gen_params.mask_image                  = mask_image;
        gen_params.width                       = this->currentItem->params.width;
        gen_params.height                      = this->currentItem->params.height;
        gen_params.sample_method               = this->currentItem->params.sample_method;
        gen_params.sample_steps                = this->currentItem->params.sample_steps;
        gen_params.eta                         = this->currentItem->params.eta;
        gen_params.strength                    = this->currentItem->params.strength;
        gen_params.seed                        = this->currentItem->params.seed;
        gen_params.batch_count                 = this->currentItem->params.batch_count;
        gen_params.control_cond                = control_image;
        gen_params.control_strength            = this->currentItem->params.control_strength;
        gen_params.style_strength              = this->currentItem->params.style_ratio;
        gen_params.normalize_input             = this->currentItem->params.normalize_input;
        gen_params.input_id_images_path        = this->currentItem->params.input_id_images_path.c_str();

        results = this->generateImageFuncPtr(this->sd_ctx, &gen_params);
        // free up the control image

        stbi_image_free(input_image_buffer);
        stbi_image_free(control_image_buffer);
        stbi_image_free(mask_image_buffer);

        control_image = NULL;
        delete control_image;

        if (results == NULL) {
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
            wxLogError("img2img error, results is null");
            return;
        }
        for (int i = 0; i < this->currentItem->params.batch_count; i++) {
            if (results[i].data == NULL) {
                continue;
            }
            std::string filepath = this->handleSdImage(results[i]);
            wxLogInfo("Saved tmp image to: '%s'", filepath.c_str());
            this->currentItem->rawImages.push_back(filepath);
            free(results[i].data);
            results[i].data = NULL;
        }

        auto finished_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME * 2));
        this->currentItem->finished_at = finished_at;
        this->sendStatus(QueueStatus::DONE, QueueEvents::ITEM_FINISHED);

        delete results;

    } catch (const std::exception& e) {
        this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
        wxLogError("Error calling img2img: '%s'", e.what());
    }
}

void ApplicationLogic::Upscale() {
    uint8_t* input_image_buffer = NULL;
    sd_image_t results;
    int c = 0;
    int w, h;
    input_image_buffer = stbi_load(this->currentItem->initial_image.c_str(), &w, &h, &c, 3);

    if (input_image_buffer == NULL) {
        wxLogError("Failed to load initial image: '%s'", this->currentItem->initial_image.c_str());
        this->currentItem->status_message = _("Failed to load image: ") + this->currentItem->initial_image;
        this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
        return;
    }
    sd_image_t control_image = sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};

    std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
    this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_GENERATION_STARTED);

    results = this->upscalerFuncPtr(this->upscale_ctx, control_image, this->currentItem->upscale_factor);

    if (results.data == NULL) {
        this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
        stbi_image_free(input_image_buffer);
        wxLogError("upscale error, results is null");
        return;
    }

    std::string filepath = this->handleSdImage(results);

    this->currentItem->rawImages.push_back(filepath);
    free(results.data);
    results.data = NULL;

    auto finished_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME * 2));
    this->currentItem->finished_at = finished_at;
    this->sendStatus(QueueStatus::DONE, QueueEvents::ITEM_FINISHED);
    stbi_image_free(input_image_buffer);
}

std::string ApplicationLogic::handleSdImage(sd_image_t& image) {
    if (image.data == nullptr) {
        wxLogError("image data is null");
        return "";
    }

    // Validate image dimensions
    if (image.width <= 0 || image.height <= 0) {
        wxLogError("invalid image dimensions: %dx%d", image.width, image.height);
        return "";
    }

    // Validate image data (check if all pixels are black/transparent)
    bool allPixelsBlack = true;
    for (int i = 0; i < image.width * image.height * 3; i++) {
        if (image.data[i] != 0) {
            allPixelsBlack = false;
            break;
        }
    }
    if (allPixelsBlack) {
        wxLogError("image data is all black/transparent: %dx%d", image.width, image.height);
        return "";
    }

    if (!std::filesystem::exists(this->tempPath)) {
        wxLogError("temp directory does not exist: '%s'", this->tempPath.c_str());
        return "";
    }

    std::string filename = this->tempPath + "/" + generateRandomFilename(".png");

    stbi_write_png_compression_level = 0;

    if (stbi_write_png(filename.c_str(), image.width, image.height, 3, image.data, 0) == 0) {
        wxLogError("stbi_write_png failed: '%s'", filename.c_str());
        return "";
    }

    // Verify that the file was actually written and has a reasonable size
    if (!std::filesystem::exists(filename)) {
        wxLogError("image file was not created: '%s'", filename.c_str());
        return "";
    }

    auto filesize = std::filesystem::file_size(filename);
    if (filesize == 0) {
        wxLogError("image file is empty: '%s'", filename.c_str());
        std::filesystem::remove(filename);
        return "";
    }

    wxLogInfo("saved image to: '%s' size: %dx%d filesize: %" PRIu64, filename.c_str(), image.width, image.height, filesize);
    return filename;
}

bool ApplicationLogic::loadSdModel() {
    // on covert, there is no model loading into ctx, but need to clean up memory
    if (this->currentItem->mode == SDMode::CONVERT) {
        // remove already loaded models
        if (this->sd_ctx != nullptr && this->currentItem->keep_checkpoint_in_memory == false) {
            this->sendStatus(QueueStatus::MODEL_LOADING, QueueEvents::ITEM_MODEL_LOAD_START, "", EPROCESS_SLEEP_TIME);
            wxLogInfo("Freeing up previous sd model");
            this->freeSdCtxPtr(this->sd_ctx);
            this->sd_ctx = nullptr;
        }
        if (this->upscale_ctx != nullptr && this->currentItem->keep_checkpoint_in_memory == false) {
            this->sendStatus(QueueStatus::MODEL_LOADING, QueueEvents::ITEM_MODEL_LOAD_START, "", EPROCESS_SLEEP_TIME);
            wxLogInfo("Freeing up previous upscaler model");
            this->freeUpscalerCtxPtr(this->upscale_ctx);
            this->upscale_ctx = nullptr;
        }
        return true;
    }

    // handle upscaler model
    if (this->currentItem->mode == SDMode::UPSCALE) {
        wxLogInfo("Loading upscale model: %s", this->currentItem->params.esrgan_path.c_str());
        this->sendStatus(QueueStatus::MODEL_LOADING, QueueEvents::ITEM_MODEL_LOAD_START, "", EPROCESS_SLEEP_TIME);
        // free up the sd model
        if (this->sd_ctx != nullptr && this->currentItem->keep_checkpoint_in_memory == false) {
            wxLogInfo("Freeing up previous sd model");
            this->freeSdCtxPtr(this->sd_ctx);
            this->sd_ctx = nullptr;
        }

        // check if we need reload the model
        if (this->lastItem != nullptr && this->lastItem->mode == SDMode::UPSCALE) {
            wxLogInfo("Previous model is upscale");
            if (this->lastItem->params.esrgan_path != this->currentItem->params.esrgan_path) {
                wxLogInfo("upscaler model changed");
                if (this->upscale_ctx != nullptr) {
                    this->freeUpscalerCtxPtr(this->upscale_ctx);
                    this->upscale_ctx = nullptr;
                }
                wxLogInfo("Loading upscaler model: %s", this->currentItem->params.esrgan_path.c_str());
                this->upscale_ctx = this->newUpscalerCtxPtr(this->currentItem->params.esrgan_path.c_str(), this->currentItem->params.n_threads);
                return this->upscale_ctx != NULL;
            }
            wxLogInfo("upscaler model is the same");
            return true;  // already loaded the model
        }
        wxLogInfo("Loading upscaler model: '%s'", this->currentItem->params.esrgan_path.c_str());
        this->upscale_ctx = this->newUpscalerCtxPtr(this->currentItem->params.esrgan_path.c_str(), this->currentItem->params.n_threads);
        return this->upscale_ctx != NULL;
    }
    if (this->currentItem->mode == SDMode::TXT2IMG || this->currentItem->mode == SDMode::IMG2IMG) {
        if (this->currentItem->params.model_path.empty() && this->currentItem->params.model_path.empty()) {
            wxLogError("Empty model path");
            this->currentItem->status_message = _("Empty model path!");
            return false;
        }
        if (this->currentItem->params.model_path == "") {
            wxLogInfo("Loading sd model: %s", this->currentItem->params.diffusion_model_path.c_str());
        } else {
            wxLogInfo("Loading sd model: %s", this->currentItem->params.model_path.c_str());
        }
        bool loadModel = true;
        if (this->lastItem != nullptr) {
            wxLogInfo("Previous model: '%s'", this->lastItem->params.model_path.c_str());
            if (this->lastItem->params.model_path.empty() && this->currentItem->params.model_path.empty()) {
                if (this->currentItem->params.diffusion_model_path == this->lastItem->params.diffusion_model_path) {
                    loadModel = false;
                }
            }

            if (this->lastItem->params.diffusion_model_path != this->currentItem->params.diffusion_model_path) {
                loadModel = true;
            }

            // prev model was empty (eg flux) new model non empty, we need to reload
            if (this->lastItem->params.model_path.empty() && !this->currentItem->params.model_path.empty()) {
                loadModel = true;
            }

            // prev model and new model the same path, no need to reload
            if (this->lastItem->params.model_path == this->currentItem->params.model_path) {
                loadModel = false;
            }

            if (this->lastItem->mode != this->currentItem->mode) {
                loadModel = true;
            }

            if (this->lastItem->params.clip_l_path != this->currentItem->params.clip_l_path) {
                loadModel = true;
            }

            if (this->lastItem->params.clip_g_path != this->currentItem->params.clip_g_path) {
                loadModel = true;
            }

            if (this->lastItem->params.t5xxl_path != this->currentItem->params.t5xxl_path) {
                loadModel = true;
            }

            if (this->lastItem->params.diffusion_model_path != this->currentItem->params.diffusion_model_path) {
                loadModel = true;
            }

            if (this->lastItem->params.vae_path != this->currentItem->params.vae_path) {
                loadModel = true;
            }

            if (this->lastItem->params.taesd_path != this->currentItem->params.taesd_path) {
                loadModel = true;
            }

            if (this->lastItem->params.controlnet_path != this->currentItem->params.controlnet_path) {
                loadModel = true;
            }

            if (this->lastItem->params.lora_model_dir != this->currentItem->params.lora_model_dir) {
                loadModel = true;
            }

            if (this->lastItem->params.embeddings_path != this->currentItem->params.embeddings_path) {
                loadModel = true;
            }

            if (this->lastItem->params.stacked_id_embeddings_path != this->currentItem->params.stacked_id_embeddings_path) {
                loadModel = true;
            }

            if (this->lastItem->params.vae_tiling != this->currentItem->params.vae_tiling) {
                loadModel = true;
            }

            if (this->lastItem->params.n_threads != this->currentItem->params.n_threads) {
                loadModel = true;
            }

            if (this->lastItem->params.wtype != this->currentItem->params.wtype) {
                loadModel = true;
            }

            if (this->lastItem->params.rng_type != this->currentItem->params.rng_type) {
                loadModel = true;
            }

            if (this->lastItem->params.schedule != this->currentItem->params.schedule) {
                loadModel = true;
            }
            if (this->lastItem->params.clip_on_cpu != this->currentItem->params.clip_on_cpu) {
                loadModel = true;
            }
            if (this->lastItem->params.control_net_cpu != this->currentItem->params.control_net_cpu) {
                loadModel = true;
            }
            if (this->lastItem->params.vae_on_cpu != this->currentItem->params.vae_on_cpu) {
                loadModel = true;
            }
        }

        if (loadModel == true) {
            this->sendStatus(QueueStatus::MODEL_LOADING, QueueEvents::ITEM_MODEL_LOAD_START, "", EPROCESS_SLEEP_TIME);
            wxLogInfo("Loading sd model: %s", this->currentItem->params.model_path.c_str());
            if (this->sd_ctx != nullptr) {
                wxLogInfo("Freeing sd_ctx");
                this->freeSdCtxPtr(this->sd_ctx);
                this->sd_ctx = nullptr;
            }

            if (this->currentItem->params.model_path.empty()) {
                wxLogInfo("Loading model: '%s'", this->currentItem->params.diffusion_model_path.c_str());
            } else {
                wxLogInfo("Loading model: '%s'", this->currentItem->params.model_path.c_str());
            }
            bool vae_decode_only = true;

            switch (this->currentItem->mode) {
                case SDMode::IMG2IMG:
                    // case SDMode::IMG2VID:
                    vae_decode_only = false;
                    break;
                case SDMode::TXT2IMG:
                    vae_decode_only = true;
                    break;
                default:
                    break;
            }

            if (this->upscale_ctx != nullptr && this->currentItem->keep_upscaler_in_memory == false) {
                wxLogInfo("Freeing upscale_ctx");
                this->freeUpscalerCtxPtr(this->upscale_ctx);
                this->upscale_ctx = nullptr;
            }

            // Use new API with sd_ctx_params_t
            sd_ctx_params_t ctx_params;
            this->sdCtxParamsInitPtr(&ctx_params);

            ctx_params.model_path              = this->currentItem->params.model_path.c_str();
            ctx_params.clip_l_path             = this->currentItem->params.clip_l_path.c_str();
            ctx_params.clip_g_path             = this->currentItem->params.clip_g_path.c_str();
            ctx_params.t5xxl_path              = this->currentItem->params.t5xxl_path.c_str();
            ctx_params.diffusion_model_path    = this->currentItem->params.diffusion_model_path.c_str();
            ctx_params.vae_path                = this->currentItem->params.vae_path.c_str();
            ctx_params.taesd_path              = this->currentItem->params.taesd_path.c_str();
            ctx_params.control_net_path        = this->currentItem->params.controlnet_path.c_str();
            ctx_params.lora_model_dir          = this->currentItem->params.lora_model_dir.c_str();
            ctx_params.embedding_dir           = this->currentItem->params.embeddings_path.c_str();
            ctx_params.stacked_id_embed_dir    = this->currentItem->params.stacked_id_embeddings_path.c_str();
            ctx_params.vae_decode_only         = vae_decode_only;
            ctx_params.vae_tiling              = this->currentItem->params.vae_tiling;
            ctx_params.free_params_immediately = false;
            ctx_params.n_threads               = this->currentItem->params.n_threads;
            ctx_params.wtype                   = this->currentItem->params.wtype;
            ctx_params.rng_type                = this->currentItem->params.rng_type;
            ctx_params.schedule                = this->currentItem->params.schedule;
            ctx_params.keep_clip_on_cpu        = this->currentItem->params.clip_on_cpu;
            ctx_params.keep_control_net_on_cpu = this->currentItem->params.control_net_cpu;
            ctx_params.keep_vae_on_cpu         = this->currentItem->params.vae_on_cpu;
            ctx_params.diffusion_flash_attn    = this->currentItem->params.diffusion_flash_attn;
            ctx_params.chroma_use_dit_mask     = false;
            ctx_params.chroma_use_t5_mask      = false;
            ctx_params.chroma_t5_mask_pad      = 0;

            this->sd_ctx = this->newSdCtxFuncPtr(&ctx_params);

            if (this->sd_ctx == NULL) {
                return false;
            }
            this->sendStatus(QueueStatus::MODEL_LOADING, QueueEvents::ITEM_MODEL_LOADED, "", EPROCESS_SLEEP_TIME);
        }
        return sd_ctx == NULL ? false : true;
    }
    return false;
}