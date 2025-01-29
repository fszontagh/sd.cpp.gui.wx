#ifndef HELPERS_QUEUE_ITEM_H
#define HELPERS_QUEUE_ITEM_H


struct QueueItem : public sd_gui_utils::networks::RemoteQueueItem {
    std::vector<std::string> rawImages = {};
    std::string initial_image          = "";
    std::string mask_image             = "";
    inline wxString GetActualSpeed() {
        wxString speed = "";
        if (this->time == 0.0f) {
            return wxString("0s/it");
        }
        if (this->status == QueueStatus::MODEL_LOADING || this->mode == SDMode::CONVERT) {
            speed = wxString::Format(this->time > 1.0f ? "%.2fs/it" : "%.2fit/s", this->time > 1.0f || this->time == 0 ? this->time : (1.0f / this->time));
        } else {
            wxString formatStr = this->time > 1.0f ? "%.2fs/it %d/%d" : "%.2fit/s %d/%d";
            speed              = wxString::Format(formatStr, (this->time > 1.0f ? this->time : (1.0f / this->time)), this->step, this->steps);
        }
        return speed;
    };
    inline int GetActualProgress() {
        float current_progress = 0.f;
        if (this->status == QueueStatus::PENDING) {
            return 0;
        }

        if (this->step > 0 && this->steps > 0) {
            current_progress = 100.f * (static_cast<float>(this->step) /
                                        static_cast<float>(this->steps));
        }
        if (this->step == this->steps) {
            current_progress = 100.f;
        }
        return static_cast<int>(current_progress);
    }
    inline bool IsUpdated(const QueueItem& newJob) {
        return this->status != newJob.status ||
               this->update_index != newJob.update_index ||
               this->updated_at != newJob.updated_at;
    }
    inline void prepareImagesForClients(wxString targetDir) {
        int counter = 0;
        if (this->rawImages.empty()) {
            return;
        }

        for (auto& img : this->rawImages) {
            // wxLogInfo("QueueItem::prepareImagesForClients: %s", img);
            sd_gui_utils::ImageInfo info;
            info.server_id = this->server;
            info.jobid     = this->id;

            wxImage imageFile;
            if (imageFile.LoadFile(img, wxBITMAP_TYPE_ANY) == true) {
                info.width  = imageFile.GetWidth();
                info.height = imageFile.GetHeight();
            }

            wxFileName newName(targetDir, wxString::Format("%" PRIu64 "_%d_generated.png", this->id, counter));
            wxFileName md5name(newName);
            md5name.SetExt("png.md5");

            wxFileName base64Name(newName);
            base64Name.SetExt("png.base64");
            if (!wxFileExists(newName.GetAbsolutePath())) {
                wxRenameFile(img, newName.GetAbsolutePath());
            }

            info.target_filename = newName.GetAbsolutePath().ToStdString();
            info.type            = sd_gui_utils::networks::ImageType::GENERATED | sd_gui_utils::networks::ImageType::MOVEABLE;
            if (wxFileExists(md5name.GetFullPath()) == true) {
                wxFile f;
                wxString md5;
                if (f.Open(md5name.GetFullPath(), wxFile::read)) {
                    f.ReadAll(&md5);
                    f.Close();
                }
                info.id = md5.ToStdString();

            } else {
                info.id = sd_gui_utils::calculateMD5(newName.GetAbsolutePath().ToStdString());
                wxFile f;
                if (f.Open(md5name.GetFullPath(), wxFile::write)) {
                    f.Write(wxString(info.id.data(), info.id.size()));
                    f.Close();
                }
            }
            // generate base64
            if (wxFileExists(base64Name.GetFullPath()) == false) {
                if (sd_gui_utils::EncodeFileToBase64(newName.GetAbsolutePath().ToStdString(), info.data) == false) {
                    continue;
                }
                wxFile tmpFile;
                if (tmpFile.Open(base64Name.GetFullPath(), wxFile::write)) {
                    tmpFile.Write(wxString(info.data.data(), info.data.size()));
                    tmpFile.Close();
                }

            } else {
                wxString base64;
                wxFile f;
                if (f.Open(base64Name.GetFullPath(), wxFile::read)) {
                    f.ReadAll(&base64);
                    f.Close();
                }
                info.data = base64.ToStdString();
            }
            this->image_info.emplace_back(info);
            counter++;
        }

        this->rawImages.clear();
    }
    inline void PrepareImagesForServer() {
        for (sd_gui_utils::networks::ImageInfo& img : this->image_info) {
            if (wxFileExists(img.data_filename)) {
                img.data.clear();
                sd_gui_utils::EncodeFileToBase64(img.data_filename, img.data);
                img.id = sd_gui_utils::calculateMD5(img.data_filename);
            }
        }
    }
    inline QueueItem* RemoveRawImageData() {
        if (this->rawImages.empty()) {
            return this;
        }
        for (auto it = this->image_info.begin(); it != this->image_info.end();) {
            if (!it->data.empty()) {
                it->data.clear();
            }
        }
        return this;
    }
    inline QueueItem* LoadImageInfos() {
        if (this->image_info.empty()) {
            return this;
        }
        for (auto it = this->image_info.begin(); it != this->image_info.end();) {
            if (it->data.empty() && it->target_filename.empty() == false) {
                wxFileName b64file(it->target_filename);
                b64file.SetExt("png.base64");
                if (wxFileExists(b64file.GetFullPath()) == true) {
                    wxFile f;
                    wxString base64string;
                    if (f.Open(b64file.GetFullPath(), wxFile::read)) {
                        f.ReadAll(&base64string);
                        it->data = base64string.ToStdString();
                        f.Close();
                    }
                }
            }
        }
    }
    inline QueueItem* RemoveGeneratedImages() {
        if (this->image_info.empty()) {
            return this;
        }
        for (auto it = this->image_info.begin(); it != this->image_info.end();) {
            if (sd_gui_utils::hasImageType(it->type, sd_gui_utils::networks::ImageType::GENERATED)) {
                it = this->image_info.erase(it);
            } else {
                it++;
            }
        }
        return this;
    }
    inline QueueItem ConvertToSharedMemory() {
        QueueItem newItem(*this);
        if (!newItem.image_info.empty()) {
            newItem.image_info.clear();
            newItem.image_info.shrink_to_fit();  // Opció: memória felszabadítása
        }
        return newItem;
    }

