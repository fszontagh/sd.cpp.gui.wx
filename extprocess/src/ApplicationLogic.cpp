#include "ApplicationLogic.h"
#include <cstdlib>
#include <iostream>
#include "ui/QueueManager.h"
#include "wx/strvararg.h"
#include "wx/translation.h"
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
    : sd_dll(libName), sharedMemoryManager(sharedMemoryManager) {}

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

void ApplicationLogic::processMessage(QM::QueueItem& item) {
    if (item.id == 0) {
        std::cerr << "[EXTPROCESS] Invalid item id: " << item.id << std::endl;
        return;
    }

    this->currentItem = std::make_shared<QM::QueueItem>(item);

    this->currentItem->status     = QM::QueueStatus::RUNNING;
    this->currentItem->event      = QM::QueueEvents::ITEM_MODEL_LOAD_START;
    this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    this->currentItem->started_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    nlohmann::json j              = *this->currentItem;
    std::string jsonString        = j.dump();
    this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

    std::cout << "[EXTPROCESS] Processing item: " << this->currentItem->id << std::endl;

    if (this->loadSdModel(this->currentItem) == false) {
        if (this->currentItem->mode == QM::GenerationMode::TXT2IMG || this->currentItem->mode == QM::GenerationMode::IMG2IMG) {
            std::cerr << "[EXTPROCESS] Failed to load model: " << this->currentItem->params.model_path << std::endl;
            this->currentItem->status_message = "Failed to load model: " + this->currentItem->params.model_path;
        }

        if (this->currentItem->mode == QM::GenerationMode::UPSCALE) {
            std::cerr << "[EXTPROCESS] Failed to load model: " << this->currentItem->params.esrgan_path << std::endl;
            this->currentItem->status_message = "Failed to load model: " + this->currentItem->params.esrgan_path;
        }

        this->currentItem->status     = QM::QueueStatus::FAILED;
        this->currentItem->event      = QM::QueueEvents::ITEM_MODEL_FAILED;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        j                             = *this->currentItem;
        std::string jsonString        = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
        this->currentItem = nullptr;
        return;
    }

    this->currentItem->status     = QM::QueueStatus::RUNNING;
    this->currentItem->event      = QM::QueueEvents::ITEM_MODEL_LOADED;
    this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    j                             = *this->currentItem;
    jsonString                    = j.dump();
    this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
    std::cout << "[EXTPROCESS] Starting item: " << this->currentItem->id << " type: " << QM::GenerationMode_str.at(this->currentItem->mode) << std::endl;
    switch (this->currentItem->mode) {
        case QM::GenerationMode::TXT2IMG: {
            std::cout << "[EXTPROCESS] Running txt2img" << std::endl;
            Txt2Img();
        } break;
        case QM::GenerationMode::IMG2IMG: {
            std::cout << "[EXTPROCESS] Running img2img" << std::endl;
            Img2img();
        } break;
        case QM::GenerationMode::UPSCALE: {
            std::cout << "[EXTPROCESS] Running upscale" << std::endl;
            Upscale();
        } break;

        default: {
            this->currentItem->status         = QM::QueueStatus::FAILED;
            this->currentItem->event          = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            this->currentItem->status_message = wxString::Format(_("Unknown mode: %s"), QM::GenerationMode_str.at(this->currentItem->mode).c_str());
            nlohmann::json j                  = *this->currentItem;
            std::string jsonString            = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
            std::cerr << this->currentItem->status_message << std::endl;
        } break;
    }
    this->lastItem    = this->currentItem;
    this->currentItem = nullptr;
}

void ApplicationLogic::Txt2Img() {
    try {
        this->currentItem->status     = QM::QueueStatus::RUNNING;
        this->currentItem->event      = QM::QueueEvents::ITEM_GENERATION_STARTED;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        nlohmann::json j              = *this->currentItem;
        std::string jsonString        = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

        if (this->voidHolder != nullptr) {
            delete this->voidHolder;
            this->voidHolder = nullptr;
        }
        this->voidHolder = new sd_gui_utils::VoidHolder;

        if (txt2imgFuncPtr == nullptr) {
            this->currentItem->status     = QM::QueueStatus::FAILED;
            this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j              = *this->currentItem;
            std::string jsonString        = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
            this->currentItem = nullptr;
            return;
        }

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
            this->currentItem->params.input_id_images_path.c_str());

        // free up the control image
        control_image = NULL;
        delete control_image;

        if (results == NULL) {
            this->currentItem->status     = QM::QueueStatus::FAILED;
            this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j              = *this->currentItem;
            std::string jsonString        = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
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

        this->currentItem->status      = QM::QueueStatus::RUNNING;
        this->currentItem->event       = QM::QueueEvents::ITEM_FINISHED;
        this->currentItem->updated_at  = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        this->currentItem->finished_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        j                              = *this->currentItem;
        jsonString                     = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

        delete results;

    } catch (const std::exception& e) {
        this->currentItem->status     = QM::QueueStatus::RUNNING;
        this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        nlohmann::json j              = *this->currentItem;
        std::string jsonString        = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

        std::cerr << "Error calling txt2img: " << e.what() << std::endl;
    }
}

