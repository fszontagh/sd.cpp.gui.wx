#ifndef EXTPROCESS_APPLICATIONLOGIC_H
#define EXTPROCESS_APPLICATIONLOGIC_H

class ApplicationLogic {
public:
    ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager);
    ~ApplicationLogic();

    // stdout/stderr captured by parent
    inline static void HandleSDLog(sd_log_level_t level, const char* text, void* data) {
        if (level == SD_LOG_ERROR) {
            std::cerr << text;
        } else {
            std::cout << text;
        }
    }
    inline static void HandleHashCallback(size_t readed, std::string hash, void* data) {
        ApplicationLogic* instance = static_cast<ApplicationLogic*>(data);
        auto item = instance->currentItem;

        if (item != nullptr) {
            item->hash_progress_size = readed;
            instance->sendStatus(QM::QueueStatus::HASHING, QM::QueueEvents::ITEM_MODEL_HASH_UPDATE);
        }
    }

    inline static void HandleSDProgress(int step, int steps, float time, void* data) {
        if (time == 0.0f) {
            return;
        }
        ApplicationLogic* instance = static_cast<ApplicationLogic*>(data);

        instance->currentItem->step  = step;
        instance->currentItem->steps = steps;
        instance->currentItem->time  = time;

        if (instance->currentItem->stats.time_min == 0) {
            instance->currentItem->stats.time_min = time;
        } else {
            instance->currentItem->stats.time_min = instance->currentItem->stats.time_min > time ? time : instance->currentItem->stats.time_min;
        }

        if (instance->currentItem->stats.time_max == 0) {
            instance->currentItem->stats.time_max = time;
        } else {
            instance->currentItem->stats.time_max = instance->currentItem->stats.time_max < time ? time : instance->currentItem->stats.time_max;
        }

        // instance->currentItem->stats.time_per_step[step] = time;
        instance->currentItem->stats.time_per_step.emplace_back(QM::QueueStatsStepItem(step, steps, time));
        instance->currentItem->stats.time_total += time;

        if (instance->currentItem->step > 0) {
            instance->currentItem->stats.time_avg = instance->currentItem->stats.time_total / instance->currentItem->step;
        }

        instance->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        instance->sendStatus(QM::QueueStatus::RUNNING, QM::QueueEvents::ITEM_UPDATED);
        std::cout << "[EXTPROCESS] Item: " << instance->currentItem->id << " Step: " << instance->currentItem->step << "/" << instance->currentItem->steps << " Time: " << instance->currentItem->time << "s" << std::endl;
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
    std::string error;
    sd_ctx_t* sd_ctx                                           = NULL;
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
    std::mutex itemMutex;
    std::string tempPath;

    void Txt2Img();
    void Img2img();
    void Upscale();

    bool loadSdModel();

    std::string handleSdImage(sd_image_t& image);

    inline void sendStatus(QM::QueueStatus status, QM::QueueEvents event, const std::string& reason = "") {
        if (this->currentItem == nullptr) {
            return;
        }
        std::lock_guard<std::mutex> lock(this->itemMutex);
        this->currentItem->status     = status;
        this->currentItem->event      = event;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        this->currentItem->update_index++;

        if (reason.length() > 0) {
            this->currentItem->status_message = reason;
        }

        nlohmann::json j       = *this->currentItem;
        std::string jsonString = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
    }
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