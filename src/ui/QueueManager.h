#ifndef __SD_GUI_QUEUE_MANAGER
#define __SD_GUI_QUEUE_MANAGER

namespace QM {

    class QueueManager {
    public:
        QueueManager(wxEvtHandler* eventHandler, const wxString& jobsdir);
        ~QueueManager();
        auto AddItem(const QueueItem& _item, bool fromFile = false) -> uint64_t;
        auto AddItem(std::shared_ptr<QueueItem> _item, bool fromFile = false) -> uint64_t;
        auto GetNextId() -> uint64_t {
            int id = this->GetCurrentUnixTimestamp(false);
            while (id <= this->lastId) {
                id++;
            }
            return id;
        };
        void UpdateItem(const QueueItem& item);
        void UpdateItem(std::shared_ptr<QueueItem> item);
        auto GetItemPtr(uint64_t item_id) -> std::shared_ptr<QueueItem>;
        /**
         * @brief Get all the items in the queue as a string formatted like:
         *
         */
        std::shared_ptr<QueueItem> GetItemPtr(const QueueItem& item);
        const std::map<int, std::shared_ptr<QueueItem>> getList();
        std::shared_ptr<QueueItem> Duplicate(std::shared_ptr<QueueItem> item);
        std::shared_ptr<QueueItem> Duplicate(uint64_t id);
        // @brief Update the item too then update the status. This will store the list of the generated images too
        void SetStatus(QueueStatus status, std::shared_ptr<QueueItem>);
        void PauseAll();
        void RestartQueue();
        void UnPauseItem(std::shared_ptr<QueueItem> item);
        void PauseItem(std::shared_ptr<QueueItem> item);
        void SendEventToMainWindow(QueueEvents eventType, std::shared_ptr<QueueItem> item = nullptr);
        void OnThreadMessage(wxThreadEvent& e);
        void SaveJobToFile(uint64_t job_id);
        void SaveJobToFile(const QueueItem& item);
        auto DeleteJob(uint64_t job_id) -> bool;
        auto DeleteJob(const QueueItem& item) -> bool;
        auto IsRunning() -> bool;
        inline void resetRunning(std::shared_ptr<QueueItem> item, const std::string& reason) {
            if (this->QueueList.empty()) {
                return;
            }
            std::lock_guard<std::mutex> lock(queueMutex);
            if (this->QueueList.find(item->id) != this->QueueList.end()) {
                item->status_message = reason;
                this->SendEventToMainWindow(QueueEvents::ITEM_FAILED, this->QueueList[item->id]);
            }
        }
        inline void resetRunning(const std::string& reason) {
            if (this->QueueList.empty()) {
                return;
            }
            std::lock_guard<std::mutex> lock(queueMutex);
            if (this->currentItem == nullptr) {
                return;
            }
            if (this->QueueList.find(this->currentItem->id) != this->QueueList.end()) {
                if (reason.empty() == false) {
                    this->currentItem->status_message = reason;
                }
                this->SendEventToMainWindow(QueueEvents::ITEM_FAILED, this->QueueList[this->currentItem->id]);
            }
        }
        inline void resetRunning(const wxString& reason) {
            if (this->QueueList.empty()) {
                return;
            }
            std::lock_guard<std::mutex> lock(queueMutex);
            if (this->currentItem == nullptr) {
                return;
            }
            if (this->QueueList.find(this->currentItem->id) != this->QueueList.end()) {
                if (reason.empty() == false) {
                    this->currentItem->status_message = reason.utf8_string();
                }
                this->SendEventToMainWindow(QueueEvents::ITEM_FAILED, this->QueueList[this->currentItem->id]);
            }
        }
        inline std::shared_ptr<QueueItem> GetCurrentItem() { return this->currentItem; }

    private:
        std::mutex queueMutex;
        auto GetCurrentUnixTimestamp(bool milliseconds = false) -> int;
        void LoadJobListFromDir();
        wxString jobsDir;
        uint64_t lastId    = 0;
        uint64_t lastExtId = 0;
        auto GetAnId() -> uint64_t;
        std::shared_ptr<QueueItem> currentItem = nullptr;

        // @brief check if something is running or not
        bool isRunning = false;

        wxEvtHandler* eventHandler;
        wxWindow* parent;
        std::map<uint64_t, std::shared_ptr<QueueItem>> QueueList;
        std::map<uint64_t, std::shared_ptr<QueueItem>> RemoteQueueList;
    };
};

#endif