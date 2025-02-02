#include "QueueItem.h"

// --- Conversion and initialization methods ---
QueueItem QueueItem::ConvertToSharedMemory() {
    QueueItem newItem(*this);
    if (!newItem.image_info.empty()) {
        newItem.image_info.clear();
        newItem.image_info.shrink_to_fit();  // Optionally free memory
    }
    return newItem;
}

void QueueItem::ConvertFromSharedMemory(QueueItem item) {
    auto imginfo     = std::move(this->image_info);
    *this            = std::move(item);
    this->image_info = std::move(imginfo);
}

QueueItem QueueItem::convertFromNetwork(sd_gui_utils::networks::RemoteQueueItem&& item, wxString tempDir) {
    // QueueItem newItem(item);
    QueueItem newItem(item);
    for (sd_gui_utils::networks::ImageInfo& img : newItem.image_info) {
        if (!img.convertFromNetwork(tempDir)) {
            continue;
        }
    }
    return newItem;
}

sd_gui_utils::networks::RemoteQueueItem QueueItem::convertToNetwork(bool clear_images_data, std::string model_names_prefix) {
    sd_gui_utils::networks::RemoteQueueItem newItem(*this);

    if (clear_images_data) {
        for (sd_gui_utils::networks::ImageInfo& img : newItem.image_info) {
            img.data.clear();
            img.data_filename.clear();
            img.target_filename.clear();
        }
    }
    if (!model_names_prefix.empty()) {
        newItem.params.model_path  = newItem.params.model_path.empty() ? newItem.params.model_path : model_names_prefix + ":" + newItem.params.model_path;
        newItem.params.taesd_path  = newItem.params.taesd_path.empty() ? newItem.params.taesd_path : model_names_prefix + ":" + newItem.params.taesd_path;
        newItem.params.vae_path    = newItem.params.vae_path.empty() ? newItem.params.vae_path : model_names_prefix + ":" + newItem.params.vae_path;
        newItem.params.esrgan_path = newItem.params.esrgan_path.empty() ? newItem.params.esrgan_path : model_names_prefix + ":" + newItem.params.esrgan_path;
    }
    return newItem;
}

// --- Query methods ---
const wxString QueueItem::GetActualSpeed() const {
    wxString speed = "-.--s/it";
    if (this->time == 0.0f) {
        return speed;
    }
    if (this->status == QueueStatus::MODEL_LOADING || this->mode == SDMode::CONVERT) {
        speed = wxString::Format(this->time > 1.0f ? "%.2fs/it" : "%.2fit/s", this->time > 1.0f || this->time == 0 ? this->time : (1.0f / this->time));
    } else {
        wxString formatStr = this->time > 1.0f ? "%.2fs/it %d/%d" : "%.2fit/s %d/%d";
        speed              = wxString::Format(formatStr, (this->time > 1.0f ? this->time : (1.0f / this->time)), this->step, this->steps);
    }
    return speed;
}

const int QueueItem::GetActualProgress() {
    if (this->status == QueueStatus::PENDING) {
        return 0;
    }
    if (this->step == this->steps) {
        return 100;
    }
    float current_progress = 0.f;

    if (this->step > 0 && this->steps > 0) {
        current_progress = 100.f * (static_cast<float>(this->step) /
                                    static_cast<float>(this->steps));
    }
    return static_cast<int>(current_progress);
}

bool QueueItem::IsUpdated(const QueueItem& anotherItem) {
    return this->status != anotherItem.status ||
           this->update_index != anotherItem.update_index ||
           this->updated_at != anotherItem.updated_at;
}

// --- Image and file handling methods ---
void QueueItem::SetImagesPathsFromInfo() {
    for (auto& img : this->image_info) {
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::networks::ImageType::INITIAL)) {
            this->initial_image = img.target_filename;
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::networks::ImageType::MASK_USED)) {
            this->mask_image = img.target_filename;
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::networks::ImageType::CONTROLNET)) {
            this->params.control_image_path = img.target_filename;
        }
    }
}

