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

    void AddItem(QueueItem& newItem) {
        std::lock_guard<std::mutex> lock(this->mutex);
        std::shared_ptr<QueueItem> item = std::make_shared<QueueItem>(newItem);
        if (item->id == 0) {
            auto id = this->generator.generateID(this->jobs.size());

            while (id < this->lastID) {
                id = this->lastID + 1;
            }

            item->id = id;
        }
        wxString output_file_prefix = wxString::Format("%s%slocal_%lu", this->jobs_path, wxFileName::GetPathSeparators(), item->id);
        item->params.output_path    = wxFileName::CreateTempFileName(output_file_prefix).ToStdString();
        item->status                = QueueStatus::PENDING;
        // item->event                 = QueueEvents::ITEM_ADDED;
        item->created_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        item->server     = this->server_id;
        std::cout << "Item added: " << item->id << std::endl;
        this->jobs.push_back(item);
        {
            this->StoreJobInFile(item->id);
        }
    }
    std::vector<std::shared_ptr<QueueItem>> GetJobList() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->jobs;
    }
    std::shared_ptr<QueueItem> GetCurrentJob() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->currentItem;
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
            list.push_back(*item);
        }
        return list;
    }
    auto UpdateJob(QueueItem& item) -> bool {
        // lock this->mutex
        std::lock_guard<std::mutex> lock(this->mutex);
        for (auto& job : this->jobs) {
            if (job->id == item.id && (item.update_index != job->update_index || item.status != job->status)) {
                *job = item;
                {
                    this->StoreJobInFile(item.id);
                }
                if (job->status == QueueStatus::DONE) {
                    this->currentItem = nullptr;
                }
                return true;
            }
        }
        return false;
        std::cout << "No item updated with id: " << item.id << std::endl;
    }

    auto GetItem(uint64_t id) -> std::shared_ptr<QueueItem> {
        // lock this->mutex
        // std::lock_guard<std::mutex> lock(this->mutex);
        for (auto& job : this->jobs) {
            if (job->id == id) {
                return job;
            }
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
                const nlohmann::json jsondata = nlohmann::json::parse(data);
                auto item                     = std::make_shared<QueueItem>(jsondata.get<QueueItem>());
                bool need_update              = false;
                if (item->id != 0) {
                    if (item->status & QueueStatusFlags::RUNNING_FLAG) {
                        item->status     = QueueStatus::FAILED;
                        item->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        need_update      = true;
                        std::cout << "Changed job id status to failed:  " << item->id << std::endl;
                    }
                    if (item->status == QueueStatus::PENDING) {
                        item->status = QueueStatus::PAUSED;
                        std::cout << "Chenged job id status to paused: " << item->id << std::endl;
                        need_update = true;
                    }
                    this->jobs.push_back(item);
                    this->lastID = item->id;
                    if (need_update) {
                        this->StoreJobInFile(item->id);
                    }
                }
            }
            cont = dir.GetNext(&path);
        }
    }

private:
    // mutex
    std::mutex mutex;
    std::vector<std::shared_ptr<QueueItem>> jobs;
    std::string server_id, jobs_path;
    sd_gui_utils::SnowflakeIDGenerator generator;
    uint64_t lastID                        = 0;
    std::shared_ptr<QueueItem> currentItem = nullptr;
    void StoreJobInFile(uint64_t item_id) {
        if (item_id == 0) {
            return;
        }

        auto item = this->GetItem(item_id);
        if (item->id == 0) {
            return;
        }
        try {
            const nlohmann::json jsondata = *item;
            const auto filepath           = wxString::Format("%s%slocal_%lu.json", this->jobs_path, wxFileName::GetPathSeparators(), item_id);
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