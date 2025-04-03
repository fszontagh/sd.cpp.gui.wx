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
    this->UpdateChat();
};

void sd_gui_utils::LlamaGuiHelper::addChatPanel(const wxString& title, bool select) {
    auto panel        = new wxPanel(this->parent->m_chatListBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    auto* webView       = wxWebView::New(panel, wxID_ANY);
    const wxString html = sd_gui_utils::ChatMessageTemplate();
    webView->SetPage(html, wxT("/"));
    webView->EnableContextMenu(false);
    if (isDEBUG) {
        webView->EnableAccessToDevTools(true);
    }
    webView->EnableHistory(false);

    sizer->Add(webView, 1, wxEXPAND | wxALL, 0);
    panel->SetSizer(sizer);
    panel->Layout();

    this->parent->m_chatListBook->AddPage(panel, title, select);
    this->currentChatPanel = panel;
    this->chatPanels.push_back(panel);

    auto* panelData = new PanelData{webView, this->parent->chat_currentMessage, {}};
    panel->SetClientData(panelData);
    const auto webviewInfo = webView->GetBackendVersionInfo();
    std::cout << webviewInfo.ToString() << std::endl;
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
void sd_gui_utils::LlamaGuiHelper::UpdateChat() {
    auto panelData = static_cast<PanelData*>(this->currentChatPanel->GetClientData());
    if (!panelData || !panelData->webView) {
        return;
    }

    if (panelData->chatData == nullptr) {
        panelData->chatData = this->parent->chat_currentMessage;
    }

    for (const auto msg : this->parent->chat_currentMessage->GetMessages()) {
        const wxString sender = sd_gui_utils::llvmTextSenderMap.at(msg.second.sender);
        if (msg.second.text.empty()) {
            continue;
        }

        if (panelData->messageLastUpdates.contains(msg.first)) {
            if (panelData->messageLastUpdates[msg.first] != msg.second.updated_at) {
                panelData->messageLastUpdates[msg.first] = msg.second.updated_at;

                wxString text = msg.second.text;

                if (msg.second.sender != sd_gui_utils::llvmTextSender::LLVM_TEXT_SENDER_USER) {
                    text = sd_gui_utils::ConvertMarkdownToHtml(msg.second.text);
                } else {
                    text = sd_gui_utils::removeHTMLTags(text);
                    text = sd_gui_utils::nl2br(msg.second.text);
                }

                sd_gui_utils::UpdateChatMessage(panelData->webView, text, msg.first);
                std::cout << "Updating message: " << msg.first << " sender: " << sender << std::endl;
                return;
            }
        } else {
            panelData->messageLastUpdates[msg.first] = msg.second.updated_at;

            // there is no need markdown, just add the message
            sd_gui_utils::AddMessage(panelData->webView, sender, msg.second.text, msg.first);
            std::cout << "Adding message: " << msg.first << " sender: " << sender << std::endl;
            return;
        }
    }

    //
};