void QueueItem::PrepareImagesForClients(const wxString& targetDir) {
    if (this->rawImages.empty()) {
        return;
    }
    int counter = 0;
    for (auto& img : this->rawImages) {
        sd_gui_utils::ImageInfo info;
        info.server_id = this->server;
        info.jobid     = this->id;

        wxImage imageFile;
        if (imageFile.LoadFile(img, wxBITMAP_TYPE_ANY)) {
            info.width  = imageFile.GetWidth();
            info.height = imageFile.GetHeight();
        }

        wxFileName newName(targetDir, wxString::Format("%" PRIu64 "_%d_generated.png", this->id, counter));
        wxFileName newNameB64(targetDir, wxString::Format("%" PRIu64 "_%d_generated.png.base64", this->id, counter));

        if (!wxFileExists(newName.GetAbsolutePath())) {
            wxRenameFile(img, newName.GetAbsolutePath());
        }
        if (!wxFileExists(newNameB64.GetAbsolutePath())) {
            sd_gui_utils::StoreBase64ToFile(newName.GetAbsolutePath().ToStdString(), newNameB64.GetAbsolutePath().ToStdString());
        }

        info.target_filename = newName.GetAbsolutePath().ToStdString();
        info.type            = sd_gui_utils::networks::ImageType::GENERATED | sd_gui_utils::networks::ImageType::MOVEABLE;
        info.md5_hash        = sd_gui_utils::calculateMD5(newName.GetAbsolutePath().ToStdString());

        this->image_info.emplace_back(info);
        counter++;
    }

    this->rawImages.clear();
}

void QueueItem::PrepareImagesForServer() {
    for (sd_gui_utils::networks::ImageInfo& img : this->image_info) {
        if (wxFileExists(img.data_filename)) {
            img.data.clear();
            sd_gui_utils::EncodeFileToBase64(img.data_filename, img.data);
            img.md5_hash = sd_gui_utils::calculateMD5(img.data_filename);
        }
    }
}

QueueItem* QueueItem::RemoveRawImageData() {
    if (this->rawImages.empty()) {
        return this;
    }
    for (auto& img : this->image_info) {
        img.data.clear();
    }
    return this;
}

/**
 * @brief Loads additional image metadata from existing sources.
 *
 * This method creates a copy of the current QueueItem and loads Base64-encoded
 * image data if missing. It does not modify the original instance.
 *
 * @return A new QueueItem instance with updated image data.
 */
QueueItem QueueItem::LoadImageInfos() const {
    QueueItem copy = *this;  // Create a copy of the current object

    if (copy.image_info.empty()) {
        return copy;
    }

    for (auto& img : copy.image_info) {  // Work on the copy's image_info
        if (img.data.empty() && !img.target_filename.empty()) {
            wxFileName b64Name(img.target_filename);
            wxFileName targetFileName(img.target_filename);
            b64Name.SetEmptyExt();
            b64Name.SetExt(targetFileName.GetExt() + ".base64");

            if (!wxFileExists(b64Name.GetAbsolutePath())) {
                sd_gui_utils::StoreBase64ToFile(img.target_filename, b64Name.GetAbsolutePath().ToStdString());
            }
            img.data = sd_gui_utils::ReadBase64FromFile(b64Name.GetAbsolutePath().ToStdString());
        }
    }

    return copy;
}

void QueueItem::RemoveGeneratedImages() {
    if (this->image_info.empty()) {
        return;
    }
    for (auto it = this->image_info.begin(); it != this->image_info.end();) {
        if (sd_gui_utils::hasImageType(it->type, sd_gui_utils::networks::ImageType::GENERATED)) {
            it = this->image_info.erase(it);
        } else {
            ++it;
        }
    }
}

/**
 * @brief Clears the image data for all images in the job.
 *
 * This method is typically used when the job is stored in file at the server and the
 * image data is no longer needed. It does not modify the image
 * metadata, only the image data.
 */
void QueueItem::ClearImageInfosData() {
    if (this->image_info.empty()) {
        return;
    }
    for (auto& img : this->image_info) {
        img.data.clear();
    }
}
void QueueItem::SetOrReplaceImageInfo(const sd_gui_utils::networks::ImageInfo& imageInfo) {
    for (auto it = this->image_info.begin(); it != this->image_info.end();) {
        if (it->GetId() == imageInfo.GetId()) {
            *it = imageInfo;
            return;
        } else {
            ++it;
        }
    }

    this->image_info.emplace_back(imageInfo);
}
sd_gui_utils::ImageInfo QueueItem::GetImageInfo(const std::string& id) const {
    for (const auto& img : this->image_info) {
        if (img.GetId() == id) {
            return img;
        }
    }
    return sd_gui_utils::ImageInfo();
}
