#ifndef __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#define __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#include "libs/SnowFlakeIdGenerarot.hpp"
class SimpleQueueManager {
public:
    SimpleQueueManager(std::string server_id, std::string jobs_path)
        : server_id(server_id), generator(server_id), jobs_path(jobs_path) {
    }
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
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->jobs.contains(job_id)) {
            auto oldJob            = this->jobs.at(job_id);
            auto newJob            = std::make_shared<QueueItem>(*oldJob);
            newJob->id             = this->GenerateNextId();
            newJob->status         = QueueStatus::PAUSED;
            newJob->created_at     = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            newJob->server         = this->server_id;
            newJob->RemoveGeneratedImages(); // clean up generated images
            this->jobs[newJob->id] = newJob;
            this->lastID           = (this->lastID < newJob->id) ? newJob->id : this->lastID;
            this->StoreJobInFile(newJob);
            return newJob;
        }
        return nullptr;
    }

    std::shared_ptr<QueueItem> AddItem(QueueItem& newItem) {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::shared_ptr<QueueItem> item = std::make_shared<QueueItem>(newItem);
        if (item->id == 0) {
            auto id  = this->GenerateNextId();
            item->id = id;
        }
        item->status     = QueueStatus::PENDING;
        item->created_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        item->server     = this->server_id;
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
                *j          = item;

                if (j->status == QueueStatus::DONE) {
                    j->prepareImagesForClients(target_dir);  // convert images to base64 and generate image ID
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
            std::cerr << "Error opening directory: " << this->jobs_path << std::endl;
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
    sd_gui_utils::SnowflakeIDGenerator generator;
    uint64_t lastID                        = 0;
    std::shared_ptr<QueueItem> currentItem = nullptr;
    void StoreJobInFile(std::shared_ptr<QueueItem> item) {
        if (!item || item->id == 0) {
            return;
        }
        try {
            const nlohmann::json jsondata = *item;
            const auto filepath           = wxString::Format("%s%slocal_%" PRIu64 ".json", this->jobs_path, wxFileName::GetPathSeparators(), item->id);
            wxFileName fn(filepath);
            wxFile file;
            if (file.Open(fn.GetAbsolutePath(), wxFile::write)) {
                file.Write(jsondata.dump(2));
                file.Close();
            } else {
                std::cerr << "Error opening file: " << filepath << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error serializing item to JSON: " << e.what() << std::endl;
        }
    }
};
#endif