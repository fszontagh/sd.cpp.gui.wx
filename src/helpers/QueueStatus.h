#ifndef __SDGUI_HELPERS_QUEUE_STATUS__
#define __SDGUI_HELPERS_QUEUE_STATUS__

enum QueueStatus {
    PENDING       = 1 << 0,  // deletable
    RUNNING       = 1 << 1,  // non deletable, running
    PAUSED        = 1 << 2,  // deletable
    FAILED        = 1 << 3,  // deletable
    MODEL_LOADING = 1 << 4,  // non deletable, running
    DONE          = 1 << 5,  // deletable
    HASHING       = 1 << 6,  // non deletable, running
    HASHING_DONE  = 1 << 7   // non deletable, running
};

namespace QueueStatusFlags {
    constexpr int DELETABLE_FLAG   = (PENDING | PAUSED | FAILED | DONE);
    constexpr int RUNNING_FLAG     = (RUNNING | MODEL_LOADING | HASHING | HASHING_DONE);
    constexpr int PAUSABLE_FLAG    = (PENDING | PAUSED);
    constexpr int REQUEUEABLE_FLAG = (RUNNING | HASHING);
};

inline const std::unordered_map<QueueStatus, wxString> QueueStatus_GUI_str = {
    {QueueStatus::PENDING, _("pending")},
    {QueueStatus::RUNNING, _("running")},
    {QueueStatus::PAUSED, _("paused")},
    {QueueStatus::FAILED, _("failed")},
    {QueueStatus::MODEL_LOADING, _("model loading...")},
    {QueueStatus::DONE, _("finished")},
    {QueueStatus::HASHING, _("model hashing...")}};

#endif  // __SDGUI_HELPERS_QUEUE_STATUS__