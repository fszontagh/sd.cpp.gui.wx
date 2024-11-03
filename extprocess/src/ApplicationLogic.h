#ifndef EXTPROCESS_APPLICATIONLOGIC_H
#define EXTPROCESS_APPLICATIONLOGIC_H

#include "ui/QueueManager.h"
#include "libs/SharedLibrary.h"
#include "libs/SharedMemoryManager.h"
#include "libs/json.hpp"
#include "helpers/sd.hpp"
#include <string>
#include <memory>

class ApplicationLogic {
public:
    ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager);
    ~ApplicationLogic();

    // stdout/stderr captured by parent
    inline static void HandleSDLog(sd_log_level_t level, const char* text, void* data) {
        if (level == SD_LOG_ERROR) {
            std::cerr << "SD ERR: " << text;
        } else {
            std::cout << "SD: "<< text;
        }
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
    SdSetLogCallbackFunction sdSetLogCallbackFuncPtr           = nullptr;
    sd_gui_utils::VoidHolder* voidHolder                       = nullptr;

    void Txt2Img();
    void Img2img();
    void Upscale();

    bool loadSdModel(std::shared_ptr<QM::QueueItem> item);

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