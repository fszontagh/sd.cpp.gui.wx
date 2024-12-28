#ifndef TREE_LIST_MANAGER_H
#define TREE_LIST_MANAGER_H

class ModelFileInfoData : public wxClientData {
public:
    explicit ModelFileInfoData(sd_gui_utils::ModelFileInfo* info)
        : fileInfo(info) {}
    sd_gui_utils::ModelFileInfo* GetFileInfo() const { return fileInfo; }

private:
    sd_gui_utils::ModelFileInfo* fileInfo;
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

class TreeListManager {
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

    TreeListManager(wxTreeListCtrl* treeList) {
        if (!treeList) {
            throw std::invalid_argument("treeList cannot be null");
        }
        auto comparator = new wxTreeListModelItemComparator();
        treeListCtrl    = treeList;
        treeListCtrl->SetItemComparator(comparator);
    }

    inline void AppendColumn(const wxString& title, int width = wxCOL_WIDTH_AUTOSIZE, wxAlignment align = wxALIGN_LEFT, int flags = wxCOL_RESIZABLE) {
        int id = treeListCtrl->AppendColumn(title, width, align, flags);
        ColumnInfo info{title, width, align, flags, id};
        columns.push_back(info);
    }

    inline void AddItem(sd_gui_utils::ModelFileInfo* item, sd_gui_utils::sdServer* server = nullptr) {
        wxTreeListItem parentItem = GetOrCreateParent(item->folderGroupName);
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
        this->dataMap[newItem] = new ModelFileInfoData(item);
        treeListCtrl->SetItemText(newItem, 1, wxString(item->size_f));
        treeListCtrl->SetItemText(newItem, 2, wxString(ConvertTypeToString(item->model_type)));
        treeListCtrl->SetItemText(newItem, 3, wxString(item->sha256).substr(0, 10));
        treeListCtrl->SetItemData(newItem, this->dataMap[newItem]);
        this->dataMap[newItem] = new ModelFileInfoData(item);
    }

    void DeleteByServerId(const sd_gui_utils::sdServer* server) {
        for (auto it = dataMap.begin(); it != dataMap.end();) {
            if (it->second != nullptr && it->second->GetFileInfo() != nullptr && it->second->GetFileInfo()->server_id == server->server_id) {
                this->treeListCtrl->DeleteItem(it->first);
                it = dataMap.erase(it);
            } else {
                it++;
            }
        }
    }

    void TraverseAndDelete(const wxTreeListItem& parentItem, std::function<bool(void*)> condition) {
        wxTreeListItem child = treeListCtrl->GetFirstChild(parentItem);
        while (child.IsOk()) {
            wxTreeListItem nextSibling = treeListCtrl->GetNextSibling(child);

            // Bejárás: gyerekek rekurzív ellenőrzése
            TraverseAndDelete(child, condition);

            // Feltétel ellenőrzése és törlés
            void* data = treeListCtrl->GetItemData(child);
            if (data && condition(data)) {
                auto it = std::find_if(parentMap.begin(), parentMap.end(),
                                       [&](const auto& pair) { return pair.second == child; });
                if (it != parentMap.end()) {
                    parentMap.erase(it);
                }
                treeListCtrl->DeleteItem(child);
            }

            child = nextSibling;
        }

        // Szülő törlése, ha nincs gyerek
        if (treeListCtrl->GetFirstChild(parentItem).IsOk() == false && parentItem != treeListCtrl->GetRootItem()) {
            auto it = std::find_if(parentMap.begin(), parentMap.end(),
                                   [&](const auto& pair) { return pair.second == parentItem; });
            if (it != parentMap.end()) {
                parentMap.erase(it);
            }
            treeListCtrl->DeleteItem(parentItem);
        }
    }

    template <typename T>
    void AddItem(const wxString& title, const wxVector<TreeListManager::col>& item, const wxString& groupName = wxEmptyString, T* data = nullptr) {
        wxTreeListItem parentItem = GetOrCreateParent(groupName);
        wxTreeListItem newItem    = treeListCtrl->AppendItem(parentItem, title);
        if (data) {
            treeListCtrl->SetItemData(newItem, new DataContainer(data));
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
    void ChangeText(wxTreeListItem item, const wxString& text, unsigned int col = 0) {
        if (item.IsOk()) {
            treeListCtrl->SetItemText(item, col, text);
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
        if (data == NULL) {
            return nullptr;
        }
        auto mdl = data->GetFileInfo();
        return mdl;
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

    void CleanAll() {
        treeListCtrl->DeleteAllItems();
        parentMap.clear();
    }

private:
    wxTreeListCtrl* treeListCtrl;
    wxVector<TreeListManager::ColumnInfo> columns;
    std::map<wxString, wxTreeListItem> parentMap;
    std::map<wxTreeListItem, ModelFileInfoData*> dataMap;

    /**
     * Retrieves or creates the parent wxTreeListItem for a given folder group name.
     *
     * This function takes a folder group name, which may consist of multiple segments
     * separated by separator character, and ensures that each segment has a corresponding wxTreeListItem
     * in the tree. If the specified folder group name is empty, it returns the root item.
     * For each segment, it checks if a corresponding item already exists; if not, it creates
     * a new item and inserts it into the tree. The function returns the wxTreeListItem of
     * the last segment in the folder group name.
     *
     * @param folderGroupName A string representing the hierarchical folder group name,
     *                        where segments are separated by separator character.
     * @return The wxTreeListItem corresponding to the last segment of the folder group name.
     */
    wxTreeListItem GetOrCreateParent(const std::string& folderGroupName) {
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
                parentMap[fullPath]      = newParent;
                parent                   = newParent;
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
