#ifndef TREE_LIST_MANAGER_H
#define TREE_LIST_MANAGER_H

class ModelFileInfoData : public wxClientData {
public:
    explicit ModelFileInfoData(sd_gui_utils::ModelFileInfo* info)
        : fileInfo(info), server_id(info->server_id) {}
    explicit ModelFileInfoData(const std::string server_id)
        : server_id(server_id) {
    }
    sd_gui_utils::ModelFileInfo* GetFileInfo() const { return fileInfo; }
    const std::string GetServerId() {
        return this->server_id;
    }

private:
    sd_gui_utils::ModelFileInfo* fileInfo = nullptr;
    std::string server_id                 = "";
};

template <typename T>
class DataContainer : public wxClientData {
public:
    DataContainer(T* data)
        : data(data) {}
    T* GetData() { return data; }

private:
    T* data;
};

class wxTreeListModelItemComparator : public wxTreeListItemComparator {
public:
    int Compare(wxTreeListCtrl* treelist, unsigned column, wxTreeListItem first, wxTreeListItem second) override {
        wxString textA = treelist->GetItemText(first, column);
        wxString textB = treelist->GetItemText(second, column);
        return textA.CmpNoCase(textB);
    }
};

class ModelUiManager {
public:
    class ColumnInfo {
    public:
        const wxString& title;
        int width         = wxCOL_WIDTH_AUTOSIZE;
        wxAlignment align = wxALIGN_LEFT;
        int flags         = wxCOL_RESIZABLE;
        int id            = 0;
    };
    struct col {
        int pos;
        wxString title;
    };

    ModelUiManager(wxTreeListCtrl* treeList, sd_gui_utils::config* cfg)
        : treeListCtrl(treeList), config(cfg) {
        auto comparator = new wxTreeListModelItemComparator();
        treeListCtrl->SetItemComparator(comparator);
    }

    void AddDirTypeSelector(wxChoice* choice, sd_gui_utils::DirTypes type) {
        this->dirTypeSelectors[type] = choice;
    }

    void AppendColumn(const wxString& title, int width = wxCOL_WIDTH_AUTOSIZE, wxAlignment align = wxALIGN_LEFT, int flags = wxCOL_RESIZABLE) {
        int id = treeListCtrl->AppendColumn(title, width, align, flags);
        ColumnInfo info{title, width, align, flags, id};
        columns.push_back(info);
    }

    void AddItem(sd_gui_utils::ModelFileInfo* item, bool select = false, sd_gui_utils::sdServer* server = nullptr) {
        wxTreeListItem parentItem = GetOrCreateParent(item->folderGroupName, item->server_id);
        wxString name             = wxString::FromUTF8Unchecked(item->name);
        if (!item->folderGroupName.empty()) {
            name.Replace(item->folderGroupName, "");
            name.Replace(wxFileName::GetPathSeparator(), "");
        }
        if (sd_gui_utils::HasTag(item->tags, sd_gui_utils::ModelInfoTag::Favorite)) {
            // name.Prepend(wxUniChar(0x2B50));  // Unicode star: ⭐ --> this is not working on windows
            name = wxString::Format("%s %s", _("[F] "), name);
        }
        wxTreeListItem newItem = treeListCtrl->AppendItem(parentItem, name);
        treeListCtrl->SetItemText(newItem, 1, wxString(item->size_f));
        treeListCtrl->SetItemText(newItem, 2, wxString(ConvertTypeToString(item->model_type)));
        treeListCtrl->SetItemText(newItem, 3, wxString(item->sha256).substr(0, 10));
        treeListCtrl->SetItemData(newItem, new ModelFileInfoData(const_cast<sd_gui_utils::ModelFileInfo*>(item)));
        if (select && item->server_id.empty()) {
            treeListCtrl->UnselectAll();
            treeListCtrl->Select(newItem);
        }

        this->AddOrRemoveModelByDirType(item);
    }

    template <typename T>
    void AddItem(const wxString& title, const wxVector<ModelUiManager::col>& item, const wxString& groupName = wxEmptyString, T* data = nullptr) {
        wxTreeListItem parentItem = GetOrCreateParent(groupName);
        wxTreeListItem newItem    = treeListCtrl->AppendItem(parentItem, title);
        if (data) {
            treeListCtrl->SetItemData(newItem, data);
        }

        if (item.size() != this->columns.size()) {
            throw std::invalid_argument("item.size() != this->columns.size()");
        }
        for (size_t i = 0; i < item.size(); i++) {
            treeListCtrl->SetItemText(newItem, item[i].pos, item[i].title);
        }
    }

