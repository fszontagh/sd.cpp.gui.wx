#ifndef _NETWORK_REMOTE_QUEUE_JOB_ITEM_H_
#define _NETWORK_REMOTE_QUEUE_JOB_ITEM_H_

namespace sd_gui_utils {
    namespace helpers {
        using IDType = uint64_t;
        inline IDType generateID(const std::string& server_id) {
            std::hash<std::string> hasher;
            size_t server_hash = hasher(server_id);
            auto now           = std::chrono::system_clock::now();
            auto timestamp     = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            IDType id = (static_cast<IDType>(server_hash) << 32) | static_cast<IDType>(timestamp);
            return id;
        }
    }
    inline namespace networks {
        struct RemoteQueueItem {
            uint64_t id = 0, created_at = 0, updated_at = 0, finished_at = 0, started_at = 0;
            SDParams params      = SDParams();
            QueueStatus status   = QueueStatus::PENDING;
            QueueEvents event    = QueueEvents::ITEM_ADDED;
            QueueItemStats stats = QueueItemStats();
            int step = 0, steps = 0;
            size_t hash_fullsize = 0, hash_progress_size = 0;
            float time                           = 0;
            std::string model                    = "";
            SDMode mode                          = SDMode::TXT2IMG;
            std::string status_message           = "";
            uint32_t upscale_factor              = 4;
            std::string sha256                   = "";
            std::string app_version              = SD_GUI_VERSION;
            std::string git_version              = GIT_HASH;
            std::string original_prompt          = "";
            std::string original_negative_prompt = "";
            bool keep_checkpoint_in_memory       = false;
            bool keep_upscaler_in_memory         = false;
            bool need_sha256                     = false;
            std::string generated_sha256         = "";
            int update_index                     = -1;
            std::string server                   = "";
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RemoteQueueItem,
                                                        id,
                                                        created_at,
                                                        updated_at,
                                                        finished_at,
                                                        started_at,
                                                        params,
                                                        status,
                                                        event,
                                                        stats,
                                                        step,
                                                        steps,
                                                        hash_fullsize,
                                                        hash_progress_size,
                                                        time,
                                                        model,
                                                        mode,
                                                        status_message,
                                                        upscale_factor,
                                                        sha256,
                                                        app_version,
                                                        git_version,
                                                        original_prompt,
                                                        original_negative_prompt,
                                                        keep_checkpoint_in_memory,
                                                        keep_upscaler_in_memory,
                                                        need_sha256,
                                                        generated_sha256,
                                                        update_index,
                                                        server)
    }  // namespace networks
}  // namespace sd_gui_utils

#endif  // _NETWORK_REMOTE_QUEUE_JOB_ITEM_H_