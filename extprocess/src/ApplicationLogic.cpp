#include "ApplicationLogic.h"

#ifndef STBI_NO_FAILURE_STRINGS
#define STBI_NO_FAILURE_STRINGS
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb_image_write.h"
#endif

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "libs/stb_image_resize.h"

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
    if (sd_ctx) {
        freeSdCtxPtr(sd_ctx);
        sd_ctx = nullptr;
    }
}

bool ApplicationLogic::loadLibrary() {
    try {
        sd_dll.load();

        this->freeSdCtxPtr = (FreeSdCtxFunction)sd_dll.getFunction<FreeSdCtxFunction>("free_sd_ctx");
        if (this->freeSdCtxPtr == nullptr) {
            std::cerr << "Failed to load function: free_sd_ctx" << std::endl;
            this->error = "Failed to load function: free_sd_ctx";
            return false;
        }

        this->txt2imgFuncPtr = (Txt2ImgFunction)sd_dll.getFunction<Txt2ImgFunction>("txt2img");
        if (this->txt2imgFuncPtr == nullptr) {
            std::cerr << "Failed to load function: txt2img" << std::endl;
            this->error = "Failed to load function: txt2img";
            return false;
        }

        this->img2imgFuncPtr = (Img2ImgFunction)sd_dll.getFunction<Img2ImgFunction>("img2img");
        if (this->img2imgFuncPtr == nullptr) {
            std::cerr << "Failed to load function: img2img" << std::endl;
            this->error = "Failed to load function: img2img";
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
        std::cerr << "[EXTPROCESS] Invalid item id: " << item.id << std::endl;
        return;
    }

    this->currentItem             = std::make_shared<QueueItem>(item);
    this->currentItem->started_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    std::cout << "[EXTPROCESS] Processing item: " << this->currentItem->id << std::endl;

    if (this->currentItem->need_sha256 == true) {
        this->sendStatus(QueueStatus::HASHING, QueueEvents::ITEM_MODEL_HASH_START);
        this->currentItem->hash_fullsize = std::filesystem::file_size(this->currentItem->params.model_path.c_str());

        this->currentItem->generated_sha256 = sd_gui_utils::sha256_file_openssl(
            this->currentItem->params.model_path.c_str(),
            (void*)this,
            ApplicationLogic::HandleHashCallback);

        this->sendStatus(QueueStatus::HASHING, QueueEvents::ITEM_MODEL_HASH_DONE, "", EPROCESS_SLEEP_TIME);
    }

    this->sendStatus(QueueStatus::MODEL_LOADING, QueueEvents::ITEM_MODEL_LOAD_START, "", EPROCESS_SLEEP_TIME);
    // on mode convert always return true, because no model loading
    if (this->loadSdModel() == false) {
        if (this->currentItem->mode == SDMode::TXT2IMG || this->currentItem->mode == SDMode::IMG2IMG) {
            std::cerr << "[EXTPROCESS] Failed to load model: " << this->currentItem->params.model_path << std::endl;
            this->currentItem->status_message = _("Failed to load model: ") + this->currentItem->params.model_path;
        }

        if (this->currentItem->mode == SDMode::UPSCALE) {
            std::cerr << "[EXTPROCESS] Failed to load model: " << this->currentItem->params.esrgan_path << std::endl;
            this->currentItem->status_message = _("Failed to load model: ") + this->currentItem->params.esrgan_path;
        }
        this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_MODEL_FAILED);
        this->currentItem = nullptr;
        this->lastItem    = nullptr;
        return;
    }

    // set the steps
    this->currentItem->step = 0;
    this->currentItem->steps = 1;

    this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_MODEL_LOADED, "", EPROCESS_SLEEP_TIME);

    // handle the convert differently
    if (this->currentItem->mode == SDMode::CONVERT) {
        this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_GENERATION_STARTED, "", EPROCESS_SLEEP_TIME);
        bool status = this->convertFuncPtr(this->currentItem->params.model_path.c_str(), this->currentItem->params.vae_path.c_str(), this->currentItem->params.output_path.c_str(), this->currentItem->params.wtype);
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

    std::cout << "[EXTPROCESS] Starting item: " << this->currentItem->id << " type: " << GenerationMode_str.at(this->currentItem->mode) << std::endl;
    switch (this->currentItem->mode) {
        case SDMode::TXT2IMG: {
            std::cout << "[EXTPROCESS] Running txt2img" << std::endl;
            Txt2Img();
        } break;
        case SDMode::IMG2IMG: {
            std::cout << "[EXTPROCESS] Running img2img" << std::endl;
            Img2img();
        } break;
        case SDMode::UPSCALE: {
            std::cout << "[EXTPROCESS] Running upscale" << std::endl;
            Upscale();
        } break;

        default: {
            std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
            this->currentItem->status_message = wxString::Format(_("Unknown mode: %s"), GenerationMode_str.at(this->currentItem->mode).c_str());
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
            std::cerr << this->currentItem->status_message << std::endl;
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
                std::cerr << "[EXTPROCESS] Control image not found: " << this->currentItem->params.control_image_path << std::endl;
            }
        }

        results = txt2imgFuncPtr(
            this->sd_ctx,
            this->currentItem->params.prompt.c_str(),
            this->currentItem->params.negative_prompt.c_str(),
            this->currentItem->params.clip_skip,
            this->currentItem->params.cfg_scale,
            this->currentItem->params.guidance,
            this->currentItem->params.width,
            this->currentItem->params.height,
            this->currentItem->params.sample_method,
            this->currentItem->params.sample_steps,
            this->currentItem->params.seed,
            this->currentItem->params.batch_count,
            control_image,
            this->currentItem->params.strength,
            this->currentItem->params.style_ratio,
            this->currentItem->params.normalize_input,
            this->currentItem->params.input_id_images_path.c_str(),
            this->currentItem->params.skip_layers.data(),
            this->currentItem->params.skip_layers.size(),
            this->currentItem->params.slg_scale,
            this->currentItem->params.skip_layer_start,
            this->currentItem->params.skip_layer_end);

        // free up the control image
        control_image = NULL;
        delete control_image;

        if (results == NULL) {
            std::cout << "[EXTPROCESS] txt2img failed" << std::endl;
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
            return;
        }

        std::cout << "Processing images " << this->currentItem->params.batch_count << std::endl;
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
        std::cerr << "Error calling txt2img: " << e.what() << std::endl;
    }
}

