#ifndef __HELPERS_QUEUE_ITEM_
#define __HELPERS_QUEUE_ITEM_
/**
@file QueueItem.h
@brief A status of a QueueItem
*/
enum QueueStatus {
    PENDING,        // deletable
    RUNNING,        // non deletable
    PAUSED,         // deletable
    FAILED,         // deletable
    MODEL_LOADING,  // non deletable
    DONE,           // deletable
    HASHING,        // non deletable
    HASHING_DONE    // non deletable
};

inline const std::unordered_map<SDMode, std::string> GenerationMode_str = {
    {SDMode::TXT2IMG, "txt2img"},
    {SDMode::IMG2IMG, "img2img"},
    {SDMode::CONVERT, "convert"},
    {SDMode::UPSCALE, "upscale"},
    {SDMode::IMG2VID, "img2vid"}};

inline const std::unordered_map<std::string, SDMode> GenerationMode_str_inv = {
    {"txt2img", SDMode::TXT2IMG},
    {"img2img", SDMode::IMG2IMG},
    {"convert", SDMode::CONVERT},
    {"upscale", SDMode::UPSCALE},
    {"img2vid", SDMode::IMG2VID}};

inline const char* QueueStatus_str[] = {
    _("pending"),
    _("running"),
    _("paused"),
    _("failed"),
    _("model loading..."),
    _("finished"),
    _("model hashing...")};

inline const std::unordered_map<QueueStatus, wxString> QueueStatus_GUI_str = {
    {QueueStatus::PENDING, "pending"},
    {QueueStatus::RUNNING, "running"},
    {QueueStatus::PAUSED, "paused"},
    {QueueStatus::FAILED, "failed"},
    {QueueStatus::MODEL_LOADING, "model loading..."},
    {QueueStatus::DONE, "finished"},
    {QueueStatus::HASHING, "model hashing..."}};

/// @brief Event commands to inter thread communication
enum class QueueEvents : unsigned int {
    ITEM_DELETED            = 1,
    ITEM_ADDED              = 2,
    ITEM_STATUS_CHANGED     = 4,
    ITEM_UPDATED            = 8,
    ITEM_START              = 16,
    ITEM_FINISHED           = 32,
    ITEM_MODEL_LOAD_START   = 64,
    ITEM_MODEL_LOADED       = 128,
    ITEM_MODEL_FAILED       = 256,
    ITEM_GENERATION_STARTED = 512,
    ITEM_FAILED             = 1024,
    ITEM_MODEL_HASH_START   = 2048,
    ITEM_MODEL_HASH_UPDATE  = 4096,
    ITEM_MODEL_HASH_DONE    = 8192
};

inline const std::unordered_map<QueueEvents, std::string> QueueEvents_str = {
    {QueueEvents::ITEM_DELETED, "ITEM_DELETED"},
    {QueueEvents::ITEM_ADDED, "ITEM_ADDED"},
    {QueueEvents::ITEM_STATUS_CHANGED, "ITEM_STATUS_CHANGED"},
    {QueueEvents::ITEM_UPDATED, "ITEM_UPDATED"},
    {QueueEvents::ITEM_START, "ITEM_START"},
    {QueueEvents::ITEM_FINISHED, "ITEM_FINISHED"},
    {QueueEvents::ITEM_MODEL_LOAD_START, "ITEM_MODEL_LOAD_START"},
    {QueueEvents::ITEM_MODEL_LOADED, "ITEM_MODEL_LOADED"},
    {QueueEvents::ITEM_MODEL_FAILED, "ITEM_MODEL_FAILED"},
    {QueueEvents::ITEM_GENERATION_STARTED, "ITEM_GENERATION_STARTED"},
    {QueueEvents::ITEM_FAILED, "ITEM_FAILED"},
    {QueueEvents::ITEM_MODEL_HASH_START, "ITEM_MODEL_HASH_START"},
    {QueueEvents::ITEM_MODEL_HASH_UPDATE, "ITEM_MODEL_HASH_UPDATE"},
    {QueueEvents::ITEM_MODEL_HASH_DONE, "ITEM_MODEL_HASH_DONE"}};

struct QueueStatsStepItem {
    int step;
    int steps;
    float time;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(QueueStatsStepItem, step, steps, time)
struct QueueItemStats {
    float time_min = 0.f, time_max = 0.f, time_avg = 0.f, time_total = 0.f;
    std::vector<QueueStatsStepItem> time_per_step = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(QueueItemStats, time_min, time_max, time_avg, time_total, time_per_step)

#endif  // __HELPERS_QUEUE_ITEM