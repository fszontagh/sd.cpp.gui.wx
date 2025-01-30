#ifndef __SDGUI_HELPERS_DATAVIEWLISTMANAGER__
#define __SDGUI_HELPERS_DATAVIEWLISTMANAGER__

class DataViewListManager {
public:
    enum class queueJobColumns : unsigned int {
        ID             = 1 << 0,  // 1
        CREATED_AT     = 1 << 1,  // 2
        TYPE           = 1 << 2,  // 4
        SERVER         = 1 << 3,
        MODEL          = 1 << 4,   // 8
        SAMPLER        = 1 << 5,   // 16
        SEED           = 1 << 6,   // 32
        PROGRESS       = 1 << 7,   // 64
        SPEED          = 1 << 8,   // 128
        STATUS         = 1 << 9,   // 256
        STATUS_MESSAGE = 1 << 10,  // 512
        N_COUNTER      = 1 << 11   // 2048
    };
    inline static std::unordered_map<DataViewListManager::queueJobColumns, unsigned int> RowNumber = {
        {DataViewListManager::queueJobColumns::ID, 1},
        {DataViewListManager::queueJobColumns::CREATED_AT, 2},
        {DataViewListManager::queueJobColumns::TYPE, 3},
        {DataViewListManager::queueJobColumns::MODEL, 4},
        {DataViewListManager::queueJobColumns::SAMPLER, 5},
        {DataViewListManager::queueJobColumns::SEED, 6},
        {DataViewListManager::queueJobColumns::PROGRESS, 7},
        {DataViewListManager::queueJobColumns::SPEED, 8},
        {DataViewListManager::queueJobColumns::STATUS, 9},
        {DataViewListManager::queueJobColumns::STATUS_MESSAGE, 10},
        {DataViewListManager::queueJobColumns::SERVER, 11},
        {DataViewListManager::queueJobColumns::N_COUNTER, 12},

    };
    inline static unsigned int RowToInt(DataViewListManager::queueJobColumns type) {
        return DataViewListManager::RowNumber.at(type);
    }
    inline static bool hasFlag(DataViewListManager::queueJobColumns value, DataViewListManager::queueJobColumns flag) {
        return (static_cast<unsigned int>(value) & static_cast<unsigned int>(flag)) != 0;
    }

    DataViewListManager(wxDataViewListCtrl* parent, std::shared_ptr<QM::QueueManager> queueManager, sd_gui_utils::config* cfg)
        : parent(parent), store(parent->GetStore()), queueManager(queueManager), config(cfg) {}

    void DeleteItems(const wxDataViewItemArray& selected_items) {
        for (const auto& item : selected_items) {
            const auto row = parent->ItemToRow(item);
            auto job_id    = store->GetItemData(item);
            auto job_item  = this->queueManager->GetItemPtr(job_id);

            if (!job_item) {
                continue;
            }
            if (row != wxNOT_FOUND && job_item->status & QueueStatusFlags::DELETABLE_FLAG) {
                this->DeleteItem(job_id);
                // this->queueManager->DeleteJob(job_id);
                // parent->DeleteItem(row);
            }
        }
    }
    void DeleteItems(std::vector<wxDataViewItem> items) {
        wxDataViewItemArray selected_items;
        for (const auto& item : items) {
            selected_items.Add(item);
        }
        this->DeleteItems(selected_items);
    }

    /// \brief Delete an item from the dataview and the queue manager
    /// \param job_id The id of the job to delete
    /// \details This function will delete the item from the dataview and the queue manager by iterating through all the items in the dataview and checking if the id matches.
    void DeleteItem(uint64_t job_id) {
        for (int i = this->parent->GetItemCount() - 1; i >= 0; i--) {
            auto item = this->parent->RowToItem(i);
            auto id   = this->parent->GetItemData(item);
            if (id == job_id) {
                this->parent->DeleteItem(i);
                this->queueManager->DeleteJob(job_id);
            }
        }
    }