void ApplicationLogic::Img2img() {
    std::cout << std::flush;
    std::cout << "running img2img" << std::endl;
    try {
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME * 2));
        this->sendStatus(QueueStatus::RUNNING, QueueEvents::ITEM_GENERATION_STARTED);
        std::cout << " sent status" << std::endl;

        unsigned char* input_image_buffer   = NULL;
        unsigned char* control_image_buffer = NULL;
        unsigned char* mask_image_buffer    = NULL;
        sd_image_t input_image;
        sd_image_t* control_image = NULL;
        sd_image_t mask_image;
        sd_image_t* results;

        int input_w, input_h, input_c       = 0;
        int control_w, control_h, control_c = 0;

        std::cout << "Checking initial image: " << this->currentItem->initial_image << std::endl;

        if (this->currentItem->initial_image.length() > 0) {
            if (std::filesystem::exists(this->currentItem->initial_image)) {
                input_image_buffer = stbi_load(this->currentItem->initial_image.c_str(), &input_w, &input_h, &input_c, 3);
                input_image        = sd_image_t{(uint32_t)input_w, (uint32_t)input_h, 3, input_image_buffer};
                std::cout << " input image loaded: " << this->currentItem->initial_image << " width: " << input_w << " height: " << input_h << " channels: " << input_c << std::endl;
            } else {
                std::cerr << "Initial image not found: " << this->currentItem->initial_image << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
                this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED, "Initial image not found:" + this->currentItem->initial_image);
                return;
            }
        } else {
            std::cerr << "Missing input image" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED, "Missing input image");
            return;
        }
        if (this->currentItem->mask_image.length() > 0 && std::filesystem::exists(this->currentItem->mask_image)) {
            mask_image_buffer = stbi_load(this->currentItem->mask_image.c_str(), &input_w, &input_h, &input_c, 1);
            std::cout << " mask image loaded: " << this->currentItem->mask_image << " width: " << input_w << " height: " << input_h << " channels: " << input_c << std::endl;
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
                std::cout << " control image loaded: " << this->currentItem->params.control_image_path << std::endl;
            } else {
                std::cerr << "Control image not found: " << this->currentItem->params.control_image_path << std::endl;
                return;
            }
        }

        results = this->img2imgFuncPtr(
            this->sd_ctx,
            input_image,
            mask_image,
            this->currentItem->params.prompt.c_str(),
            this->currentItem->params.negative_prompt.c_str(),
            this->currentItem->params.clip_skip,
            this->currentItem->params.cfg_scale,
            this->currentItem->params.guidance,
            this->currentItem->params.width,
            this->currentItem->params.height,
            this->currentItem->params.sample_method,
            this->currentItem->params.sample_steps,
            this->currentItem->params.strength,
            this->currentItem->params.seed,
            this->currentItem->params.batch_count,
            control_image,
            this->currentItem->params.control_strength,
            this->currentItem->params.style_ratio,
            this->currentItem->params.normalize_input,
            this->currentItem->params.input_id_images_path.c_str(),
            this->currentItem->params.skip_layers.data(),
            this->currentItem->params.skip_layers.size(),
            this->currentItem->params.slg_scale,
            this->currentItem->params.skip_layer_start,
            this->currentItem->params.skip_layer_end);
        // free up the control image

        stbi_image_free(input_image_buffer);
        stbi_image_free(control_image_buffer);
        stbi_image_free(mask_image_buffer);

        control_image = NULL;
        delete control_image;

        if (results == NULL) {
            this->sendStatus(QueueStatus::FAILED, QueueEvents::ITEM_FAILED);
            return;
        }
        for (int i = 0; i < this->currentItem->params.batch_count; i++) {
            if (results[i].data == NULL) {
                continue;
            }
            std::string filepath = this->handleSdImage(results[i]);
            std::cout << "saved tmp image to: " << filepath;
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
        std::cerr << "Error calling img2img: " << e.what() << std::endl;
    }
}

