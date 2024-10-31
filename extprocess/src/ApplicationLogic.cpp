#include "ApplicationLogic.h"
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
#include "libs/stbi_image_write.h"
#endif

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
    if (this->currentItem == nullptr) {
        std::cout << "[EXTPROCESS] New item: " << item.id << std::endl;
        this->currentItem = std::make_shared<QM::QueueItem>(item);
    }

    this->currentItem->status     = QM::QueueStatus::RUNNING;
    this->currentItem->event      = QM::QueueEvents::ITEM_MODEL_LOAD_START;
    this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    this->currentItem->started_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    nlohmann::json j              = *this->currentItem;
    std::string jsonString        = j.dump();
    this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());

    std::cout << "[EXTPROCESS] Processing item: " << this->currentItem->id << std::endl;

    if (this->loadSdModel(this->currentItem) == false) {
        std::cerr << "[EXTPROCESS] Failed to load model: " << this->currentItem->params.model_path << std::endl;
        this->currentItem->status     = QM::QueueStatus::RUNNING;
        this->currentItem->event      = QM::QueueEvents::ITEM_MODEL_FAILED;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        j                             = *this->currentItem;
        std::string jsonString        = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
        return;
    }

    this->currentItem->status     = QM::QueueStatus::RUNNING;
    this->currentItem->event      = QM::QueueEvents::ITEM_MODEL_LOADED;
    this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    j                             = *this->currentItem;
    jsonString                    = j.dump();
    this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
    std::cout << "[EXTPROCESS] Sent : " << jsonString << std::endl;

    switch (this->currentItem->mode) {
        case QM::GenerationMode::TXT2IMG: {
            std::cout << "[EXTPROCESS] Running txt2img" << std::endl;
            Txt2Img();
            break;
        }
        default: {
            this->currentItem->status     = QM::QueueStatus::RUNNING;
            this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j              = *this->currentItem;
            std::string jsonString        = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
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

        this->txt2imgFuncPtr = sd_dll.getFunction<Txt2ImgFunction>("txt2img");

        if (this->voidHolder != nullptr) {
            delete this->voidHolder;
            this->voidHolder = nullptr;
        }
        this->voidHolder = new sd_gui_utils::VoidHolder;

        sdSetProgressCallbackFuncPtr(ApplicationLogic::HandleSDProgress, (void*)this);

        if (txt2imgFuncPtr == nullptr) {
            this->currentItem->status     = QM::QueueStatus::FAILED;
            this->currentItem->event      = QM::QueueEvents::ITEM_FAILED;
            this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            nlohmann::json j              = *this->currentItem;
            std::string jsonString        = j.dump();
            this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
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