#ifndef LLAMAGUIHELPER_HPP
#define LLAMAGUIHELPER_HPP

class MainWindowUI;
namespace sd_gui_utils {

    class LlamaGuiHelper {
    private:
        MainWindowUI* parent = nullptr;
        struct PanelData {
            wxScrolledWindow* scrolledWindow;
            wxBoxSizer* chatSizer;
            std::shared_ptr<sd_gui_utils::llvmMessage> chatData = nullptr;
            std::unordered_map<uint64_t, wxWebView*> messageWebViews;
            std::unordered_map<uint64_t, uint64_t> messageLastUpdates;
        };

        std::vector<wxPanel*> chatPanels = {};
        wxPanel* currentChatPanel        = nullptr;

        void UpdateChatPanel(wxPanel* chatPanel);
        void UpdateModelMetaInfo();
        void ClearModelMetaInfo();
        void InsertTreeItem(wxTreeListCtrl* treeView, wxTreeListItem parent, const wxString& ItemKey, const wxString& ItemValue);

    public:
        LlamaGuiHelper(MainWindowUI* parent);
        ~LlamaGuiHelper();
        void ChangeChatPanel(wxPanel* chatPanel);
        void ProcessEvents();
        void addChatPanel(const wxString& title = _("New Chat"), bool select = true);
    };
};

#endif