void ApplicationLogic::Img2img() {
    try {
        this->currentItem->status     = QM::QueueStatus::RUNNING;
        this->currentItem->event      = QM::QueueEvents::ITEM_GENERATION_STARTED;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        nlohmann::json j              = *this->currentItem;
        std::string jsonString        = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

        if (this->voidHolder != nullptr) {
            delete this->voidHolder;
            this->voidHolder = nullptr;
        }
        this->voidHolder = new sd_gui_utils::VoidHolder;

        if (this->img2imgFuncPtr == nullptr) {
            this->currentItem->status     = QM::QueueStatus::FAILED;
            this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j              = *this->currentItem;
            std::string jsonString        = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
        }

        sd_image_t* input_image = NULL;
        stbi_uc* input_image_buffer;
        sd_image_t* results;

        if (this->currentItem->initial_image.length() > 0) {
            if (std::filesystem::exists(this->currentItem->initial_image)) {
                int c = 0;
                int w, h;
                input_image_buffer = stbi_load(this->currentItem->params.input_path.c_str(), &w, &h, &c, 3);
                input_image        = new sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
                input_image_buffer = NULL;
                delete input_image_buffer;
            }
        }

        sd_image_t* control_image = NULL;

        // prepare control image, if we have one
        if (this->currentItem->params.control_image_path.length() > 0) {
            if (std::filesystem::exists(this->currentItem->params.control_image_path)) {
                int c = 0;
                int w, h;
                stbi_uc* input_image_buffer = stbi_load(this->currentItem->params.control_image_path.c_str(), &w, &h, &c, 3);
                control_image               = new sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
                input_image_buffer          = NULL;
                delete input_image_buffer;
            }
        }

        // resize the input image

        int c              = 0;
        int width          = 0;
        int height         = 0;
        input_image_buffer = stbi_load(this->currentItem->initial_image.c_str(), &width, &height, &c, 3);

        if (input_image_buffer == NULL) {
            this->currentItem->status_message = "Failed to load image from '" + this->currentItem->initial_image + "'";
            this->currentItem->status         = QM::QueueStatus::FAILED;
            this->currentItem->event          = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j                  = *this->currentItem;
            std::string jsonString            = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
            std::cerr << "load image from '" << this->currentItem->initial_image << "' failed" << std::endl;
            return;
        }
        if (c < 3) {
            this->currentItem->status_message = "The number of channels for the input image must be >= 3, but got " + std::to_string(c) + " channels";
            this->currentItem->status         = QM::QueueStatus::FAILED;
            this->currentItem->event          = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j                  = *this->currentItem;
            std::string jsonString            = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
            fprintf(stderr, "the number of channels for the input image must be >= 3, but got %d channels\n", c);
            free(input_image_buffer);
            return;
        }
        if (width <= 0) {
            this->currentItem->status_message = "The width of image must be greater than 0";
            this->currentItem->status         = QM::QueueStatus::FAILED;
            this->currentItem->event          = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j                  = *this->currentItem;
            std::string jsonString            = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
            fprintf(stderr, "error: the width of image must be greater than 0\n");
            free(input_image_buffer);
            return;
        }
        if (height <= 0) {
            this->currentItem->status_message = "The height of image must be greater than 0";
            this->currentItem->status         = QM::QueueStatus::FAILED;
            this->currentItem->event          = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j                  = *this->currentItem;
            std::string jsonString            = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
            fprintf(stderr, "error: the height of image must be greater than 0\n");
            free(input_image_buffer);
            return;
        }

        // Resize input image ...
        if (this->currentItem->params.width != width || this->currentItem->params.height != height) {
            printf("resize input image from %dx%d to %dx%d\n", width, height, this->currentItem->params.width, this->currentItem->params.height);
            int resized_height = this->currentItem->params.height;
            int resized_width  = this->currentItem->params.width;

            uint8_t* resized_image_buffer = (uint8_t*)malloc(resized_height * resized_width * 3);
            if (resized_image_buffer == NULL) {
                this->currentItem->status_message = "error: allocate memory for resize input image";
                this->currentItem->status         = QM::QueueStatus::FAILED;
                this->currentItem->event          = QM::QueueEvents::ITEM_FAILED;
                this->currentItem->updated_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                nlohmann::json j                  = *this->currentItem;
                std::string jsonString            = j.dump();
                this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
                fprintf(stderr, "error: allocate memory for resize input image\n");
                free(input_image_buffer);
                return;
            }
            stbir_resize(input_image_buffer, width, height, 0,
                         resized_image_buffer, resized_width, resized_height, 0, STBIR_TYPE_UINT8,
                         3 /*RGB channel*/, STBIR_ALPHA_CHANNEL_NONE, 0,
                         STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
                         STBIR_FILTER_BOX, STBIR_FILTER_BOX,
                         STBIR_COLORSPACE_SRGB, nullptr);

            // Save resized result
            free(input_image_buffer);
            input_image_buffer = resized_image_buffer;
        }

        results = this->img2imgFuncPtr(
            this->sd_ctx,
            *input_image,
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
            this->currentItem->params.input_id_images_path.c_str());

        // free up the control image
        free(control_image);
        free(input_image_buffer);
        free(input_image->data);
        input_image = NULL;
        delete control_image;

        if (results == NULL) {
            this->currentItem->status     = QM::QueueStatus::FAILED;
            this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j              = *this->currentItem;
            std::string jsonString        = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
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

        this->currentItem->status      = QM::QueueStatus::RUNNING;
        this->currentItem->event       = QM::QueueEvents::ITEM_FINISHED;
        this->currentItem->updated_at  = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        this->currentItem->finished_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        j                              = *this->currentItem;
        jsonString                     = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

        delete results;

    } catch (const std::exception& e) {
        this->currentItem->status     = QM::QueueStatus::RUNNING;
        this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        nlohmann::json j              = *this->currentItem;
        std::string jsonString        = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

        std::cerr << "Error calling txt2img: " << e.what() << std::endl;
    }
}

void ApplicationLogic::Upscale() {
    sd_image_t results;
    int c = 0;
    int w, h;
    stbi_uc* input_image_buffer = stbi_load(this->currentItem->initial_image.c_str(), &w, &h, &c, 3);
    sd_image_t control_image    = sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
    input_image_buffer          = NULL;
    delete input_image_buffer;

    results = this->upscalerFuncPtr(this->upscale_ctx, control_image, this->currentItem->upscale_factor);

    if (results.data == NULL) {
        this->currentItem->status     = QM::QueueStatus::FAILED;
        this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        nlohmann::json j              = *this->currentItem;
        std::string jsonString        = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
        return;
    }

    std::string filepath = this->handleSdImage(results);
    std::cout << "saved tmp image to: " << filepath;
    this->currentItem->rawImages.push_back(filepath);
    free(results.data);
    results.data                   = NULL;
    this->currentItem->status      = QM::QueueStatus::DONE;
    this->currentItem->event       = QM::QueueEvents::ITEM_FINISHED;
    this->currentItem->updated_at  = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    this->currentItem->finished_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    nlohmann::json j               = *this->currentItem;
    std::string jsonString         = j.dump();
    this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
}

std::string ApplicationLogic::handleSdImage(sd_image_t& image) {
    if (image.data == nullptr) {
        std::cerr << __FILE__ << ":" << __LINE__ << ": image data is null" << std::endl;
        return "";
    }

    std::filesystem::path tempPath = std::filesystem::temp_directory_path();
    if (!std::filesystem::exists(tempPath)) {
        std::cerr << __FILE__ << ":" << __LINE__ << ": temp directory does not exist" << std::endl;
        return "";
    }

    std::string filename = tempPath.string() + "/" + generateRandomFilename(".jpg");

    if (stbi_write_jpg(filename.c_str(), image.width, image.height, 3, image.data, 100) == 0) {
        std::cerr << __FILE__ << ":" << __LINE__ << ": stbi_write_jpg failed" << std::endl;
        return "";
    }
    std::cout << __FILE__ << ":" << __LINE__ << ": saved image to: " << filename << " size: " << image.width << "x" << image.height << " filesize: " << std::filesystem::file_size(filename) << std::endl;

    return filename;
}

bool ApplicationLogic::loadSdModel(std::shared_ptr<QM::QueueItem> item) {
    if (item->mode == QM::GenerationMode::CONVERT) {
        this->error = "Not implemented yes";
        return false;
    }

    // hnalde upscaler model
    if (item->mode == QM::GenerationMode::UPSCALE) {
        // free up the sd model
        if (this->sd_ctx != nullptr) {
            this->freeSdCtxPtr(this->sd_ctx);
            this->sd_ctx = nullptr;
        }

        // check if we need reload the model
        if (this->lastItem != nullptr && this->lastItem->mode == QM::GenerationMode::UPSCALE) {
            if (this->lastItem->params.esrgan_path != item->params.esrgan_path) {
                if (this->upscale_ctx != nullptr) {
                    this->freeUpscalerCtxPtr(this->upscale_ctx);
                    this->upscale_ctx = nullptr;
                }
                // need to load a new model
                this->upscale_ctx = this->newUpscalerCtxPtr(item->params.esrgan_path.c_str(), item->params.n_threads, item->params.wtype);
                return this->upscale_ctx != NULL;
            }
            return true;  // already loaded the model
        }
        return false;
    }
    if (item->mode == QM::GenerationMode::TXT2IMG || item->mode == QM::GenerationMode::IMG2IMG) {
        /// handle sd model
        bool loadModel = true;
        if (this->lastItem != nullptr) {
            if (this->lastItem->params.model_path == item->params.model_path) {
                loadModel = false;
            }

            if (this->lastItem->mode != item->mode) {
                loadModel = true;
            }

            if (this->lastItem->params.clip_l_path != item->params.clip_l_path) {
                loadModel = true;
            }

            if (this->lastItem->params.clip_g_path != item->params.clip_g_path) {
                loadModel = true;
            }

            if (this->lastItem->params.t5xxl_path != item->params.t5xxl_path) {
                loadModel = true;
            }

            if (this->lastItem->params.diffusion_model_path != item->params.diffusion_model_path) {
                loadModel = true;
            }

            if (this->lastItem->params.vae_path != item->params.vae_path) {
                loadModel = true;
            }

            if (this->lastItem->params.taesd_path != item->params.taesd_path) {
                loadModel = true;
            }

            if (this->lastItem->params.controlnet_path != item->params.controlnet_path) {
                loadModel = true;
            }

            if (this->lastItem->params.lora_model_dir != item->params.lora_model_dir) {
                loadModel = true;
            }

            if (this->lastItem->params.embeddings_path != item->params.embeddings_path) {
                loadModel = true;
            }

            if (this->lastItem->params.stacked_id_embeddings_path != item->params.stacked_id_embeddings_path) {
                loadModel = true;
            }

            if (this->lastItem->params.vae_tiling != item->params.vae_tiling) {
                loadModel = true;
            }

            if (this->lastItem->params.n_threads != item->params.n_threads) {
                loadModel = true;
            }

            if (this->lastItem->params.wtype != item->params.wtype) {
                loadModel = true;
            }

            if (this->lastItem->params.rng_type != item->params.rng_type) {
                loadModel = true;
            }

            if (this->lastItem->params.schedule != item->params.schedule) {
                loadModel = true;
            }
            if (this->lastItem->params.clip_on_cpu != item->params.clip_on_cpu) {
                loadModel = true;
            }
            if (this->lastItem->params.control_net_cpu != item->params.control_net_cpu) {
                loadModel = true;
            }
            if (this->lastItem->params.vae_on_cpu != item->params.vae_on_cpu) {
                loadModel = true;
            }
        }

        if (loadModel == true) {
            if (this->sd_ctx != nullptr) {
                this->freeSdCtxPtr(this->sd_ctx);
                this->sd_ctx = nullptr;
            }

            std::cout << "[EXTPROCESS] Loading model: " << item->params.model_path << std::endl;
            bool vae_decode_only = true;

            switch (item->mode) {
                case QM::GenerationMode::IMG2IMG:
                    // case QM::GenerationMode::IMG2VID:
                    vae_decode_only = false;
                    break;
                case QM::GenerationMode::TXT2IMG:
                    vae_decode_only = true;
                    break;
                default:
                    break;
            }

            this->sd_ctx = this->newSdCtxFuncPtr(
                item->params.model_path.c_str(),
                item->params.clip_l_path.c_str(),
                item->params.clip_g_path.c_str(),
                item->params.t5xxl_path.c_str(),
                item->params.diffusion_model_path.c_str(),
                item->params.vae_path.c_str(),
                item->params.taesd_path.c_str(),
                item->params.controlnet_path.c_str(),
                item->params.lora_model_dir.c_str(),
                item->params.embeddings_path.c_str(),
                item->params.stacked_id_embeddings_path.c_str(),
                vae_decode_only,  // vae decode only
                item->params.vae_tiling,
                false,  // free params immediately
                item->params.n_threads,
                item->params.wtype,
                item->params.rng_type,
                item->params.schedule,
                item->params.clip_on_cpu,
                item->params.control_net_cpu,
                item->params.vae_on_cpu

            );
            loadedModel = item->params.model_path;
            if (this->sd_ctx == NULL) {
                return false;
            }
        }
        return sd_ctx == NULL ? false : true;
    }
    return false;
}