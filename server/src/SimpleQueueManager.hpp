#ifndef __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
#define __SGUI_SERVER_SIMPLEQUEUEMANAGER_H_
class SimpleQueueManager {
    std::vector<sd_gui_utils::networks::RemoteQueueItem> jobs;
    std::string server_id;

private:
public:
    SimpleQueueManager(std::string server_id)
        : server_id(server_id) {
    }

    void AddItem(sd_gui_utils::networks::RemoteQueueItem& item) {
        if (item.id == 0) {
            sd_gui_utils::helpers::generateID(this->server_id);
        }
    }
};
#endif