    std::vector<uint64_t> GetSelectedItemsId(wxDataViewItemArray selected_items) {
        std::vector<uint64_t> list;
        for (const auto& item : selected_items) {
            auto job_id    = store->GetItemData(item);
            list.emplace_back(job_id);
        }
        return list;
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

    bool ItemExists(uint64_t item_id) {
        for (unsigned int i = 0; i < this->store->GetItemCount(); i++) {
            if (this->store->GetItemData(this->store->GetItem(i)) == item_id) {
                return true;
            }
        }
        return false;
    }

    void AddItem(std::shared_ptr<QueueItem> item) {
        wxVector<wxVariant> data;

        auto created_at     = sd_gui_utils::formatUnixTimestampToDate(item->created_at);
        wxString serverName = _("local");

        if (!item->server.empty()) {
            auto srv = this->config->GetTcpServer(item->server);
            if (srv) {
                serverName = srv->GetName();
            } else {
                std::cerr << "Server not found: " << item->server << std::endl;
            }
        }

        data.push_back(wxVariant(wxString::Format("%" PRIu64, item->id)));
        data.push_back(wxVariant(created_at));
        data.push_back(wxVariant(modes_str[item->mode]));
        data.push_back(wxVariant(serverName));
        data.push_back(wxVariant(item->model));

        if (item->mode == SDMode::UPSCALE || item->mode == SDMode::CONVERT) {
            data.push_back(wxVariant("--"));  // sample method
            data.push_back(wxVariant("--"));  // seed
        } else {
            data.push_back(wxVariant(sd_gui_utils::samplerUiName.at(item->params.sample_method)));
            data.push_back(wxVariant(std::to_string(item->params.seed)));
        }

        data.push_back(item->status == QueueStatus::DONE ? 100 : 1);  // progressbar
        data.push_back(wxString(item->GetActualSpeed()));
        data.push_back(wxVariant(wxGetTranslation(QueueStatus_GUI_str.at(item->status))));  // status
        data.push_back(wxVariant(item->status_message));

        if (item->server.empty() == false) {
            auto srv = this->config->GetTcpServer(item->server);
            if (srv != nullptr) {
                data.push_back(wxVariant(srv->GetName()));
            } else {
                data.push_back(wxVariant(_("deleted server")));
            }
        } else {
            data.push_back(wxVariant(_("local")));
        }
        this->parent->PrependItem(data, wxUIntPtr(item->id));
    }

    void RemoveRemoteItems(const std::string& server_id) {
        if (server_id.empty()) {
            return;
        }
        for (int i = this->parent->GetItemCount() - 1; i >= 0; i--) {
            auto item  = this->parent->RowToItem(i);
            auto id    = this->parent->GetItemData(item);
            auto qitem = this->queueManager->GetItemPtr(id);
            if (!qitem) {
                this->parent->DeleteItem(i);
                continue;
            }
            if (qitem->server == server_id) {
                this->parent->DeleteItem(i);
            }
        }
    }

    void UpdateColumn(DataViewListManager::queueJobColumns column, std::shared_ptr<QueueItem> item, std::function<wxVariant()> callback) {
        if (item == nullptr) {
            return;
        }
        bool need_refresh = false;
        for (unsigned int i = 0; i < this->store->GetItemCount(); i++) {
            auto currentItem = store->GetItem(i);
            auto id          = store->GetItemData(currentItem);

            if (id != item->id) {
                continue;
            }
            std::shared_ptr<QueueItem> qitem = this->queueManager->GetItemPtr(id);
            if (qitem == nullptr) {
                continue;
            }

            for (const auto rnumber : DataViewListManager::RowNumber) {
                if (DataViewListManager::hasFlag(column, rnumber.first)) {
                    auto value = callback();
                    wxVariant oldValue;
                    this->store->GetValue(oldValue, currentItem, rnumber.second);
                    if (value != oldValue) {
                        this->store->SetValue(value, currentItem, rnumber.second);
                        this->store->RowValueChanged(i, rnumber.second);
                        need_refresh = true;
                    }
                }
            }
        }
        if (need_refresh) {
            this->parent->Refresh();
            this->parent->Update();
        }
    }

    void UpdateColumns(DataViewListManager::queueJobColumns columns, std::shared_ptr<QueueItem> item) {
        if (item == nullptr) {
            return;
        }
        if (store == nullptr) {
            return;
        }

        bool refresh = true;
        for (unsigned int i = 0; i < this->store->GetItemCount(); i++) {
            auto currentItem = store->GetItem(i);
            auto id          = store->GetItemData(currentItem);
            if (id != item->id) {
                continue;
            }
            std::shared_ptr<QueueItem> qitem = this->queueManager->GetItemPtr(id);
            if (qitem == nullptr) {
                continue;
            }

            if (DataViewListManager::hasFlag(columns, queueJobColumns::PROGRESS)) {
                const auto progress = item->GetActualProgress();
                wxVariant old_progress;
                store->GetValueByRow(old_progress, i, DataViewListManager::RowToInt(queueJobColumns::PROGRESS));
                refresh = false;
                if (old_progress.GetInteger() != progress) {
                    store->SetValueByRow(wxAny(progress), i, DataViewListManager::RowToInt(queueJobColumns::PROGRESS));
                    store->RowValueChanged(i, DataViewListManager::RowToInt(queueJobColumns::PROGRESS));
                    refresh = true;
                }
            }
            if (DataViewListManager::hasFlag(columns, queueJobColumns::SPEED)) {
                store->SetValueByRow(item->GetActualSpeed(), i, DataViewListManager::RowToInt(queueJobColumns::SPEED));
                store->RowValueChanged(i, DataViewListManager::RowToInt(queueJobColumns::SPEED));
            }
            if (DataViewListManager::hasFlag(columns, queueJobColumns::STATUS)) {
                store->SetValueByRow(wxGetTranslation(QueueStatus_GUI_str.at(item->status)), i, DataViewListManager::RowToInt(queueJobColumns::STATUS));
                store->RowValueChanged(i, DataViewListManager::RowToInt(queueJobColumns::STATUS));
            }
            if (DataViewListManager::hasFlag(columns, DataViewListManager::queueJobColumns::STATUS_MESSAGE)) {
                store->SetValueByRow(item->status_message, i, DataViewListManager::RowToInt(queueJobColumns::STATUS_MESSAGE));
                store->RowValueChanged(i, DataViewListManager::RowToInt(queueJobColumns::STATUS_MESSAGE));
            }
            if (refresh) {
                this->parent->Refresh();
                this->parent->Update();
            }
            break;
        }
    }

private:
    wxDataViewListCtrl* parent                     = nullptr;
    wxDataViewListStore* store                     = nullptr;
    std::shared_ptr<QM::QueueManager> queueManager = nullptr;
    sd_gui_utils::config* config                   = nullptr;
};

// Helper functions for bitwise operations
inline DataViewListManager::queueJobColumns operator|(DataViewListManager::queueJobColumns a, DataViewListManager::queueJobColumns b) {
    return static_cast<DataViewListManager::queueJobColumns>(
        static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline DataViewListManager::queueJobColumns operator&(DataViewListManager::queueJobColumns a, DataViewListManager::queueJobColumns b) {
    return static_cast<DataViewListManager::queueJobColumns>(
        static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline DataViewListManager::queueJobColumns operator~(DataViewListManager::queueJobColumns a) {
    return static_cast<DataViewListManager::queueJobColumns>(~static_cast<unsigned int>(a));
}

#endif  // __SDGUI_HELPERS_DATAVIEWLISTMANAGER__