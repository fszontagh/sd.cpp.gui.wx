#ifndef __MainWindowUI__
#define __MainWindowUI__

/**
@file
Subclass of UI, which is generated by wxFormBuilder.
*/

#include "../helpers/ExternalProcessHelper.h"
#include "../helpers/ModelFileMoveThread.h"
#include "../helpers/simplecurl.h"
#include "../helpers/sslUtils.hpp"
#include "MainWindowAboutDialog.h"
#include "MainWindowDesktopWidget.h"
#include "MainWindowImageDialog.h"

#include "../helpers/DataViewListManager.hpp"
#include "../helpers/FormatStyledTextCtrl.h"
#include "../helpers/InPaintHelper.hpp"
#include "../helpers/ModelUiManager.hpp"

class MainApp;

/** Implementing UI */
class MainWindowUI : public mainUI {
protected:
    // Handlers for UI events.
    void OnClose(wxCloseEvent& event) override;
    void onSettings(wxCommandEvent& event) override;
    void onModelsRefresh(wxCommandEvent& event) override;
    void OnAboutButton(wxCommandEvent& event) override;
    void OnCivitAitButton(wxCommandEvent& event) override;
    void OnStopBackgroundProcess(wxCommandEvent& event) override;
    void onModelSelect(wxCommandEvent& event) override;
    void onTypeSelect(wxCommandEvent& event) override;
    void onVaeSelect(wxCommandEvent& event) override;
    void onSamplerSelect(wxCommandEvent& event) override;
    void onRandomGenerateButton(wxCommandEvent& event) override;
    void onSd15ResSelect(wxCommandEvent& event) override;
    void onSdXLResSelect(wxCommandEvent& event) override;
    void OnWHChange(wxCommandEvent& event) override;
    void onResolutionSwap(wxCommandEvent& event) override;
    void onSavePreset(wxCommandEvent& event) override;
    void onLoadPreset(wxCommandEvent& event) override;
    void onSelectPreset(wxCommandEvent& event) override;
    void onDeletePreset(wxCommandEvent& event) override;
    void m_notebook1302OnNotebookPageChanged(wxNotebookEvent& event) override;
    void OnJobListItemActivated(wxDataViewEvent& event) override;
    void OnJobListItemKeyDown(wxKeyEvent& event) override;
    void onContextMenu(wxDataViewEvent& event) override;
    void OnDataModelTreeContextMenu(wxTreeListEvent& event) override;
    void OnDataModelTreeColSorted(wxTreeListEvent& event) override;
    void OnDataModelTreeExpanded(wxTreeListEvent& event) override;
    void OnJobListItemSelection(wxDataViewEvent& event) override;
    void onTxt2ImgFileDrop(wxDropFilesEvent& event) override;
    // void OnPromptText(wxCommandEvent& event) override;
    // void OnNegPromptText(wxCommandEvent& event) override;
    void OnPromptKeyDown(wxKeyEvent& event) override;
    void OnTextChanged(wxStyledTextEvent& event);
    void OnAutoCompSelection(wxStyledTextEvent& event);
    void HighLightPrompts(wxStyledTextCtrl* stc);
    void InitPrompts();
    void onGenerate(wxCommandEvent& event) override;
    void OnControlnetImageOpen(wxFileDirPickerEvent& event) override;
    void OnControlnetImagePreviewButton(wxCommandEvent& event) override;
    void OnControlnetImageDelete(wxCommandEvent& event) override;
    void onFilePickerDiffusionModel(wxFileDirPickerEvent& event) override;
    void onFilePickerClipL(wxFileDirPickerEvent& event) override;
    void onFilePickerT5XXL(wxFileDirPickerEvent& event) override;
    void onCleanDiffusionModel(wxCommandEvent& event) override;
    void Onimg2imgDropFile(wxDropFilesEvent& event) override;
    void OnImageOpenFileChanged(wxFileDirPickerEvent& event) override;
    void OnImg2ImgPreviewButton(wxCommandEvent& event) override;
    void OnDeleteInitialImage(wxCommandEvent& event) override;
    // void OnMainSplitterSashPosChanged(wxSplitterEvent& event) override;
    void OnUpscalerDropFile(wxDropFilesEvent& event) override;
    void OnImageOpenFilePickerChanged(wxFileDirPickerEvent& event) override;
    void OnDeleteUpscaleImage(wxCommandEvent& event) override;
    void OnUpscalerModelSelection(wxCommandEvent& event) override;
    void OnUpscalerFactorChange(wxSpinEvent& event) override;
    void OnDataModelTreeSelected(wxTreeListEvent& event) override;
    void OnModelFavoriteChange(wxCommandEvent& event) override;
    void onCnOnCpu(wxCommandEvent& event) override;
    void onClipOnCpu(wxCommandEvent& event) override;
    void onVAEOnCpu(wxCommandEvent& event) override;
    void onDiffusionFlashAttn(wxCommandEvent& event) override;
    void OnImageInfoOpen(wxFileDirPickerEvent& event) override;
    void OnCleanImageInfo(wxCommandEvent& event) override;
    void OnImageInfoLoadTxt2img(wxCommandEvent& event) override;
    void OnImageInfoLoadImg2img(wxCommandEvent& event) override;
    void OnImageInfoCopyPrompt(wxCommandEvent& event) override;
    void OnImageInfoTryFindModel(wxCommandEvent& event) override;
    void OnShowWidget(wxCommandEvent& event) override;
    // painting on img2img
    void OnImg2ImgMouseDown(wxMouseEvent& event) override;
    void OnImg2ImgRMouseDown(wxMouseEvent& event) override;
    void OnImg2ImgRMouseUp(wxMouseEvent& event) override;
    void OnImg2ImgMouseUp(wxMouseEvent& event) override;
    void OnImg2ImgMouseMotion(wxMouseEvent& event) override;
    void OnImg2ImgPaint(wxPaintEvent& event) override;
    void OnImg2ImgEraseBackground(wxEraseEvent& event) override;
    // void OnImg2ImgMouseEnter(wxMouseEvent& event) override;
    void OnImg2ImgMouseLeave(wxMouseEvent& event) override;
    void OnImg2ImgMouseWheel(wxMouseEvent& event) override;
    void OnInpaintSaveMask(wxCommandEvent& event) override;
    void OnImg2ImgSize(wxSizeEvent& event) override;
    void OnInpaintResizeImage(wxCommandEvent& event) override;
    void OnInpaintInvertMask(wxCommandEvent& event) override;
    void OnInpaintMaskOpen(wxFileDirPickerEvent& event) override;
    void OnInPaintBrushStyleToggle(wxCommandEvent& event) override;
    void OnInpaintCleanMask(wxCommandEvent& event) override;
    void OnInpaintCanvasResizeApply(wxCommandEvent& event) override;
    void OnInpaintBrushSizeSliderScroll(wxScrollEvent& event) override;
    void OnInpaintZoomSliderScroll(wxScrollEvent& event) override;