    inline void ConvertFromSharedMemory(QueueItem item) {
        auto imginfo     = std::move(this->image_info);
        *this            = std::move(item);
        this->image_info = std::move(imginfo);
    }

    inline void SetImagesPathsFromInfo() {
        for (auto& img : this->image_info) {
            if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::networks::ImageType::INITIAL)) {
                this->initial_image = img.target_filename;
            }
            if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::networks::ImageType::MASK_USED)) {
                this->mask_image = img.target_filename;
            }
        }
    }  // SetImagesPathsFromInfo

    inline sd_gui_utils::networks::RemoteQueueItem convertToNetwork(bool clear_images_data = true, std::string model_names_prefix = "") {
        sd_gui_utils::networks::RemoteQueueItem newItem(*this);

        if (clear_images_data == true) {
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
    }  // convertToNetwork

    inline static QueueItem convertFromNetwork(const sd_gui_utils::networks::RemoteQueueItem& item, wxString tempDir = wxFileName::GetTempDir()) {
        QueueItem newItem(item);
        // newItem.image_data.clear();
        for (sd_gui_utils::networks::ImageInfo& img : newItem.image_info) {
            img.target_filename = wxFileName(tempDir, wxString::Format("%" PRIu64 "_%s_%s.png", item.id, img.id, item.server)).GetAbsolutePath().ToStdString();
            if (wxFileExists(img.target_filename) == false && img.data.empty() == false) {
                if (sd_gui_utils::DecodeBase64ToFile(img.data, img.target_filename) == false) {
                    continue;
                }
            }
        }
        return newItem;
    }  // convertFromNetwork
};  // QueueItem

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