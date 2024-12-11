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

    void AppendColumn(const wxString& title, int width = wxCOL_WIDTH_AUTOSIZE, wxAlignment align = wxALIGN_LEFT, int flags = wxCOL_RESIZABLE) {
        int id = treeListCtrl->AppendColumn(title, width, align, flags);
        ColumnInfo info{title, width, align, flags, id};
        columns.push_back(info);
    }

    void AddItem(const sd_gui_utils::ModelFileInfo* item) {
        wxTreeListItem parentItem = GetOrCreateParent(item->folderGroupName);
        wxString name             = wxString::FromUTF8Unchecked(item->name);
        if (!item->folderGroupName.empty()) {
            name.Replace(item->folderGroupName, "");
            name.Replace(wxFileName::GetPathSeparator(), "");
        }

        wxTreeListItem newItem = treeListCtrl->AppendItem(parentItem, name);
        treeListCtrl->SetItemText(newItem, 1, wxString(item->size_f));
        treeListCtrl->SetItemText(newItem, 2, wxString(ConvertTypeToString(item->model_type)));
        treeListCtrl->SetItemText(newItem, 3, wxString(item->sha256));
        treeListCtrl->SetItemData(newItem, new ModelFileInfoData(const_cast<sd_gui_utils::ModelFileInfo*>(item)));
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
        return data ? data->GetFileInfo() : nullptr;
    }

private:
    wxTreeListCtrl* treeListCtrl;
    wxVector<TreeListManager::ColumnInfo> columns;

    std::map<wxString, wxTreeListItem> parentMap;
    wxTreeListItem GetOrCreateParent(const wxString& folderGroupName) {
        return GetOrCreateParent(folderGroupName.utf8_string());
    }
    wxTreeListItem GetOrCreateParent(const std::string& folderGroupName) {
        if (folderGroupName.empty()) {
            return treeListCtrl->GetRootItem();
        }

        std::vector<std::string> groups;
        SplitFolderGroupName(folderGroupName, groups);

        wxTreeListItem parent = treeListCtrl->GetRootItem();
        std::string fullPath;
        for (const auto& group : groups) {
            if (!fullPath.empty()) {
                fullPath += "/";
            }
            fullPath += group;

            if (parentMap.find(fullPath) == parentMap.end()) {
                parentMap[fullPath] = treeListCtrl->AppendItem(parent, wxString(group));
            }
            parent = parentMap[fullPath];
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

    void SplitFolderGroupName(const std::string& folderGroupName, std::vector<std::string>& groups) {
        std::stringstream ss(folderGroupName);
        std::string segment;
        while (std::getline(ss, segment, '/')) {
            groups.push_back(segment);
        }
    }

    std::string ConvertTypeToString(sd_gui_utils::DirTypes type) {
        return sd_gui_utils::dirtypes_str.contains(type) ? sd_gui_utils::dirtypes_str.at(type) : "Unknown";
    }
};

#endif  // TREE_LIST_MANAGER_H