    void OnServerSelect(wxCommandEvent& event) override;
    // chat things
    void OnSendChat(wxCommandEvent& event) override;
    void OnChatInputTextEnter(wxCommandEvent& event) override;
    void OnLanguageModelSelect(wxCommandEvent& event) override;
    // chat things

public:
    /** Constructor */
    MainWindowUI(wxWindow* parent, const std::string& stablediffusionDllName, const std::string& llamaDllName, const std::string& usingBackend, bool disableExternalProcessHandling, MainApp* mapp);
    //// end generated class members
    ~MainWindowUI();
    void OnThreadMessage(wxThreadEvent& e);
    void OnCivitAiThreadMessage(wxThreadEvent& e);

    void LoadPresets();
    void LoadPromptTemplates();
    void loadModelList();
    void loadLoraList();
    void loadVaeList();
    void loadTaesdList();
    void loadControlnetList();
    void loadEsrganList();
    void loadEmbeddingList();
    void loadSchedulerList();
    void loadSamplerList();
    void loadTypeList();

private:
    MainApp* mapp                                            = nullptr;
    std::unique_ptr<ModelUiManager> modelUiManager           = nullptr;
    std::unique_ptr<DataViewListManager> dataViewListManager = nullptr;

    bool m_autoCompJustCompleted = false;

    bool disableExternalProcessHandling               = false;
    std::atomic<unsigned int> jobsCountSinceSegfault  = {0};
    std::atomic<unsigned int> stepsCountSinceSegfault = {0};
    std::string usingBackend;

    std::vector<wxStaticBitmap*> modelImagePreviews;

    struct JobImagePreview {
        wxStaticBitmap* bitmap  = nullptr;
        bool need_to_download   = false;
        bool local_missing      = false;
        bool is_ok              = false;
        bool download_requested = false;
        std::string imgid       = {};
    };
    // store the image_info hash -> wxStaticBitmap
    std::map<std::string, JobImagePreview> jobImagePreviews;

