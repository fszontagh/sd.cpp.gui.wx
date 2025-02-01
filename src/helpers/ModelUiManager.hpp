#ifndef MODEL_UI_MANAGER_H
#define MODEL_UI_MANAGER_H

// --- Helper Classes ---
class ModelFileInfoData : public wxClientData {
public:
    explicit ModelFileInfoData(sd_gui_utils::ModelFileInfo* info)
        : fileInfo(info), server_id(info->server_id) {}

    explicit ModelFileInfoData(const std::string& server_id)
        : server_id(server_id) {}

    sd_gui_utils::ModelFileInfo* GetFileInfo() const { return fileInfo; }
    const std::string& GetServerId() const { return server_id; }

private:
    sd_gui_utils::ModelFileInfo* fileInfo = nullptr;
    std::string server_id;
};

template <typename T>
class DataContainer : public wxClientData {
public:
    explicit DataContainer(T* data)
        : data(data) {}
    T* GetData() const { return data; }

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

// --- Main Class ---
class ModelUiManager {
public:
    struct ColumnInfo {
        const wxString& title;
        int width         = wxCOL_WIDTH_AUTOSIZE;
        wxAlignment align = wxALIGN_LEFT;
        int flags         = wxCOL_RESIZABLE;
        int id            = 0;
    };

    struct Col {
        int pos;
        wxString title;
    };

    ModelUiManager(wxTreeListCtrl* treeList, sd_gui_utils::config* cfg);

    // --- UI Management ---
    void AddDirTypeSelector(wxChoice* choice, sd_gui_utils::DirTypes type);
    void AppendColumn(const wxString& title, int width = wxCOL_WIDTH_AUTOSIZE, wxAlignment align = wxALIGN_LEFT, int flags = wxCOL_RESIZABLE);

    // --- Item Management ---
    void AddItem(sd_gui_utils::ModelFileInfo* item, bool select = false, sd_gui_utils::sdServer* server = nullptr);
    void RemoveItem(const std::string& path);
    void SelectItemByModelPath(const std::string& path, bool unselectBefore = true);
    void UpdateItem(const sd_gui_utils::ModelFileInfo* updatedItem);
    void AddOrRemoveModelByDirType(sd_gui_utils::ModelFileInfo* model);
    void CleanAll();

    // --- Item Modification ---
    void ChangeText(const std::string& path, const wxString& text, unsigned int col);
    void ChangeText(wxTreeListItem item, const wxString& text, unsigned int col);

    // --- Item Search ---
    sd_gui_utils::ModelFileInfo* FindItem(const std::string& path);
    sd_gui_utils::ModelFileInfo* FindItem(const wxTreeListItem& item);
    std::vector<wxTreeListItem> GetParentsByServerId(const std::string& server_id);
    std::vector<wxTreeListItem> GetItemsByServerId(const std::string& server_id);
    sd_gui_utils::ModelFileInfo* GetModelByItem(wxTreeListItem item);

    // --- Server & Favorite Management ---
    void DeleteByServerId(const std::string& serverId);
    sd_gui_utils::ModelFileInfo* SetSelectedAsFavorite(bool state, sd_gui_utils::sdServer* server = nullptr);
    void ChangeModelByInfo(sd_gui_utils::ModelFileInfo* info, sd_gui_utils::sdServer* server = nullptr);

    // --- Utility Functions ---
    wxTreeListItem GetOrCreateParent(const wxString& folderGroupName);
    const wxString findParentPath(const wxTreeListItem& item, sd_gui_utils::config* config);

    // --- Template Function for Generic Item Addition ---
    template <typename T>
    inline void AddItem(const wxString& title, const wxVector<ModelUiManager::Col>& item, const wxString& groupName = wxEmptyString, T* data = nullptr) {
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

private:
    // --- Internal Methods ---
    wxTreeListItem GetOrCreateParent(const std::string& folderGroupName, std::string server_id = "");
    wxTreeListItem FindItemByPath(const wxTreeListItem& parent, const std::string& path);
    void SplitFolderGroupName(const wxString& folderGroupName, std::vector<std::string>& groups);
    std::string ConvertTypeToString(sd_gui_utils::DirTypes type);

    // --- Internal Data Members ---
    wxTreeListCtrl* treeListCtrl = nullptr;
    sd_gui_utils::config* config = nullptr;
    wxVector<ModelUiManager::ColumnInfo> columns;
    std::unordered_map<sd_gui_utils::DirTypes, wxChoice*> dirTypeSelectors;
    std::map<wxString, wxTreeListItem> parentMap;
};

#endif  // MODEL_UI_MANAGER_H
