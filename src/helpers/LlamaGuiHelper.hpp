#ifndef LLAMAGUIHELPER_HPP
#define LLAMAGUIHELPER_HPP

class MainWindowUI;
namespace sd_gui_utils {

    class LlamaGuiHelper {
    private:
        MainWindowUI* parent = nullptr;
        struct PanelData {
            wxWebView* webView;
            std::shared_ptr<sd_gui_utils::llvmMessage> chatData = nullptr;
            std::unordered_map<uint64_t, uint64_t> messageLastUpdates;
        };

        std::vector<wxPanel*> chatPanels = {};
        wxPanel* currentChatPanel        = nullptr;
        void UpdateModelMetaInfo();
        void ClearModelMetaInfo();
        void InsertTreeItem(wxTreeListCtrl* treeView, wxTreeListItem parent, const wxString& ItemKey, const wxString& ItemValue);
        void UpdateChat();

    public:
        LlamaGuiHelper(MainWindowUI* parent);
        ~LlamaGuiHelper();
        void ChangeChatPanel(wxPanel* chatPanel);
        void ProcessEvents();
        void addChatPanel(const wxString& title = _("New Chat"), bool select = true);
        inline const sd_gui_utils::LlamaGuiHelper::PanelData GetCurrentPanelData() {
            if (!this->currentChatPanel) {
                return {};
            }
            return *static_cast<sd_gui_utils::LlamaGuiHelper::PanelData*>(this->currentChatPanel->GetClientData());
        };
        inline void PrintCurrentwebView() {
            if (!this->currentChatPanel) {
                return;
            }
            auto panelData = static_cast<sd_gui_utils::LlamaGuiHelper::PanelData*>(this->currentChatPanel->GetClientData());
            panelData->webView->Print();
        }
        inline const wxString GetCurrentWebviewSource() {
            if (!this->currentChatPanel) {
                return wxEmptyString;
            }
            auto panelData = static_cast<sd_gui_utils::LlamaGuiHelper::PanelData*>(this->currentChatPanel->GetClientData());
            // panelData->webView->DeleteSelection();
            // panelData->webView->SelectAll();
            // const auto selection = panelData->webView->GetSelectedSource();
            // panelData->webView->DeleteSelection();
            // return panelData->webView->GetPageSource();
            wxString output;
            panelData->webView->RunScript(wxString("document.documentElement.outerHTML"), &output);
            return output;
        }
    };
};

#endif