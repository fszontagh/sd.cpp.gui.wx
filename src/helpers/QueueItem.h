#ifndef HELPERS_QUEUE_ITEM_H
#define HELPERS_QUEUE_ITEM_H

#include "../libs/stb_image.h"

struct QueueItem : public sd_gui_utils::networks::RemoteQueueItem {
    QueueItem() = default;
    QueueItem(const sd_gui_utils::networks::RemoteQueueItem& remoteItem)
        : sd_gui_utils::networks::RemoteQueueItem(remoteItem),
          rawImages({}),
          initial_image(""),
          mask_image("") {
        id                        = remoteItem.id;
        created_at                = remoteItem.created_at;
        updated_at                = remoteItem.updated_at;
        finished_at               = remoteItem.finished_at;
        started_at                = remoteItem.started_at;
        number_of_images          = remoteItem.number_of_images;
        params                    = remoteItem.params;
        status                    = remoteItem.status;
        stats                     = remoteItem.stats;
        step                      = remoteItem.step;
        steps                     = remoteItem.steps;
        hash_fullsize             = remoteItem.hash_fullsize;
        hash_progress_size        = remoteItem.hash_progress_size;
        time                      = remoteItem.time;
        model                     = remoteItem.model;
        hashes                    = remoteItem.hashes;
        mode                      = remoteItem.mode;
        status_message            = remoteItem.status_message;
        upscale_factor            = remoteItem.upscale_factor;
        sha256                    = remoteItem.sha256;
        app_version               = remoteItem.app_version;
        git_version               = remoteItem.git_version;
        original_prompt           = remoteItem.original_prompt;
        original_negative_prompt  = remoteItem.original_negative_prompt;
        keep_checkpoint_in_memory = remoteItem.keep_checkpoint_in_memory;
        keep_upscaler_in_memory   = remoteItem.keep_upscaler_in_memory;
        need_sha256               = remoteItem.need_sha256;
        generated_sha256          = remoteItem.generated_sha256;
        update_index              = remoteItem.update_index;
        server                    = remoteItem.server;
        image_info                = remoteItem.image_info;
    }

    // --- Data members ---
    std::vector<std::string> rawImages = {};  ///< Stores raw image data before processing
    std::string initial_image          = "";  ///< Path to the initial input image
    std::string mask_image             = "";  ///< Path to the mask image if applicable
    bool image_download_requested      = false;

    // --- Conversion and initialization methods ---
    [[nodiscard]] QueueItem ConvertToSharedMemory();                                                                                                 ///< Converts the item to shared memory format.
    void ConvertFromSharedMemory(QueueItem item);                                                                                                    ///< Restores an item from shared memory.
    [[nodiscard]] static QueueItem convertFromNetwork(sd_gui_utils::networks::RemoteQueueItem&& item, wxString tempDir = wxFileName::GetTempDir());  ///< Converts a network queue item to a local QueueItem.
    [[nodiscard]] sd_gui_utils::networks::RemoteQueueItem convertToNetwork(bool clear_images_data = true, std::string model_names_prefix = "");      ///< Converts the item to a network-compatible format.

    // --- Query methods ---
    [[nodiscard]] const wxString GetActualSpeed() const;  ///< Retrieves the actual processing speed.
    [[nodiscard]] const int GetActualProgress();          ///< Gets the current progress percentage.
    bool IsUpdated(const QueueItem& anotherItem);         ///< Checks if the item has been updated compared to another.

    // --- Image and file handling methods ---
    void SetImagesPathsFromInfo();                            ///< Sets image paths based on available metadata.
    void PrepareImagesForClients(const wxString& targetDir);  ///< Prepares images for client-side usage.
    void PrepareImagesForServer();                            ///< Prepares images for server-side processing.
    void RemoveGeneratedImages();                             ///< Deletes generated images if they exist.
    QueueItem* RemoveRawImageData();                          ///< Clears raw image data to reduce memory usage.
    [[nodiscard]] QueueItem LoadImageInfos() const;           ///< Loads additional image metadata from existing sources and create a copy
    void ClearImageInfosData();                               ///< Clears image data, keep
    void SetOrReplaceImageInfo(const sd_gui_utils::networks::ImageInfo& imageInfo);
    inline void CopyFromProcess(QueueItem& other) {
        updated_at         = other.updated_at;
        update_index       = other.update_index;
        finished_at        = other.finished_at;
        started_at         = other.started_at;
        status             = other.status;
        stats              = other.stats;
        step               = other.step;
        steps              = other.steps;
        hash_fullsize      = other.hash_fullsize;
        hash_progress_size = other.hash_progress_size;
        time               = other.time;
        mode               = other.mode;
        status_message     = other.status_message;
        upscale_factor     = other.upscale_factor;
        rawImages          = other.rawImages;
    };
    sd_gui_utils::ImageInfo GetImageInfo(const std::string& id) const;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(QueueItem,
                                                id,
                                                created_at,
                                                updated_at,
                                                finished_at,
                                                started_at,
                                                params,
                                                status,
                                                //  event,
                                                stats,
                                                step,
                                                steps,
                                                hash_fullsize,
                                                hash_progress_size,
                                                time,
                                                model,
                                                hashes,
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
                                                server,
                                                image_info,
                                                rawImages,
                                                initial_image,
                                                mask_image)

#endif  // __HELPERS_QUEUE_ITEM