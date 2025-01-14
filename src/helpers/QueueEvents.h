#ifndef __SDGUI_HELPERS_QUEUE_EVENTS__
#define __SDGUI_HELPERS_QUEUE_EVENTS__
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
#endif