    std::vector<std::shared_ptr<sd_gui_utils::networks::TcpClient>> tcpClients;

    wxTaskBarIcon* TaskBar;
    wxMenu* TaskBarMenu;
    wxBitmap TaskBarIcon;

    MainWindowDesktopWidget* widget      = nullptr;
    MainWindowCivitAiWindow* civitwindow = nullptr;
    MainWindowAboutDialog* aboutDialog   = nullptr;
    MainWindowSettings* settingsWindow   = nullptr;

    std::map<std::string, sd_gui_utils::generator_preset> Presets;
    std::map<std::string, sd_gui_utils::prompt_templates> PromptTemplates;

    std::shared_ptr<ModelInfo::Manager> ModelManager;
    std::shared_ptr<QM::QueueManager> qmanager;
    std::mutex mutex;
    std::mutex logMutex;
    std::mutex taskBarMutex;

    int init_width  = 512;
    int init_height = 512;

    std::vector<std::thread*> threads;
    wxBitmap ControlnetOrigPreviewBitmap;
    wxBitmap AppOrigPlaceHolderBitmap;

    std::shared_ptr<sd_gui_utils::InPaintHelper> inpaintHelper = nullptr;

    std::vector<sd_gui_utils::VoidHolder*> voids;

    // external process
    std::vector<std::shared_ptr<ExternalProcessHelper>> processHelpers;
    wxString extProcessLogFile = wxEmptyString;
    wxString llamaLogFile      = wxEmptyString;
    // external process
    // chat sessions
    std::mutex chat_mutex;
    std::shared_ptr<sd_gui_utils::llvmMessage> chat_currentMessage = nullptr;
    // chat sessions

    sd_gui_utils::LastItemNotification lastItemNotification = {};

    // std::ofstream logfile;
    wxFile logfile;
    std::unordered_map<wxString, wxString> lastImageInfoParams;

    std::vector<wxPanel*> chatPanels;
    wxPanel* currentChatPanel = nullptr;

    void OnCloseSettings(wxCloseEvent& event);
    void OnCloseCivitWindow(wxCloseEvent& event);
    void OnHtmlLinkClicked(wxHtmlLinkEvent& event) override;
    void onWhatIsThis(wxCommandEvent& event) override;
    void OnLoadPrevPrompt(wxCommandEvent& event) override;
    void OnClearPrompt(wxCommandEvent& event) override;
    void OnNormalizePrompt(wxCommandEvent& event) override;
    void OnExit(wxEvent& event);
    void OnPopupClick(wxCommandEvent& evt);
    void OnModelListPopUpClick(wxCommandEvent& evt);
    void LoadFileList(sd_gui_utils::DirTypes type = sd_gui_utils::DirTypes::CHECKPOINT);
    void ShowNotification(const wxString& title, const wxString& message);
    void ShowNotification(std::shared_ptr<QueueItem> jobItem);
    void ChangeModelByInfo(sd_gui_utils::ModelFileInfo* info);
    void ChangeGuiFromQueueItem(const QueueItem item);
    void UpdateModelInfoDetailsFromModelList(sd_gui_utils::ModelFileInfo* modelinfo);
    void UpdateJobInfoDetailsFromJobQueueList(std::shared_ptr<QueueItem> item);
    void UpdateJobImagePreviews(std::shared_ptr<QueueItem> jobItem = nullptr);
    void AddContextMenuToJobImagePreview(wxStaticBitmap* bitmap, std::shared_ptr<QueueItem> item, const sd_gui_utils::ImageInfo img, int gen_index);
    // external process callbacks
    bool ProcessEventHandler(const std::string& message, const std::string& error = "");
    void ProcessStdOutEvent(const char* bytes, size_t n);
    void ProcessStdErrEvent(const char* bytes, size_t n);
    // external process callbacks
    // chat updated event
    void UpdateChatGui();
    // chat update event
    void UpdateCurrentProgress(std::shared_ptr<QueueItem> item, const QueueEvents& event);
    void SetSchedulerByType(schedule_t schedule);
    void SetSamplerByType(sample_method_t sampler);
    void SetTypeByType(sd_type_t type);
    // chat things
    struct PanelData {
        wxScrolledWindow* scrolledWindow;
        wxBoxSizer* chatSizer;
        std::shared_ptr<sd_gui_utils::llvmMessage> chatData = nullptr;
        std::unordered_map<uint64_t, wxWebView*> messageWebViews;
        std::unordered_map<uint64_t, uint64_t> messageLastUpdates;
    };

