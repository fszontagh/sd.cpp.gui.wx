#ifndef EXTPROCESS_APPLICATIONLOGIC_H
#define EXTPROCESS_APPLICATIONLOGIC_H

class ApplicationLogic {
public:
    ApplicationLogic(const std::string& libName, std::shared_ptr<SharedMemoryManager>& sharedMemoryManager);
    ~ApplicationLogic();

    // stdout/stderr captured by parent
    inline static void HandleSDLog(sd_log_level_t level, const char* text, void* data) {
        wxString msg(text);
        if (msg.IsEmpty()) {
            return;
        }
        auto instance = static_cast<ApplicationLogic*>(data);
        std::lock_guard<std::mutex> lock(instance->itemMutex);
        instance->updateLastMessageTime();
        switch (level) {
            case sd_log_level_t::SD_LOG_DEBUG: {
                wxLogInfo(msg);  // debug logs go into the info log otherwise it printed to std::out
            } break;
            case sd_log_level_t::SD_LOG_INFO: {
                wxLogInfo(msg);
            } break;
            case sd_log_level_t::SD_LOG_ERROR: {
                wxLogError(msg);
            } break;
            case sd_log_level_t::SD_LOG_WARN: {
                wxLogWarning(msg);
            } break;
            default: {
                wxLogMessage(msg);
            } break;
        }
        std::cout.flush();
    }
    inline static void HandleHashCallback(size_t readed, std::string hash, void* data) {
        ApplicationLogic* instance = static_cast<ApplicationLogic*>(data);
        auto item                  = instance->currentItem;

        if (item != nullptr) {
            item->hash_progress_size = readed;
            item->generated_sha256   = hash;
            instance->sendStatus(QueueStatus::HASHING, QueueEvents::ITEM_MODEL_HASH_UPDATE);
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
        instance->currentItem->stats.time_per_step.emplace_back(QueueStatsStepItem(step, steps, time));
        instance->currentItem->stats.time_total += time;

        if (instance->currentItem->step > 0) {
            instance->currentItem->stats.time_avg = instance->currentItem->stats.time_total / instance->currentItem->step;
        }

        instance->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        instance->sendStatus(QueueEvents::ITEM_UPDATED);
    }

    bool loadLibrary();

    inline const std::string getErrorMessage() {
        return this->error;
    }
    inline const auto getCurrentItem() { return this->currentItem; }

    void processMessage(QueueItem& item);
    void updateLastMessageTime() { this->last_message_time.store(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count()); };
    uint64_t GetLastMessageTime() { return last_message_time.load(); }
    uint64_t CalcTimeoutTime() {
        auto curtime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if (this->last_message_time.load() == 0) {
            return 0;
        }
        return this->last_message_time.load() - curtime;
    }

private:
    SharedLibrary sd_dll;
    std::shared_ptr<SharedMemoryManager> sharedMemoryManager;
    std::string error;
    sd_ctx_t* sd_ctx                                           = NULL;
    upscaler_ctx_t* upscale_ctx                                = nullptr;
    std::shared_ptr<QueueItem> currentItem                     = nullptr;  //< shared pointer to current item>
    std::shared_ptr<QueueItem> lastItem                        = nullptr;  //< shared pointer to last item>
    // New API functions
    SdCtxParamsInitFunction sdCtxParamsInitPtr                 = nullptr;
    NewSdCtxFunction newSdCtxFuncPtr                           = nullptr;
    FreeSdCtxFunction freeSdCtxPtr                             = nullptr;
    SdImgGenParamsInitFunction sdImgGenParamsInitPtr           = nullptr;
    GenerateImageFunction generateImageFuncPtr                 = nullptr;
    NewUpscalerCtxFunction newUpscalerCtxPtr                   = nullptr;
    FreeUpscalerCtxFunction freeUpscalerCtxPtr                 = nullptr;
    UpscalerFunction upscalerFuncPtr                           = nullptr;
    ConvertFunction convertFuncPtr                             = nullptr;
    PreprocessCannyFunction preprocessCannyFuncPtr             = nullptr;
    SdSetProgressCallbackFunction sdSetProgressCallbackFuncPtr = nullptr;
    SdSetLogCallbackFunction sdSetLogCallbackFuncPtr           = nullptr;
    std::mutex itemMutex;
    std::mutex logMutex;
    std::string tempPath;
    std::atomic<uint64_t> last_message_time = {0};  // store the last message from the parent

    void Txt2Img();
    void Img2img();
    void Upscale();

    bool loadSdModel();

    std::string handleSdImage(sd_image_t& image);
    inline void sendStatus(QueueStatus status, QueueEvents event, const std::string& reason = "", unsigned int sleep = 0) {
        if (this->currentItem == nullptr) {
            return;
        }
        // we need some time to wait to the main process
        if (sleep > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        }

        std::lock_guard<std::mutex> lock(this->itemMutex);
        this->currentItem->status = status;
        // this->currentItem->event      = event;
        this->currentItem->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        this->currentItem->update_index++;

        if (reason.length() > 0) {
            this->currentItem->status_message = reason;
        }

        nlohmann::json j       = *this->currentItem;
        std::string jsonString = j.dump();
        this->sharedMemoryManager->write(jsonString.c_str(), jsonString.length());
        this->updateLastMessageTime();
    }
    inline void sendStatus(QueueEvents event, const std::string& reason = "", unsigned int sleep = 0) {
        if (this->currentItem == nullptr) {
            return;
        }
        this->sendStatus(this->currentItem->status, event, reason, sleep);
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