    void RemoveItem(const std::string& path) {
        wxTreeListItem root = treeListCtrl->GetRootItem();
        wxTreeListItem item = FindItemByPath(root, path);
        if (item.IsOk()) {
            treeListCtrl->DeleteItem(item);
        }
    }

    void SelectItemByModelPath(const std::string& path, bool unselectBefore = true) {
        if (path.empty()) {
            return;
        }

        wxTreeListItem root = treeListCtrl->GetRootItem();
        wxTreeListItem item = FindItemByPath(root, path);
        if (item.IsOk()) {
            if (unselectBefore) {
                treeListCtrl->UnselectAll();
            }
            treeListCtrl->Select(item);
        }
    }

    void UpdateItem(const sd_gui_utils::ModelFileInfo* updatedItem) {
        wxTreeListItem root = treeListCtrl->GetRootItem();
        wxTreeListItem item = FindItemByPath(root, updatedItem->path);
        if (item.IsOk()) {
            treeListCtrl->SetItemText(item, 0, wxString(updatedItem->name));
            treeListCtrl->SetItemText(item, 1, wxString(updatedItem->size_f));
            treeListCtrl->SetItemText(item, 2, wxString(ConvertTypeToString(updatedItem->model_type)));
            treeListCtrl->SetItemText(item, 3, wxString(updatedItem->sha256));
        }
    }
    void ChangeText(const std::string& path, const wxString& text, unsigned int col) {
        wxTreeListItem root = treeListCtrl->GetRootItem();
        wxTreeListItem item = FindItemByPath(root, path);
        if (item.IsOk()) {
            treeListCtrl->SetItemText(item, col, wxString::FromUTF8(text));
        }
    }

    void ChangeText(wxTreeListItem item, const wxString& text, unsigned int col) {
        if (item.IsOk()) {
            treeListCtrl->SetItemText(item, col, wxString::FromUTF8(text));
        }
    }

    sd_gui_utils::ModelFileInfo* FindItem(const std::string& path) {
        wxTreeListItem root = treeListCtrl->GetRootItem();
        wxTreeListItem item = FindItemByPath(root, path);
        if (item.IsOk()) {
            ModelFileInfoData* data = static_cast<ModelFileInfoData*>(treeListCtrl->GetItemData(item));
            return data ? data->GetFileInfo() : nullptr;
        }
        return nullptr;
    }

    sd_gui_utils::ModelFileInfo* FindItem(const wxTreeListItem& item) {
        ModelFileInfoData* data = static_cast<ModelFileInfoData*>(treeListCtrl->GetItemData(item));
        if (data == nullptr) {
            return nullptr;
        }
        return data ? data->GetFileInfo() : nullptr;
    }

    wxTreeListItem GetOrCreateParent(const wxString& folderGroupName) {
        return GetOrCreateParent(folderGroupName.utf8_string());
    }

    const wxString findParentPath(const wxTreeListItem& item, sd_gui_utils::config* config) {
        auto selected     = item;
        auto selectedName = this->treeListCtrl->GetItemText(item);
        wxString basePath = selectedName, groupFolderName = selectedName;

        while (selected.IsOk()) {
            auto parent         = this->treeListCtrl->GetItemParent(selected);
            wxString parentName = this->treeListCtrl->GetItemText(parent);
            if (parent.IsOk() == false || parentName.empty()) {
                break;
            }
            auto parentPath = config->getPathByDirType(parentName);
            if (parentPath.empty() == false) {
                basePath.Prepend(parentPath + wxFileName::GetPathSeparators());
            } else {
                parentName.Append(wxFileName::GetPathSeparators());
                basePath.Prepend(parentName);
            }
            groupFolderName.Prepend(parentName + wxFileName::GetPathSeparators());
            selected = parent;
        }
        if (basePath == selectedName) {
            basePath = config->getPathByDirType(selectedName);
            if (basePath.empty() == true) {
                return wxEmptyString;
            }
        }
        return basePath;
    }

