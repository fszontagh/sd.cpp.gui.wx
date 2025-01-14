#ifndef __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#define __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#include "libs/SnowFlakeIdGenerarot.hpp"
class SimpleQueueManager {
    struct JobItem : public sd_gui_utils::networks::RemoteQueueItem {
        std::string initial_image;
        std::string mask_image;
        std::string status_message;
        JobItem(sd_gui_utils::networks::RemoteQueueItem& item) {
            *this = item;
        }
    };

public:
    SimpleQueueManager(std::string server_id)
        : server_id(server_id), generator(server_id) {
    }

    void AddItem(sd_gui_utils::networks::RemoteQueueItem& item) {
        if (item.id == 0) {
            auto id = this->generator.generateID(this->jobs.size());
            if (id < this->lastID) {
                id = this->lastID + 1;
            }
            item.id = id;
        }
        item.status = QueueStatus::PENDING;
        item.event  = QueueEvents::ITEM_ADDED;
        this->jobs.push_back(item);
    }
    std::vector<JobItem> GetJobList() { return this->jobs; }

private:
    std::vector<JobItem> jobs;
    std::string server_id;
    sd_gui_utils::SnowflakeIDGenerator generator;
    uint64_t lastID = 0;
};
#endif