void ApplicationLogic::Upscale() {
    uint8_t* input_image_buffer = NULL;
    sd_image_t results;
    int c = 0;
    int w, h;
    input_image_buffer = stbi_load(this->currentItem->initial_image.c_str(), &w, &h, &c, 3);

    if (input_image_buffer == NULL) {
        std::cerr << "Failed to load image: " << this->currentItem->initial_image << std::endl;
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
        std::cerr << __FILE__ << ":" << __LINE__ << ": image data is null" << std::endl;
        return "";
    }

    if (!std::filesystem::exists(this->tempPath)) {
        std::cerr << __FILE__ << ":" << __LINE__ << ": temp directory does not exist" << std::endl;
        return "";
    }

    std::string filename = this->tempPath + "/" + generateRandomFilename(".png");

    stbi_write_png_compression_level = 0;

    if (stbi_write_png(filename.c_str(), image.width, image.height, 3, image.data, 0) == 0) {
        std::cerr << __FILE__ << ":" << __LINE__ << ": stbi_write_png failed: " << filename.c_str() << std::endl;
        return "";
    }
    if (BUILD_TYPE == "Debug") {
        std::cout << __FILE__ << ":" << __LINE__ << ": saved image to: " << filename << " size: " << image.width << "x" << image.height << " filesize: " << std::filesystem::file_size(filename) << std::endl;
    }

    return filename;
}

