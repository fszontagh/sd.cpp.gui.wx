#ifndef __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#define __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#include "libs/SnowFlakeIdGenerarot.hpp"
class SimpleQueueManager {
public:
    SimpleQueueManager(std::shared_ptr<ServerConfig> config)
        : server_id(config->server_id),
          jobs_path(config->GetJobsPath()),
          rewrite_server_id_in_jobfiles(config->rewrite_server_id_in_jobfiles),
          generator(config->server_id) {}
    size_t GetJobCount() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->jobs.size();
    }
    uint64_t GenerateNextId() {
        std::lock_guard<std::mutex> lock(this->mutex);

        auto id = this->generator.generateID(this->jobs.size());
        // auto id = this->jobs.end()->first

        while (id < this->lastID) {
            id = this->lastID + 1;
        }
        return id;
    }
    std::shared_ptr<QueueItem> DuplicateItem(uint64_t job_id) {
        // std::lock_guard<std::mutex> lock(this->mutex);
        if (this->jobs.contains(job_id)) {
            auto oldJob        = this->jobs.at(job_id);
            auto newJob        = std::make_shared<QueueItem>(*oldJob);
            newJob->id         = this->GenerateNextId();
            newJob->status     = QueueStatus::PAUSED;
            newJob->created_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            newJob->server     = this->server_id;
            newJob->time       = 0;
            newJob->stats      = {};
            newJob->started_at = 0;
            newJob->RemoveGeneratedImages();  // clean up generated images
            this->jobs[newJob->id] = newJob;
            this->lastID           = (this->lastID < newJob->id) ? newJob->id : this->lastID;
            this->StoreJobInFile(newJob);
            return newJob;
        }
        return nullptr;
    }

    std::shared_ptr<QueueItem> PauseItem(uint64_t job_id, wxString& error) {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->jobs.contains(job_id)) {
            auto job = this->jobs.at(job_id);
            if (job->status & QueueStatusFlags::PAUSABLE_FLAG) {
                job->status = QueueStatus::PAUSED;
                this->StoreJobInFile(job);
                return job;
            }
            error = wxString::Format(_("Job cannot be paused: %" PRIu64), job_id);
            return nullptr;
        }
        error = _("Job not found");
        return nullptr;
    }

    std::shared_ptr<QueueItem> ResumeItem(uint64_t job_id, wxString& error) {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->jobs.contains(job_id)) {
            auto job = this->jobs.at(job_id);

            if (job->status & QueueStatusFlags::RESUMEABLE_FLAG) {
                job->status = QueueStatus::PENDING;
                this->StoreJobInFile(job);
                return job;
            }
            error = wxString::Format(_("Job cannot be resumed: %" PRIu64), job_id);
            return nullptr;
        }
        error = _("Job not found");
        return nullptr;
    }

    std::shared_ptr<QueueItem> AddItem(QueueItem& newItem) {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::shared_ptr<QueueItem> item = std::make_shared<QueueItem>(newItem);
        if (item->id == 0) {
            auto id  = this->GenerateNextId();
            item->id = id;
        }
        item->status         = QueueStatus::PENDING;
        item->created_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        item->server         = this->server_id;
        this->jobs[item->id] = item;
        this->lastID         = (this->lastID < item->id) ? item->id : this->lastID;

        this->StoreJobInFile(this->jobs[item->id]);
        return this->jobs[item->id];
    }
    std::map<uint64_t, std::shared_ptr<QueueItem>> GetJobList() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->jobs;
    }
    std::shared_ptr<QueueItem> GetCurrentJob() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->currentItem;
    }
    void ChangeCurrentJobStatus(QueueStatus status) {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->currentItem != nullptr) {
            this->currentItem->status = status;
        }
    }
    std::shared_ptr<QueueItem> GetNextPendingJob() {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->jobs.empty()) {
            return nullptr;
        }
        for (auto it = this->jobs.begin(); it != this->jobs.end(); it++) {
            if ((*it).second->status == QueueStatus::PENDING) {
                return (*it).second;
            }
        }
        return nullptr;
    }
    void SetCurrentJob(std::shared_ptr<QueueItem> item) {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->currentItem = item;
    }
    void DeleteCurrentJob() {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->currentItem = nullptr;
    }
    std::vector<QueueItem> GetJobListCopy() {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::vector<QueueItem> list;
        for (const auto& item : this->jobs) {
            list.insert(list.begin(), *item.second);  // add to reverse order :)
        }
        return list;
    }
    auto UpdateCurrentJob(QueueItem& item, const std::string& target_dir) -> bool {
        std::lock_guard<std::mutex> lock(this->mutex);

        if (this->jobs.contains(item.id)) {
            auto j = this->jobs[item.id];
            if (item.status == QueueStatus::PENDING) {
                return false;
            }
            if (j->IsUpdated(item)) {
                auto oldJob = *j;
                //*j          = item;
                j->ConvertFromSharedMemory(item);

                if (j->status == QueueStatus::DONE) {
                    j->PrepareImagesForClients(target_dir);  // convert ONLY the generated images to base64 and generate image ID
                    if (this->currentItem && this->currentItem->id == oldJob.id) {
                        this->currentItem = nullptr;
                    }
                }
                if (j->status == QueueStatus::FAILED) {
                    if (this->currentItem && this->currentItem->id == oldJob.id) {
                        this->currentItem = nullptr;
                    }
                }
                this->StoreJobInFile(j);
                return true;
            }
            return false;
        }
        return false;
    }

    auto DeleteItem(uint64_t item_id, std::string& error_str) -> bool {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->jobs.contains(item_id)) {
            if (this->currentItem != nullptr && this->currentItem->id == item_id) {
                error_str = _("Cannot delete current job");
                return false;
            }
            this->jobs.erase(item_id);
            // delete the json file, don't throw error if not found TODO: better handle this situation
            wxFileName jobFile(this->jobs_path, wxString::Format("local_%" PRIu64 ".json", item_id));
            if (wxFileExists(jobFile.GetAbsolutePath())) {
                wxRemoveFile(jobFile.GetAbsolutePath());
            }
            return true;
        }
        error_str = _("Item not found");
        return false;
    }
    auto GetItem(uint64_t id) -> std::shared_ptr<QueueItem> {
        if (this->jobs.contains(id)) {
            return this->jobs.at(id);
        }
        return nullptr;
    }
    void LoadJobListFromDir() {
        if (!wxDirExists(this->jobs_path)) {
            wxMkdir(this->jobs_path);
            return;
        }

        const wxDir dir(this->jobs_path);
        if (!dir.IsOpened()) {
            wxLogMessage(_("Error opening directory: %s"), this->jobs_path);
            return;
        }

        wxString path;
        bool cont = dir.GetFirst(&path, wxEmptyString, wxDIR_FILES);
        while (cont) {
            const wxFileName fn(this->jobs_path, path);
            if (fn.IsDir()) {
                cont = dir.GetNext(&path);
                continue;
            }

            if (fn.GetExt() != "json") {
                cont = dir.GetNext(&path);
                continue;
            }

            wxFile file;
            if (file.Open(fn.GetAbsolutePath(), wxFile::read)) {
                wxString data;
                file.ReadAll(&data);
                file.Close();
                try {
                    const nlohmann::json jsondata = nlohmann::json::parse(data);
                    auto item                     = std::make_shared<QueueItem>(jsondata.get<QueueItem>());
                    bool need_update              = false;
                    if (item->id != 0) {
                        if (item->server != this->server_id) {
                            wxLogMessage("Server id mismatch in job id: %" PRIu64, item->id);
                            if (this->rewrite_server_id_in_jobfiles) {
                                wxLogMessage("Changed server id in job id: %" PRIu64 " from: %s to: %s", item->id, item->server.c_str(), this->server_id.c_str());
                                item->server = this->server_id;
                                need_update  = true;
                            } else {
                                wxRenameFile(fn.GetAbsolutePath(), fn.GetAbsolutePath() + ".wrong_version");
                                cont = dir.GetNext(&path);
                                continue;
                            }
                        }
                        if (item->status & QueueStatusFlags::RUNNING_FLAG) {
                            item->status     = QueueStatus::FAILED;
                            item->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                            need_update      = true;
                            wxLogInfo("Changed job id status to failed:  %" PRIu64, item->id);
                        }
                        if (item->status == QueueStatus::PENDING) {
                            item->status = QueueStatus::PAUSED;
                            wxLogInfo("Chenged job id status to paused: %" PRIu64, item->id);
                            need_update = true;
                        }
                        this->jobs[item->id] = item;
                        this->lastID         = this->lastID < item->id ? item->id : this->lastID;
                        if (need_update) {
                            this->StoreJobInFile(item);
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing json: " << fn.GetAbsolutePath() << " - " << e.what() << std::endl;
                }
            }
            cont = dir.GetNext(&path);
        }
    }

private:
    // mutex
    std::mutex mutex;
    std::map<uint64_t, std::shared_ptr<QueueItem>> jobs;
    std::string server_id, jobs_path;
    bool rewrite_server_id_in_jobfiles = false;
    sd_gui_utils::SnowflakeIDGenerator generator;
    uint64_t lastID                        = 0;
    std::shared_ptr<QueueItem> currentItem = nullptr;

    void StoreJobInFile(std::shared_ptr<QueueItem> item) {
        if (!item || item->id == 0) {
            return;
        }

        try {
            auto storeItem = *item;
            storeItem.ClearImageInfosData();  // Remove all RAW image data (base64)

            nlohmann::json jsonData;
            try {
                jsonData = storeItem;
            } catch (const std::exception& e) {
                std::cerr << "Error converting item to JSON: " << e.what() << std::endl;
                return;
            }

            // JSON string létrehozása
            std::string jsonString;
            try {
                jsonString = jsonData.dump(2);
            } catch (const std::exception& e) {
                std::cerr << "Error serializing JSON: " << e.what() << std::endl;
                return;
            }

            // Fájlnév meghatározása
            const auto filepath = wxString::Format("local_%" PRIu64 ".json", item->id);
            wxFileName fn(this->jobs_path, filepath);
            wxString tempFilePath = fn.GetAbsolutePath() + ".tmp";  // Temporary file path

            wxFile file;
            if (file.Open(tempFilePath, wxFile::write)) {
                file.Write(jsonString);
                file.Close();
                if (fn.FileExists()) {
                    wxRemoveFile(fn.GetAbsolutePath());
                }
                if (!wxRenameFile(tempFilePath, fn.GetAbsolutePath())) {
                    std::cerr << "Error renaming temp file to: " << filepath << std::endl;
                }
            } else {
                std::cerr << "Error opening file for writing: " << tempFilePath << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error in StoreJobInFile: " << e.what() << std::endl;
        }
    }
};
#endif