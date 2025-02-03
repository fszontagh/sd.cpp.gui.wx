#include "DataViewListManager.hpp"
void DataViewListManager::DeleteItems(const wxDataViewItemArray& selected_items) {
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
void DataViewListManager::DeleteItems(std::vector<wxDataViewItem> items) {
    wxDataViewItemArray selected_items;
    for (const auto& item : items) {
        selected_items.Add(item);
    }
    this->DeleteItems(selected_items);
}
void DataViewListManager::DeleteItem(uint64_t job_id) {
    for (int i = this->parent->GetItemCount() - 1; i >= 0; i--) {
        auto item = this->parent->RowToItem(i);
        auto id   = this->parent->GetItemData(item);
        if (id == job_id) {
            this->parent->DeleteItem(i);
            this->queueManager->DeleteJob(job_id);
        }
    }
}
std::vector<uint64_t> DataViewListManager::GetSelectedItemsId(wxDataViewItemArray selected_items) {
    std::vector<uint64_t> list;
    for (const auto& item : selected_items) {
        auto job_id = store->GetItemData(item);
        list.emplace_back(job_id);
    }
    return list;
}
std::shared_ptr<QueueItem> DataViewListManager::GetQueueItem(const unsigned int row) {
    auto item   = store->GetItem(row);
    auto job_id = store->GetItemData(item);
    return this->queueManager->GetItemPtr(job_id);
}
std::shared_ptr<QueueItem> DataViewListManager::GetQueueItem(const wxDataViewItem& item) {
    auto job_id = store->GetItemData(item);
    return this->queueManager->GetItemPtr(job_id);
}
std::vector<std::shared_ptr<QueueItem>> DataViewListManager::GetQueueItems(const wxDataViewItemArray& selected_items) {
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
bool DataViewListManager::ItemExists(uint64_t item_id) {
    for (unsigned int i = 0; i < this->store->GetItemCount(); i++) {
        if (this->store->GetItemData(this->store->GetItem(i)) == item_id) {
            return true;
        }
    }
    return false;
}
void DataViewListManager::AddItem(std::shared_ptr<QueueItem> item) {
    wxVector<wxVariant> data;

    auto created_at     = sd_gui_utils::formatUnixTimestampToDate(item->created_at);
    wxString serverName = _("local");

    if (!item->server.empty()) {
        auto srv = this->config->GetTcpServer(item->server);
        if (srv) {
            serverName = srv->GetName();
        } else {
            serverName = _("unknown server");
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
    data.push_back(wxVariant(wxString(item->status_message.data(), item->status_message.size())));

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
void DataViewListManager::RemoveRemoteItems(const std::string& server_id) {
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
void DataViewListManager::UpdateColumn(DataViewListManager::queueJobColumns column, std::shared_ptr<QueueItem> item, std::function<wxVariant()> callback) {
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
void DataViewListManager::UpdateColumns(DataViewListManager::queueJobColumns columns, std::shared_ptr<QueueItem> item) {
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
wxDataViewItem DataViewListManager::JobToItem(std::shared_ptr<QueueItem> item) {
    if (item == nullptr) {
        return wxDataViewItem();
    }
    for (unsigned int i = 0; i < this->store->GetItemCount(); i++) {
        auto currentItem = store->GetItem(i);
        auto id          = store->GetItemData(currentItem);
        if (id == item->id) {
            return currentItem;
        }
    }
    return wxDataViewItem();
}
wxDataViewItem DataViewListManager::RowToDataViewItem(unsigned int row) {
    if (store == nullptr) {
        return wxDataViewItem();
    }
    return store->GetItem(row);
}
