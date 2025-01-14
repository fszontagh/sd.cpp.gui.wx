#ifndef __SDGUI_HELPERS_QUEUE_STATUS__
#define __SDGUI_HELPERS_QUEUE_STATUS__

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

#endif  // __SDGUI_HELPERS_QUEUE_STATUS__