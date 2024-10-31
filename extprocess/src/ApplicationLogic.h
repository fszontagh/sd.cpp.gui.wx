#ifndef EXTPROCESS_APPLICATIONLOGIC_H
#define EXTPROCESS_APPLICATIONLOGIC_H

#include "ui/QueueManager.h"
#include "libs/SharedLibrary.h"
#include "libs/SharedMemoryManager.h"
#include "libs/json.hpp"

class ApplicationLogic {
public:
    ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager);
    ~ApplicationLogic();
    inline static void HandleSDLog(sd_log_level_t level, const char* text, void* data) {
    }

    inline static void HandleSDProgress(int step, int steps, float time, void* data) {
        ApplicationLogic* instance = static_cast<ApplicationLogic*>(data);

        instance->currentItem->step       = step;
        instance->currentItem->steps      = steps;
        instance->currentItem->time       = time;
        instance->currentItem->status     = QM::QueueStatus::RUNNING;
        instance->currentItem->event      = QM::QueueEvents::ITEM_UPDATED;
        instance->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        nlohmann::json j                  = *instance->currentItem;
        std::string jsonString            = j.dump();
        instance->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
    }

    bool loadLibrary();

    inline const std::string getErrorMessage() {
        return this->error;
    }
    inline const auto getCurrentItem() { return this->currentItem; }

    void processMessage(QM::QueueItem& item);

private:
    SharedLibrary sd_dll;
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager;
    std::filesystem::path loadedModel;
    std::string error;
    sd_ctx_t* sd_ctx                                           = nullptr;
    upscaler_ctx_t* upscale_ctx                                = nullptr;
    std::shared_ptr<QM::QueueItem> currentItem                 = nullptr;  //< shared pointer to current item>
    std::shared_ptr<QM::QueueItem> lastItem                    = nullptr;  //< shared pointer to last item>
    FreeSdCtxFunction freeSdCtxPtr                             = nullptr;
    Txt2ImgFunction txt2imgFuncPtr                             = nullptr;
    Img2ImgFunction img2imgFuncPtr                             = nullptr;
    UpscalerFunction upscalerFuncPtr                           = nullptr;
    NewSdCtxFunction newSdCtxFuncPtr                           = nullptr;
    NewUpscalerCtxFunction newUpscalerCtxPtr                   = nullptr;
    ConvertFunction convertFuncPtr                             = nullptr;
    PreprocessCannyFunction preprocessCannyFuncPtr             = nullptr;
    FreeUpscalerCtxFunction freeUpscalerCtxPtr                 = nullptr;
    SdSetProgressCallbackFunction sdSetProgressCallbackFuncPtr = nullptr;
    sd_gui_utils::VoidHolder* voidHolder                       = nullptr;

    void Txt2Img();

    bool loadSdModel(std::shared_ptr<QM::QueueItem> item) {
        bool loadModel = true;
        if (this->lastItem != nullptr) {
            if (this->lastItem->params.model_path == item->params.model_path) {
                loadModel = false;
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

            // load model
            sd_ctx = this->newSdCtxFuncPtr(
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
                false,  // vae decode only
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
            return sd_ctx == NULL ? false : true;
        }
        return sd_ctx == NULL ? false : true;
    }

    std::string handleSdImage(sd_image_t& image);
    std::string generateRandomFilename(const std::string& extension = ".tmp") {
        const char charset[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789";
        const size_t length = 10;  // fájlnév hossza

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

        std::string filename = "stablediffusiongui_";
        for (size_t i = 0; i < length; ++i) {
            filename += charset[dist(generator)];
        }
        return filename + extension;
    }
};

#endif  // EXTPROCESS_APPLICATIONLOGIC_H