    void DeleteByServerId(const std::string& serverId) {
        if (serverId.empty()) {
            return;
        }

        auto parents = this->GetParentsByServerId(serverId);
        if (parents.size() > 0) {
            this->treeListCtrl->DeleteItem(parents.front());
            for (const auto& item : parents) {
                if (item.IsOk() == false) {
                    continue;
                }
                if (this->parentMap.empty()) {
                    continue;
                }
                for (auto it = this->parentMap.begin(); it != this->parentMap.end();) {
                    if ((*it).second == item) {
                        it = this->parentMap.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        for (auto& selectors : this->dirTypeSelectors) {
            auto choice = selectors.second;
            for (int i = choice->GetCount() - 1; i >= 0; i--) {
                const auto modelinfo = static_cast<sd_gui_utils::ModelFileInfo*>(choice->GetClientData(i));
                if (modelinfo && modelinfo->server_id.empty() == false && modelinfo->server_id == serverId) {
                    choice->Delete(i);
                }
            }
        }
    }

    sd_gui_utils::ModelFileInfo* GetModelByItem(const wxTreeListItem item) {
        auto data = static_cast<ModelFileInfoData*>(treeListCtrl->GetItemData(item));
        if (data) {
            return data->GetFileInfo();
        }
        return nullptr;
    }

    sd_gui_utils::ModelFileInfo* SetSelectedAsFavorite(bool state, sd_gui_utils::sdServer* server = nullptr) {
        wxTreeListItems selections;
        if (this->treeListCtrl->GetSelections(selections) == 0) {
            return nullptr;
        }
        auto item  = selections.front();
        auto model = this->GetModelByItem(item);
        if (model == nullptr) {
            return nullptr;
        }

        wxString name = wxString::FromUTF8Unchecked(model->name);
        if (state) {
            model->tags = model->tags | sd_gui_utils::ModelInfoTag::Favorite;
            name        = wxString::Format("%s %s", _("[F] "), name);
            this->ChangeText(item, name, 0);
        } else {
            this->ChangeText(item, name, 0);
            model->tags = model->tags & ~sd_gui_utils::ModelInfoTag::Favorite;
        }
        this->SelectItemByModelPath(model->path);

        return model;
    }

    void ChangeModelByInfo(sd_gui_utils::ModelFileInfo* info, sd_gui_utils::sdServer* server = nullptr) {
        if (info == nullptr) {
            return;
        }
        if (this->dirTypeSelectors.contains(info->model_type) == true) {
            auto selector = this->dirTypeSelectors.at(info->model_type);
            for (auto i = 0; i < selector->GetCount(); i++) {
                auto clientData = static_cast<sd_gui_utils::ModelFileInfo*>(selector->GetClientData(i));
                if (clientData && clientData == info) {
                    selector->SetSelection(i);
                    return;
                }
            }
        }

        // add the model if not found
        this->AddOrRemoveModelByDirType(info);
    }

    std::vector<wxTreeListItem> GetParentsByServerId(const std::string& server_id) {
        std::vector<wxTreeListItem> list = {};
        if (server_id.empty()) {
            return list;
        }
        wxTreeListItem currentItem = treeListCtrl->GetRootItem();
        if (currentItem.IsOk() == false) {
            return list;
        }

        while (currentItem.IsOk()) {
            auto data = static_cast<ModelFileInfoData*>(treeListCtrl->GetItemData(currentItem));
            if (data != NULL) {
                if (data->GetServerId().empty() == false && data->GetServerId() == server_id) {
                    if (currentItem != this->treeListCtrl->GetRootItem()) {
                        list.push_back(currentItem);
                    }
                }
            }
            currentItem = this->treeListCtrl->GetNextItem(currentItem);
        }

        return list;
    }

    std::vector<wxTreeListItem> GetItemsByServerId(const std::string server_id) {
        std::vector<wxTreeListItem> list = {};
        if (server_id.empty()) {
            return list;
        }
        wxTreeListItem currentItem = treeListCtrl->GetRootItem();
        if (currentItem.IsOk() == false) {
            return list;
        }

        while (currentItem.IsOk()) {
            auto data = static_cast<ModelFileInfoData*>(treeListCtrl->GetItemData(currentItem));
            if (data != NULL) {
                auto info = data->GetFileInfo();
                if (info && info->server_id == server_id) {
                    list.emplace_back(currentItem);
                }
            }
            currentItem = this->treeListCtrl->GetNextItem(currentItem);
        }

        return list;
    }

    void CleanAll() {
        treeListCtrl->DeleteAllItems();
        parentMap.clear();
    }
    void AddOrRemoveModelByDirType(sd_gui_utils::ModelFileInfo* model) {
        if (!this->dirTypeSelectors.contains(model->model_type)) {
            return;
        }

        auto selector = this->dirTypeSelectors.at(model->model_type);

        wxString name = wxString::FromUTF8(model->name);

        if (model->server_id.empty() == false) {
            auto srv = this->config->GetTcpServer(model->server_id);
            if (srv) {
                name = wxString::Format("%s %s", srv->GetName(), name);
            }
        }

        // remove all items if not favorites if only favorites required
        if (this->config->favorite_models_only && model->model_type == sd_gui_utils::DirTypes::CHECKPOINT) {
            for (int i = 1; i < selector->GetCount(); i++) {
                auto clientData = static_cast<sd_gui_utils::ModelFileInfo*>(selector->GetClientData(i));
                if (clientData && sd_gui_utils::HasTag(clientData->tags, sd_gui_utils::ModelInfoTag::Favorite) == false) {
                    selector->Delete(i);
                    if (clientData->path == model->path) {  // we just deleted the model, no more job
                        return;
                    }
                }
            }
            if (sd_gui_utils::HasTag(model->tags, sd_gui_utils::ModelInfoTag::Favorite)) {
                selector->Append(name, model);
                return;
            }
            return;
        }

        // check if we already have an inserted. If yes, then just update
        for (int i = 1; i < selector->GetCount(); i++) {
            auto clientData = static_cast<sd_gui_utils::ModelFileInfo*>(selector->GetClientData(i));
            if (clientData && clientData->path == model->path) {
                selector->SetString(i, name);
                selector->SetClientData(i, model);
                return;
            }
        }

        selector->Append(name, model);
    }

private:
    wxTreeListCtrl* treeListCtrl = nullptr;
    std::unordered_map<sd_gui_utils::DirTypes, wxChoice*> dirTypeSelectors;
    sd_gui_utils::config* config = nullptr;
    wxVector<ModelUiManager::ColumnInfo> columns;
    std::map<wxString, wxTreeListItem> parentMap;

    wxTreeListItem GetOrCreateParent(const std::string& folderGroupName, std::string server_id = "") {
        if (folderGroupName.empty()) {
            return treeListCtrl->GetRootItem();
        }
        std::vector<std::string> groups;
        SplitFolderGroupName(wxString::FromUTF8Unchecked(folderGroupName), groups);

        wxTreeListItem parent = treeListCtrl->GetRootItem();
        std::string fullPath;
        for (const auto& group : groups) {
            if (!fullPath.empty()) {
                fullPath += wxFileName::GetPathSeparator();
            }
            fullPath += group;

            if (parentMap.find(fullPath) == parentMap.end()) {
                wxTreeListItem newParent = treeListCtrl->AppendItem(parent, wxString(group));
                treeListCtrl->SetItemData(newParent, new ModelFileInfoData(server_id));
                parentMap[fullPath] = newParent;
                parent              = newParent;
            } else {
                parent = parentMap[fullPath];
            }
        }
        return parent;
    }

    wxTreeListItem FindItemByPath(const wxTreeListItem& parent, const std::string& path) {
        wxTreeListItem item = treeListCtrl->GetFirstChild(parent);
        while (item.IsOk()) {
            ModelFileInfoData* data = static_cast<ModelFileInfoData*>(treeListCtrl->GetItemData(item));
            if (data) {
                sd_gui_utils::ModelFileInfo* fileInfo = data->GetFileInfo();
                if (fileInfo && fileInfo->path == path) {
                    return item;
                }
            }
            wxTreeListItem childResult = FindItemByPath(item, path);
            if (childResult.IsOk()) {
                return childResult;
            }
            item = treeListCtrl->GetNextSibling(item);
        }
        return wxTreeListItem();
    }

    void SplitFolderGroupName(const wxString& folderGroupName, std::vector<std::string>& groups) {
        wxStringTokenizer tokenizer(folderGroupName, wxFileName::GetPathSeparator());
        while (tokenizer.HasMoreTokens()) {
            wxString token = tokenizer.GetNextToken();
            groups.push_back(std::string(token.mb_str()));
        }
    }

    std::string ConvertTypeToString(sd_gui_utils::DirTypes type) {
        return sd_gui_utils::dirtypes_str.contains(type) ? sd_gui_utils::dirtypes_str.at(type) : "Unknown";
    }
};

#endif  // TREE_LIST_MANAGER_H