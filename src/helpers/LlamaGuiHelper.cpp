#include "LlamaGuiHelper.hpp"
#include "../ui/MainWindow.h"

sd_gui_utils::LlamaGuiHelper::LlamaGuiHelper(MainWindowUI* parent) {
    this->parent = parent;
}
sd_gui_utils::LlamaGuiHelper::~LlamaGuiHelper() {
    for (auto& p : this->chatPanels) {
        delete p;
    }
    this->chatPanels.clear();
}
void sd_gui_utils::LlamaGuiHelper::ChangeChatPanel(wxPanel* chatPanel) {
    if (!chatPanel) {
        return;
    }
    this->currentChatPanel = chatPanel;
}
void sd_gui_utils::LlamaGuiHelper::ProcessEvents() {
    if (!parent->chat_currentMessage || !this->currentChatPanel) {
        return;
    }
    const auto status_msg = this->parent->chat_currentMessage->GetStatusMessage();

    const auto status = this->parent->chat_currentMessage->GetStatus();

    const auto modelStatus   = this->parent->chat_currentMessage->GetModelStatus();
    const auto contextStatus = this->parent->chat_currentMessage->GetContextStatus();

    const auto command = this->parent->chat_currentMessage->GetCommandType();

    if (status == sd_gui_utils::llvmstatus::LLVM_STATUS_ERROR) {
        this->parent->writeLog(status_msg, true);
    }
    if (this->parent->m_chatStatus->GetLabel() != status_msg) {
        this->parent->m_chatStatus->SetLabel(status_msg);
    }

    if (this->parent->chat_currentMessage->Status(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_LOADED)) {
        this->parent->m_chat_n_ctx->Enable(false);
        this->parent->m_chat_n_batch->Enable(false);
    }
    if (this->parent->chat_currentMessage->Status(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_UNLOADED)) {
        this->parent->m_chat_n_ctx->Enable();
        this->parent->m_chat_n_batch->Enable();
    }

    // check the status
    if (modelStatus == sd_gui_utils::llvmModelStatus::LLVM_MODEL_STATUS_LOADED && command == sd_gui_utils::llvmCommand::LLVM_COMMAND_MODEL_LOAD) {
        wxString modelName = wxEmptyString;
        auto model         = this->parent->ModelManager->getInfo(this->parent->chat_currentMessage->GetModelPath());
        if (model != nullptr) {
            modelName = model.name;
            this->parent->writeLog(wxString::Format(_("Llama model loaded: %s"), model.name), true);
        }
        if (status_msg.empty()) {
            this->parent->m_chatStatus->SetLabel(wxString::Format(_("Model loaded %s"), modelName));
        }
        this->parent->m_languageModel->Enable();
        this->parent->m_chatInput->Enable();
        this->parent->m_sendChat->Enable();

        // set values from the model
        this->parent->m_chat_n_batch->SetValue(this->parent->chat_currentMessage->GetNBatch());
        int ctx = this->parent->chat_currentMessage->CalculateNCtx();
        if (this->parent->chat_currentMessage->GetNctx() != ctx) {
            this->parent->chat_currentMessage->SetNCtx(ctx);
        }
        this->parent->m_chat_n_ctx->SetValue(this->parent->chat_currentMessage->GetNctx());

        this->UpdateModelMetaInfo();

        return;
    }
    if (modelStatus == sd_gui_utils::llvmModelStatus::LLVM_MODEL_STATUS_UNLOADED && command == sd_gui_utils::llvmCommand::LLVM_COMMAND_MODEL_UNLOAD) {
        if (status_msg.empty()) {
            this->parent->m_chatStatus->SetLabel(_("Model unloaded"));
            auto model = this->parent->ModelManager->getInfo(this->parent->chat_currentMessage->GetModelPath());
            if (model != nullptr) {
                this->parent->writeLog(wxString::Format(_("Llama model unloaded: %s"), model.name), true);
            }
            this->parent->m_languageModel->SetSelection(0);  // ensure the first is selected
            this->parent->m_languageModel->Enable();
            this->parent->m_chatInput->Disable();
            this->parent->m_sendChat->Disable();
            this->parent->m_chat_prompt_template->Enable();
            this->parent->m_chat_n_ctx->Enable();
            this->parent->m_chat_n_batch->Enable();
        }
        return;
    }

    this->UpdateChatPanel(this->currentChatPanel);
};
void sd_gui_utils::LlamaGuiHelper::UpdateChatPanel(wxPanel* chatPanel) {
    auto* panelData = static_cast<PanelData*>(chatPanel->GetClientData());
    if (!panelData || !panelData->chatData)
        return;

    if (panelData->chatData->CheckUpdatedAt(this->parent->chat_currentMessage->GetUpdatedAt())) {
        return;
    }

    for (const auto& [id, msg] : this->parent->chat_currentMessage->GetMessages()) {
        wxString htmlMessage = wxString::Format("<p>%s <b>%s</b> %s</p>",
                                                sd_gui_utils::formatTimestamp(msg.updated_at),
                                                sd_gui_utils::llvmTextSenderMap.at(msg.sender),
                                                msg.text);

        wxString html = sd_gui_utils::ChatMessageTemplate(htmlMessage);

        auto it = panelData->messageWebViews.find(id);
        if (it != panelData->messageWebViews.end()) {
            if (panelData->messageLastUpdates.contains(id)) {
                if (panelData->messageLastUpdates[id] == msg.updated_at) {
                    continue;
                }
            }
            std::cout << "Update: " << id << std::endl;
            it->second->SetPage(html, wxT("/"));
            panelData->messageLastUpdates[id] = msg.updated_at;
            panelData->scrolledWindow->Scroll(0, panelData->scrolledWindow->GetVirtualSize().GetHeight());
        } else {
            wxWebView* newWebView = wxWebView::New(panelData->scrolledWindow, wxID_ANY);
            newWebView->SetMinSize(wxSize(300, 100));
            newWebView->Hide();
            newWebView->SetPage(html, wxT("/"));
            panelData->chatSizer->Add(newWebView, 0, wxEXPAND, 0);
            panelData->messageWebViews[id]    = newWebView;
            panelData->messageLastUpdates[id] = msg.updated_at;
            std::cout << "Created: " << id << std::endl;
            wxString jsGetHeight = "document.body.clientHeight.toString();";

            newWebView->Bind(wxEVT_WEBVIEW_LOADED, [newWebView, jsGetHeight](wxWebViewEvent&) {
                wxString result;
                if (newWebView->RunScript(jsGetHeight, &result)) {
                    long newHeight;
                    if (result.ToLong(&newHeight) && newHeight > 0) {
                        wxSize currentSize = newWebView->GetSize();
                        newWebView->SetMinSize(wxSize(currentSize.GetWidth(), newHeight));
                        newWebView->SetSize(wxSize(currentSize.GetWidth(), newHeight));
                        if (!newWebView->IsShown()) {
                            newWebView->Show();
                        }
                        newWebView->GetParent()->Layout();
                    }
                }
            });
        }
    }

    panelData->chatSizer->Layout();
    panelData->scrolledWindow->FitInside();
};
void sd_gui_utils::LlamaGuiHelper::addChatPanel(const wxString& title, bool select) {
    auto panel        = new wxPanel(this->parent->m_chatListBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    auto scrolledWindow = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
    scrolledWindow->SetScrollRate(5, 5);

    wxBoxSizer* chatSizer = new wxBoxSizer(wxVERTICAL);
    scrolledWindow->SetSizer(chatSizer);
    scrolledWindow->Layout();

    sizer->Add(scrolledWindow, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(sizer);
    panel->Layout();

    this->parent->m_chatListBook->AddPage(panel, title, select);
    this->currentChatPanel = panel;
    this->chatPanels.push_back(panel);

    auto* panelData = new PanelData{scrolledWindow, chatSizer, std::make_shared<sd_gui_utils::llvmMessage>()};
    panel->SetClientData(panelData);
};
void sd_gui_utils::LlamaGuiHelper::ClearModelMetaInfo() {
    auto treeView = this->parent->m_chat_model_meta;
    treeView->DeleteAllItems();
};
void sd_gui_utils::LlamaGuiHelper::UpdateModelMetaInfo() {
    this->ClearModelMetaInfo();

    wxTreeListCtrl* treeView = this->parent->m_chat_model_meta;

    wxTreeListItem root = treeView->GetRootItem();
    if (!root.IsOk()) {
        root = treeView->AppendItem(treeView->GetRootItem(), "Model metadata");
    }

    for (const auto& [key, val] : this->parent->chat_currentMessage->GetModelMetaList()) {
        this->InsertTreeItem(treeView, root, key, val);
    }

    treeView->Refresh();
    treeView->Update();
}

void sd_gui_utils::LlamaGuiHelper::InsertTreeItem(wxTreeListCtrl* treeView, wxTreeListItem parent, const wxString& ItemKey, const wxString& ItemValue) {
    wxArrayString parts    = wxStringTokenize(ItemKey, ".", wxTOKEN_STRTOK);
    wxTreeListItem current = parent;

    for (size_t i = 0; i < parts.size(); ++i) {
        bool found = false;
        wxTreeListItem child;

        // Gyerek elemek keresése
        for (wxTreeListItem item = treeView->GetFirstChild(current); item.IsOk(); item = treeView->GetNextSibling(item)) {
            if (treeView->GetItemText(item, 0) == parts[i]) {
                current = item;
                found   = true;
                break;
            }
        }

        // Ha nem létezik, létrehozzuk
        if (!found) {
            current = treeView->AppendItem(current, parts[i]);
            if (i == parts.size() - 1) {
                treeView->SetItemText(current, 1, ItemValue);
            }
        }
    }
}