    inline void addChatPanel(const wxString& title = _("New Chat"), bool select = true) {
        auto panel        = new wxPanel(this->m_chatListBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        auto scrolledWindow = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
        scrolledWindow->SetScrollRate(5, 5);

        wxBoxSizer* chatSizer = new wxBoxSizer(wxVERTICAL);
        scrolledWindow->SetSizer(chatSizer);
        scrolledWindow->Layout();

        sizer->Add(scrolledWindow, 1, wxEXPAND | wxALL, 5);
        panel->SetSizer(sizer);
        panel->Layout();

        this->m_chatListBook->AddPage(panel, title, select);
        this->currentChatPanel = panel;
        this->chatPanels.push_back(panel);

        auto* panelData = new PanelData{scrolledWindow, chatSizer, std::make_shared<sd_gui_utils::llvmMessage>()};
        panel->SetClientData(panelData);
    }

    void UpdateChatPanel(wxPanel* chatPanel) {
        std::lock_guard<std::mutex> lock(this->chat_mutex);
        if (!chatPanel || !this->chat_currentMessage) {
            return;
        }

        const auto status_msg = this->chat_currentMessage->GetStatusMessage();
        if (this->chat_currentMessage->GetStatus() == sd_gui_utils::llvmstatus::LLVM_STATUS_ERROR) {
            this->writeLog(status_msg, true);
        }

        if (this->m_chatStatus->GetLabel() != status_msg) {
            this->m_chatStatus->SetLabel(status_msg);
        }
        if (this->chat_currentMessage->GetStatus() == sd_gui_utils::llvmstatus::LLVM_STATUS_MODEL_LOADED) {
            if (status_msg.empty()) {
                this->m_chatStatus->SetLabel(_("Model loaded"));
                auto model = this->ModelManager->getInfo(this->chat_currentMessage->GetModelPath());
                if (model != nullptr) {
                    this->writeLog(wxString::Format(_("Llama model loaded: %s"), model.name), true);
                }
                this->m_languageModel->Enable();
                this->m_chatInput->Enable();
                this->m_sendChat->Enable();
            }
            return;
        }
        if (this->chat_currentMessage->GetStatus() == sd_gui_utils::llvmstatus::LLVM_STATUS_MODEL_UNLOADED) {
            if (status_msg.empty()) {
                this->m_chatStatus->SetLabel(_("Model unloaded"));
                auto model = this->ModelManager->getInfo(this->chat_currentMessage->GetModelPath());
                if (model != nullptr) {
                    this->writeLog(wxString::Format(_("Llama model unloaded: %s"), model.name), true);
                }
                this->m_languageModel->SetSelection(0);  // ensure the first is selected
                this->m_languageModel->Enable();
                this->m_chatInput->Disable();
                this->m_sendChat->Disable();
            }
            return;
        }

        auto* panelData = static_cast<PanelData*>(chatPanel->GetClientData());
        if (!panelData || !panelData->chatData)
            return;

        if (panelData->chatData->CheckUpdatedAt(this->chat_currentMessage->GetUpdatedAt())) {
            return;
        }

        for (const auto& [id, msg] : this->chat_currentMessage->GetMessages()) {
            wxString htmlMessage = wxString::Format("<p>%s <b>%s</b> %s</p>",
                                                    sd_gui_utils::formatTimestamp(msg.updated_at),
                                                    (msg.sender == sd_gui_utils::llvmTextSender::USER) ? "User" : "Assistant",
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
    }

    // chat things

    inline static size_t findLoraDistance(const wxString& text, size_t cursorPos) {
        const wxString pattern = "<lora:";
        wxString part          = text.SubString(0, cursorPos);
        if (part.Contains(pattern) == false) {
            return wxNOT_FOUND;
        }
        auto t = part.Find(':', true) + 1;
        if (t == wxNOT_FOUND) {
            return t;
        }
        wxString subText = part.Mid(t - pattern.Length(), pattern.Length());
        if (subText != "<lora:") {
            return wxNOT_FOUND;
        }
        if (t == cursorPos) {
            return wxNOT_FOUND;
        }
        if (t > cursorPos) {
            return wxNOT_FOUND;
        }

        return t;
    }
    inline static bool AreParenthesesBalanced(const wxString& input) {
        std::stack<wchar_t> stack;

        for (size_t i = 0; i < input.Length(); ++i) {
            if (input[i] == '(') {
                stack.push('(');
            } else if (input[i] == ')') {
                if (stack.empty()) {
                    return false;
                }
                stack.pop();
            }
        }
        return stack.empty();
    }
    inline wxString NormalizePrompt(const wxString& input) {
        if (!AreParenthesesBalanced(input)) {
            this->writeLog(_("Parentheses are not balanced!"));
            return input;
        }

        wxString result;
        int depth = 0;
        wxString buffer;

        for (size_t i = 0; i < input.Length(); ++i) {
            if (input[i] == '(') {
                if (depth > 0) {
                    buffer += '(';  // A mélyebb zárójelekbe belépve
                }
                ++depth;
            } else if (input[i] == ')') {
                --depth;
                if (depth == 0) {
                    // Ha elértük a legkülső zárójelet
                    if (wxRegEx(":[0-9]+\\.[0-9]+").Matches(buffer)) {
                        result += "(" + buffer + ")";
                    } else {
                        // A súly kiszámítása a mélység alapján
                        double weight = 1.0 + (depth + 1) * 0.1;
                        wxString weightStr;
                        weightStr.Printf("%.1f", weight);
                        weightStr.Replace(",", ".");
                        // Dupla zárójel esetén megfelelő formázás
                        if (buffer.StartsWith("(") && buffer.EndsWith(")")) {
                            result += "(" + buffer + ")";
                        } else {
                            result += "(" + buffer + ":" + weightStr + ")";
                        }
                    }
                    buffer.clear();  // A következő zárójelhez tiszta buffer
                } else {
                    buffer += ')';  // Mélyebb zárójel belsejében maradunk
                }
            } else {
                if (depth > 0) {
                    buffer += input[i];  // Zárójelbe tett karakterek hozzáadása
                } else {
                    result += input[i];  // Külső karakterek hozzáadása
                }
            }
        }

        return result;
    }

    inline void NormalizeTextPrompt(wxString& prompt) {
        if (prompt.EndsWith(",")) {
            prompt.Remove(prompt.Length() - 1, 1);
        }
        prompt.Trim();
        prompt.Trim(false);
        prompt.Replace(",,", ",");
        prompt.Replace(" ,", ",");
        prompt.Replace("  ", " ");
        // prompt = this->NormalizePrompt(prompt);
        if (!AreParenthesesBalanced(prompt)) {
            this->writeLog(_("Parentheses are not balanced!"));
        } else {
            wxString output = prompt;
            wxRegEx re("\\({2,}([^()]+)\\){2,}");
            int level = 1;

            while (re.Matches(output)) {
                wxString match       = re.GetMatch(output, 1);
                wxString replacement = wxString::Format("(%s:1.%d)", match, level++);
                re.Replace(&output, replacement, false);
            }

            prompt = output;
        }
    }
    inline void GeneratePromptCombinations(const wxString& input, std::vector<wxString>& results, size_t pos = 0) {
        wxRegEx regex("\\{([^}]+)\\}");
        wxString current = input;

        if (regex.Matches(current.Mid(pos))) {
            size_t start, len;

            regex.GetMatch(&start, &len);
            wxString match = current.Mid(start, len);

            wxString options         = regex.GetMatch(current, 1);
            wxArrayString variations = wxSplit(options, '|');

            for (const auto& variation : variations) {
                wxString temp = current;
                temp.Replace(match, variation, false);
                this->GeneratePromptCombinations(temp, results, start + variation.Length());
            }
        } else {
            results.push_back(current);
        }
    }
    inline static int CountTokens(const wxString& input) {
        int tokenCount = 0;
        wxRegEx tokenRegex("[^\\s]+");

        size_t start = 0;
        while (tokenRegex.Matches(input.Mid(start))) {
            size_t matchStart = 0, matchLength = 0;

            if (tokenRegex.GetMatch(&matchStart, &matchLength)) {
                ++tokenCount;
                start += matchStart + matchLength;
            } else {
                break;
            }
        }

        return tokenCount;
    }
    inline void CheckQueueButton() {
        auto current_tab = (sd_gui_utils::GuiMainPanels)this->m_notebook1302->GetSelection();
        switch (current_tab) {
            case sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO:
            case sd_gui_utils::GuiMainPanels::PANEL_MODELS:
            case sd_gui_utils::GuiMainPanels::PANEL_QUEUE: {
                this->m_queue->Disable();
            } break;
            case sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG: {
                if (this->m_model->GetSelection() != 0) {
                    this->m_queue->Enable();
                } else {
                    this->m_queue->Disable();
                }
            } break;
            case sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG: {
                if (!this->m_img2imgOpen->GetPath().empty() && !wxFileName(this->m_img2imgOpen->GetPath()).IsDir()) {
                    this->m_queue->Enable();
                }
                if (this->m_model->GetSelection() == 0) {
                    this->m_queue->Disable();
                } else {
                    this->m_queue->Enable();
                }
            } break;
            case sd_gui_utils::GuiMainPanels::PANEL_UPSCALER: {
                if (this->m_upscaler_model->GetCurrentSelection() != 0) {
                    if (!this->m_upscaler_filepicker->GetPath().empty()) {
                        this->m_queue->Enable();
                        return;
                    }
                }
                this->m_queue->Disable();
            }
            default:
                this->m_queue->Disable();
                break;
        }
    }
    inline void DisableControlNet() {
        this->m_controlnetModels->Disable();
        this->m_controlnetModels->Select(0);
        this->m_controlnetImageDelete->Disable();
        this->m_controlnetImageOpen->Disable();
        this->m_controlnetStrength->Disable();
        this->m_controlnetImageOpen->SetPath("");
        auto origSize = this->m_controlnetImagePreview->GetSize();
        this->m_controlnetImagePreview->SetBitmap(this->ControlnetOrigPreviewBitmap);
        this->m_controlnetImagePreview->SetSize(origSize);
        this->m_controlnetImagePreviewButton->Disable();
    };

    inline void EnableControlNet() {
        this->m_controlnetModels->Enable();
        this->m_controlnetImageDelete->Enable();
        this->m_controlnetImageOpen->Enable();
        this->m_controlnetStrength->Enable();
    };

    inline void DisableModelSelect() {
        this->m_model->Select(0);
        this->m_model->Disable();
    }
    inline void EnableModelSelect() { this->m_model->Enable(); }

    static void ModelHashingCallback(size_t readed_size, std::string sha256, void* custom_pointer);
    static void ModelStandaloneHashingCallback(size_t readed_size, std::string sha256, void* custom_pointer);

    // std::shared_ptr<QueueItem> handleSdImage(std::string result, std::shared_ptr<QueueItem> itemPtr, wxEvtHandler* eventHandler);
    std::shared_ptr<QueueItem> handleSdImages(std::shared_ptr<QueueItem> itemPtr);

    wxString paramsToImageComment(const QueueItem& myItem);
    std::string paramsToImageCommentJson(QueueItem myItem, sd_gui_utils::ModelFileInfo modelInfo);
    void imageCommentToGuiParams(std::unordered_map<wxString, wxString> params, SDMode mode);
    void onimg2ImgImageOpen(const wxString& file, bool forceResolutions = false);
    void onimgInfoOpen(const wxString& file);
    void onUpscaleImageOpen(const wxString& file);
    void onControlnetImageOpen(const wxString& file);

    void readMetaDataFromImage(const wxFileName& file, const SDMode mode, bool askForLoadParameters = false);
    std::unordered_map<wxString, wxString> getMetaDataFromImage(const wxFileName& file);

    // clean up image informations
    void cleanUpImageInformations();

    void PrepareModelConvert(sd_gui_utils::ModelFileInfo* modelInfo);

    // start a thread to generate image
    void StartGeneration(std::shared_ptr<QueueItem> myJob);

    template <typename T>
    static void SendThreadEvent(wxEvtHandler* eventHandler, QueueEvents eventType, const T& payload, std::string text = "");

    template <typename T>
    static void SendThreadEvent(wxEvtHandler* eventHandler, sd_gui_utils::ThreadEvents eventType, const T& payload, std::string text = "");

    template <typename T>
    void SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, std::string text = "");

    // generate the hash for a model, from the model table list
    void threadedModelHashCalc(wxEvtHandler* eventHandler, sd_gui_utils::ModelFileInfo* modelinfo);
    void threadedModelInfoDownload(wxEvtHandler* eventHandler, sd_gui_utils::ModelFileInfo* modelinfo);
    void threadedModelInfoImageDownload(wxEvtHandler* eventHandler, sd_gui_utils::ModelFileInfo* modelinfo);

    void initLog();
    void deInitLog();
    void writeLog(const wxString& msg, bool writeIntoGui = true, bool debug = false);
    void writeLog(const std::string& message);
};

#endif  // __MainWindowUI__
