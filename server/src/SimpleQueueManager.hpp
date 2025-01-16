#ifndef __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#define __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#include "libs/SnowFlakeIdGenerarot.hpp"
class SimpleQueueManager {
public:
    SimpleQueueManager(std::string server_id, std::string jobs_path)
        : server_id(server_id), generator(server_id), jobs_path(jobs_path) {
    }

    void AddItem(QueueItem& item) {
        // lock this->mutex
        std::lock_guard<std::mutex> lock(this->mutex);
        if (item.id == 0) {
            auto id = this->generator.generateID(this->jobs.size());

            while (id < this->lastID) {
                id = this->lastID + 1;
            }

            item.id = id;
        }
        item.status     = QueueStatus::PENDING;
        item.event      = QueueEvents::ITEM_ADDED;
        item.created_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::cout << "Item added: " << item.id << std::endl;
        this->jobs.push_back(item);
        {
            this->StoreJobInFile(item.id);
        }
    }
    std::vector<QueueItem> GetJobList() {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->jobs;
    }
    auto UpdateJob(QueueItem& item) -> bool {
        // lock this->mutex
        std::lock_guard<std::mutex> lock(this->mutex);
        for (auto& job : this->jobs) {
            if (job.id == item.id && (item.update_index != job.update_index || item.status != job.status)) {
                job = item;
                {
                    this->StoreJobInFile(item.id);
                }
                return true;
            }
            return false;
        }
        return false;
        std::cout << "No item updated with id: " << item.id << std::endl;
    }

    auto GetItem(uint64_t id) -> QueueItem {
        // lock this->mutex
        //std::lock_guard<std::mutex> lock(this->mutex);
        for (auto& job : this->jobs) {
            if (job.id == id) {
                return job;
            }
        }
        return QueueItem();
    }

private:
    // mutex
    std::mutex mutex;
    std::vector<QueueItem> jobs;
    std::string server_id, jobs_path;
    sd_gui_utils::SnowflakeIDGenerator generator;
    uint64_t lastID = 0;
    void StoreJobInFile(uint64_t item_id) {
        if (item_id == 0) {
            return;
        }

        auto item = this->GetItem(item_id);
        if (item.id == 0) {
            return;
        }
        try {
            const nlohmann::json jsondata = item;
            const auto filepath           = wxString::Format("%s%slocal_%lu.json", this->jobs_path, wxFileName::GetPathSeparators(), item_id);
            wxFileName fn(filepath);
            wxFile file;
            if (file.Open(fn.GetAbsolutePath(), wxFile::write)) {
                file.Write(jsondata.dump());
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