bool ApplicationLogic::loadSdModel() {
    // on covert, there is no model loading into ctx, but need to clean up memory
    if (this->currentItem->mode == SDMode::CONVERT) {
        // remove already loaded models
        if (this->sd_ctx != nullptr && this->currentItem->keep_checkpoint_in_memory == false) {
            std::cout << "Freeing up previous sd model" << std::endl;
            this->freeSdCtxPtr(this->sd_ctx);
            this->sd_ctx = nullptr;
        }
        if (this->upscale_ctx != nullptr && this->currentItem->keep_checkpoint_in_memory == false) {
            std::cout << "Freeing up previous upscaler model" << std::endl;
            this->freeUpscalerCtxPtr(this->upscale_ctx);
            this->upscale_ctx = nullptr;
        }
        return true;
    }

    // hnalde upscaler model
    if (this->currentItem->mode == SDMode::UPSCALE) {
        std::cout << "Loading upscale model: " << this->currentItem->params.esrgan_path << std::endl;
        // free up the sd model
        if (this->sd_ctx != nullptr && this->currentItem->keep_checkpoint_in_memory == false) {
            std::cout << "Freeing up previous sd model" << std::endl;
            this->freeSdCtxPtr(this->sd_ctx);
            this->sd_ctx = nullptr;
        }

        // check if we need reload the model
        if (this->lastItem != nullptr && this->lastItem->mode == SDMode::UPSCALE) {
            std::cout << "Previous model is upscale" << std::endl;
            if (this->lastItem->params.esrgan_path != this->currentItem->params.esrgan_path) {
                std::cout << "upscaler model changed" << std::endl;
                if (this->upscale_ctx != nullptr) {
                    this->freeUpscalerCtxPtr(this->upscale_ctx);
                    this->upscale_ctx = nullptr;
                }
                std::cout << "Loading model: " << this->currentItem->params.esrgan_path << std::endl;
                this->upscale_ctx = this->newUpscalerCtxPtr(this->currentItem->params.esrgan_path.c_str(), this->currentItem->params.n_threads, this->currentItem->params.wtype);
                return this->upscale_ctx != NULL;
            }
            std::cout << "upscaler model is already loaded" << std::endl;
            return true;  // already loaded the model
        }
        std::cout << "Loading model: " << this->currentItem->params.esrgan_path << std::endl;
        this->upscale_ctx = this->newUpscalerCtxPtr(this->currentItem->params.esrgan_path.c_str(), this->currentItem->params.n_threads, this->currentItem->params.wtype);
        return this->upscale_ctx != NULL;
    }
    if (this->currentItem->mode == SDMode::TXT2IMG || this->currentItem->mode == SDMode::IMG2IMG) {
        if (this->currentItem->params.model_path == "") {
            std::cout << "Loading sd model: " << this->currentItem->params.diffusion_model_path << std::endl;
        } else {
            std::cout << "Loading sd model: " << this->currentItem->params.model_path << std::endl;
        }
        bool loadModel = true;
        if (this->lastItem != nullptr) {
            std::cout << "Previous model is not null" << std::endl;
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
            std::cout << "Model load required for new item" << std::endl;
            if (this->sd_ctx != nullptr) {
                std::cout << "Freeing sd_ctx" << std::endl;
                this->freeSdCtxPtr(this->sd_ctx);
                this->sd_ctx = nullptr;
            }

            if (this->currentItem->params.model_path.empty()) {
                std::cout << "[EXTPROCESS] Loading model: " << this->currentItem->params.diffusion_model_path << std::endl;
            } else {
                std::cout << "[EXTPROCESS] Loading model: " << this->currentItem->params.model_path << std::endl;
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
                std::cout << "Freeing upscale_ctx" << std::endl;
                this->freeUpscalerCtxPtr(this->upscale_ctx);
                this->upscale_ctx = nullptr;
            }

            this->sd_ctx = this->newSdCtxFuncPtr(
                this->currentItem->params.model_path.c_str(),
                this->currentItem->params.clip_l_path.c_str(),
                this->currentItem->params.clip_g_path.c_str(),
                this->currentItem->params.t5xxl_path.c_str(),
                this->currentItem->params.diffusion_model_path.c_str(),
                this->currentItem->params.vae_path.c_str(),
                this->currentItem->params.taesd_path.c_str(),
                this->currentItem->params.controlnet_path.c_str(),
                this->currentItem->params.lora_model_dir.c_str(),
                this->currentItem->params.embeddings_path.c_str(),
                this->currentItem->params.stacked_id_embeddings_path.c_str(),
                vae_decode_only,  // vae decode only
                this->currentItem->params.vae_tiling,
                false,  // free params immediately
                this->currentItem->params.n_threads,
                this->currentItem->params.wtype,
                this->currentItem->params.rng_type,
                this->currentItem->params.schedule,
                this->currentItem->params.clip_on_cpu,
                this->currentItem->params.control_net_cpu,
                this->currentItem->params.vae_on_cpu,
                this->currentItem->params.diffusion_flash_attn);

            if (this->sd_ctx == NULL) {
                return false;
            }
        }
        return sd_ctx == NULL ? false : true;
    }
    return false;
}