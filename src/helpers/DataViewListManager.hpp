#ifndef __SDGUI_HELPERS_DATAVIEWLISTMANAGER__
#define __SDGUI_HELPERS_DATAVIEWLISTMANAGER__

class DataViewListManager {
public:
    DataViewListManager(wxDataViewListCtrl* parent, std::shared_ptr<QM::QueueManager> queueManager)
        : parent(parent), store(parent->GetStore()), queueManager(queueManager) {}

    void DeleteItems(const wxDataViewItemArray& selected_items) {
        for (const auto& item : selected_items) {
            const auto row = parent->ItemToRow(item);
            auto job_id    = store->GetItemData(item);
            auto job_item  = this->queueManager->GetItemPtr(job_id);

            if (!job_item) {
                continue;
            }
            if (row != wxNOT_FOUND && job_item->status & QueueStatusFlags::DELETABLE_FLAG) {
                this->queueManager->DeleteJob(job_id);
                parent->DeleteItem(row);
            }
        }
    }

    std::shared_ptr<QueueItem> GetQueueItem(const unsigned int row) {
        auto item   = store->GetItem(row);
        auto job_id = store->GetItemData(item);
        return this->queueManager->GetItemPtr(job_id);
    }

    std::shared_ptr<QueueItem> GetQueueItem(const wxDataViewItem& item) {
        auto job_id = store->GetItemData(item);
        return this->queueManager->GetItemPtr(job_id);
    }

    std::vector<std::shared_ptr<QueueItem>> GetQueueItems(const wxDataViewItemArray& selected_items) {
        std::vector<std::shared_ptr<QueueItem>> items;
        for (const auto& item : selected_items) {
            const auto row = parent->ItemToRow(item);
            auto job_id    = store->GetItemData(item);
            auto job_item  = this->queueManager->GetItemPtr(job_id);
            if (row != wxNOT_FOUND && job_item->status) {
                items.push_back(job_item);
            }
        }
        return items;
    }

private:
    wxDataViewListCtrl* parent                     = nullptr;
    wxDataViewListStore* store                     = nullptr;
    std::shared_ptr<QM::QueueManager> queueManager = nullptr;
};

#endif  // __SDGUI_HELPERS_DATAVIEWLISTMANAGER__