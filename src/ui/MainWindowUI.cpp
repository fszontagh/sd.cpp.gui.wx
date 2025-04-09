#include "MainWindowUI.h"

#ifndef APP_PNG_H
#include "embedded_files/app_icon.h"
#endif

#ifndef BLANKIMAGE_PNG_H
#include "embedded_files/blankimage.png.h"
#endif

#ifndef DELETED_PNG_H
#include "embedded_files/deleted.png.h"
#endif

#ifndef CLOUD_DOWNLOAD_PNG_H
#include "embedded_files/cloud_download.png.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"

MainWindowUI::MainWindowUI(wxWindow* parent, const std::string& stablediffusionDllName, const std::string& llamaDllName, const std::string& usingBackend, bool disableExternalProcessHandling, MainApp* mapp)
    : mainUI(parent), usingBackend(usingBackend), disableExternalProcessHandling(disableExternalProcessHandling), mapp(mapp) {
    this->ControlnetOrigPreviewBitmap = this->m_controlnetImagePreview->GetBitmap();

    if (isDEBUG) {
        this->SetTitle(wxString::Format("%s - %s (%s) - %s", PROJECT_DISPLAY_NAME, SD_GUI_VERSION, GIT_HASH, BUILD_TYPE));
    } else {
        this->SetTitle(wxString::Format("%s - %s (%s)", PROJECT_DISPLAY_NAME, SD_GUI_VERSION, GIT_HASH));
    }

    this->TaskBar = new wxTaskBarIcon();

    this->TaskBarIcon = app_png_to_wx_bitmap();
    this->TaskBar->SetIcon(this->TaskBarIcon, wxString::Format("%s - %s (%s)", PROJECT_DISPLAY_NAME, SD_GUI_VERSION, GIT_HASH));
    this->TaskBar->Bind(wxEVT_TASKBAR_LEFT_DOWN, [this](wxTaskBarIconEvent& event) {
        if (this->IsIconized()) {
            this->Raise();
        } else {
            this->Iconize();
        }
    });

    wxIcon icon;
    icon.CopyFromBitmap(this->TaskBarIcon);
    this->SetIcon(icon);

    this->TaskBar->Bind(wxEVT_TASKBAR_RIGHT_DOWN, [this, icon](wxTaskBarIconEvent& event) {
        wxMenu* taskMenu = new wxMenu();
        taskMenu->Append(wxID_EXIT);
        this->TaskBar->PopupMenu(taskMenu);
        taskMenu->Bind(wxEVT_MENU, &MainWindowUI::OnExit, this);
        delete taskMenu;
    });

    if (this->mapp->cfg->enable_civitai == false) {
        this->m_civitai->Hide();
    } else {
        this->m_civitai->Show();
    }

    if (wxPersistentRegisterAndRestore(this, "maingui") == false) {
        this->Maximize();
        this->Restore();
    }

    this->qmanager       = std::make_shared<QM::QueueManager>(this->GetEventHandler(), this->mapp->cfg->jobs);
    this->ModelManager   = std::make_shared<ModelInfo::Manager>(this->mapp->cfg->datapath);
    this->modelUiManager = std::make_unique<ModelUiManager>(this->m_modelTreeList, this->mapp->cfg);

    this->modelUiManager->AddDirTypeSelector(this->m_model, sd_gui_utils::DirTypes::CHECKPOINT);
    this->modelUiManager->AddDirTypeSelector(this->m_taesd, sd_gui_utils::DirTypes::TAESD);
    this->modelUiManager->AddDirTypeSelector(this->m_vae, sd_gui_utils::DirTypes::VAE);
    this->modelUiManager->AddDirTypeSelector(this->m_upscaler_model, sd_gui_utils::DirTypes::ESRGAN);
    this->modelUiManager->AddDirTypeSelector(this->m_controlnetModels, sd_gui_utils::DirTypes::CONTROLNET);
    this->modelUiManager->AddDirTypeSelector(this->m_languageModel, sd_gui_utils::DirTypes::LLAMA_MODELS);

    this->dataViewListManager = std::make_unique<DataViewListManager>(this->m_joblist, this->qmanager, this->mapp->cfg);

    this->m_modelTreeList->ClearColumns();

    this->modelUiManager->AppendColumn(_("Name"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
    this->modelUiManager->AppendColumn(_("Size"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
    this->modelUiManager->AppendColumn(_("Type"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);
    this->modelUiManager->AppendColumn(_("Hash"), wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE | wxCOL_SORTABLE);

    this->m_modelTreeList->SetSortColumn(0, true);
    this->m_joblist->GetColumn(1)->SetSortOrder(false);

    this->initLog();
    Bind(wxEVT_THREAD, &MainWindowUI::OnThreadMessage, this);

    // load -> moved into the MainApp.h
    /*
    this->LoadPresets();
    this->LoadPromptTemplates();
    this->loadModelList();
    this->loadLoraList();
    this->loadVaeList();
    this->loadTaesdList();
    this->loadControlnetList();
    this->loadEsrganList();
    this->loadEmbeddingList();
    this->loadSchedulerList();
    this->loadSamplerList();
    this->loadTypeList();

    */

    // reload config, set up checkboxes from it
    const auto diffusion_flash_attn = this->mapp->config->ReadBool("/diffusion_flash_attn", this->diffusionFlashAttn->GetValue());
    const auto clip_on_cpu          = this->mapp->config->ReadBool("/clip_on_cpu", this->clipOnCpu->GetValue());
    const auto controlnet_on_cpu    = this->mapp->config->ReadBool("/controlnet_on_cpu", this->cnOnCpu->GetValue());
    const auto vae_on_cpu           = this->mapp->config->ReadBool("/vae_on_cpu", this->vaeOnCpu->GetValue());
    const auto widgetVisible        = this->mapp->config->ReadBool("/widgetVisible", this->mapp->cfg->widgetVisible);

    this->m_imageInfoOpen->SetPath(this->mapp->cfg->lastImageInfoPath.empty() ? this->mapp->cfg->output : this->mapp->cfg->lastImageInfoPath);
    this->m_img2imgOpen->SetPath(this->mapp->cfg->lastImg2ImgPath.empty() ? this->mapp->cfg->output : this->mapp->cfg->lastImg2ImgPath);
    this->m_upscaler_filepicker->SetPath(this->mapp->cfg->lastUpscalerPath.empty() ? this->mapp->cfg->output : this->mapp->cfg->lastUpscalerPath);

    this->diffusionFlashAttn->SetValue(diffusion_flash_attn);
    this->clipOnCpu->SetValue(clip_on_cpu);
    this->cnOnCpu->SetValue(controlnet_on_cpu);
    this->vaeOnCpu->SetValue(vae_on_cpu);

    if (widgetVisible) {
        this->m_showWidget->SetToolTip(_("Hide Widget"));
        if (this->widget == nullptr) {
            this->widget = new MainWindowDesktopWidget(NULL);
            this->widget->PassParentEventHandler(this->GetEventHandler());
            wxPersistenceManager::Get().RegisterAndRestore(this->widget);
        }
        this->widget->ShowWithEffect(wxShowEffect::wxSHOW_EFFECT_BLEND, 1000);
        this->m_showWidget->SetValue(true);
    }

    this->m_rightMainPanel->Hide();
    this->m_promptAndFluxPanel->Hide();
    this->inpaintHelper = std::make_shared<sd_gui_utils::InPaintHelper>(this->m_img2imPanel);
    this->inpaintHelper->SetBrushSize(sd_gui_utils::InPaintHelper::BrushSize(this->m_inpaintBrushSizeSlider->GetMin(),
                                                                             this->m_inpaintBrushSizeSlider->GetMax(),
                                                                             this->m_inpaintBrushSizeSlider->GetValue()));

    this->m_inpaintZoomSlider->SetLineSize(this->inpaintHelper->GetZoomStep());
    this->llamaGuiHelper = std::make_shared<sd_gui_utils::LlamaGuiHelper>(this);

    this->InitPrompts();

    //
    /*
        if (this->mapp->cfg->servers.empty() == false) {
            for (auto& server : this->mapp->cfg->ListRemoteServers()) {
                if (server->IsEnabled() == false) {
                    continue;
                }
                server->StartServer();
            }
        }
    */
    this->m_upscalerHelp->SetPage(wxString::Format((_("Officially from sd.cpp, the following upscaler model is supported: <br/><a href=\"%s\">RealESRGAN_x4Plus Anime 6B</a><br/>This is working sometimes too: <a href=\"%s\">RealESRGAN_x4Plus</a>")), wxString("https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.2.4/RealESRGAN_x4plus_anime_6B.pth"), wxString("https://civitai.com/models/147817/realesrganx4plus")));

    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, WXK_RETURN, this->m_queue->GetId());  // Ctrl + Enter
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    if (this->disableExternalProcessHandling == false) {
        this->m_stop_background_process->Show();

        // external process for stable diffusion
        wxArrayString sdParams;

        sdParams.Add(ExternalProcessHelper::buildDllPathFromName(stablediffusionDllName));
        sdParams.Add(this->extProcessLogFile);

        auto processRef = std::make_shared<ExternalProcessHelper>(
            EPROCESS_BINARY_NAME,
            ExternalProcessHelper::ProcessType::diffuser,
            sdParams,
            SHARED_MEMORY_PATH_DIFFUSER,
            SHARED_MEMORY_SIZE_DIFFUSER);
        this->processHelpers.push_back(processRef);

        processRef->onStart = [this, processRef]() {
            this->writeLog(wxString::Format(_("External process started: %s"), processRef->GetFullCommand()));
            auto* event = new wxThreadEvent();
            event->SetString(_("Process is ready"));
            event->SetId(9999);
            wxQueueEvent(this, event);
        };

        processRef->onExit = [this, processRef](bool restart) {
            if (restart) {
                return;
            }
            this->writeLog(_("External process stopped"));
            auto* event = new wxThreadEvent();
            event->SetString(_("External process stopped"));
            event->SetId(9999);
            wxQueueEvent(this, event);
        };

        processRef->onStdErr = [this](const char* data, size_t size) {
            this->ProcessStdErrEvent(data, size);
        };

        processRef->onStdOut = [this](const char* data, size_t size) {
            this->ProcessStdOutEvent(data, size);
        };

        processRef->onShmMessage = [this](const char* data, size_t size) {
            return this->ProcessEventHandler(std::string(data, size));
        };

        if (processRef->Start()) {
            this->writeLog(wxString::Format(_("External process just started: %s"), processRef->GetFullCommand()));
        } else {
            this->writeLog(wxString::Format(_("External process failed to start: %s"), processRef->GetFullCommand()));
        }

        // external process for stable diffusion
        // external process for llama
        wxArrayString llamaParams;

        llamaParams.Add(ExternalProcessHelper::buildDllPathFromName(llamaDllName));
        llamaParams.Add(this->llamaLogFile);

        auto llamaRef = std::make_shared<ExternalProcessHelper>(
            LLAMA_BINARY_NAME,
            ExternalProcessHelper::ProcessType::llama,
            llamaParams,
            SHARED_MEMORY_PATH_LLAMA,
            SHARED_MEMORY_SIZE_LLAMA);
        this->processHelpers.emplace_back(llamaRef);

        llamaRef->onStart = [this, llamaRef]() {
            this->writeLog(wxString::Format(_("Llama process started: %s"), llamaRef->GetFullCommand()));
            auto* event = new wxThreadEvent();
            event->SetString(_("Llama is ready"));
            event->SetId(10000);
            wxQueueEvent(this, event);
        };

        llamaRef->onStdErr = [this](const char* data, size_t size) {
            this->ProcessStdErrEvent(data, size);
        };

        llamaRef->onStdOut = [this](const char* data, size_t size) {
            this->ProcessStdOutEvent(data, size);
        };

        llamaRef->onExit = [this, llamaRef](bool restart) {
            if (restart) {
                return;
            }
            this->writeLog(wxString::Format(_("Llama process stopped: %s"), llamaRef->GetFullCommand()));
            auto* event = new wxThreadEvent();
            event->SetString(_("Llama is down"));
            event->SetId(10000);
            wxQueueEvent(this, event);
            //
            if (this->chat_currentMessage != nullptr) {
                // lock the chat mutex
                {
                    std::lock_guard<std::mutex> lock(this->chat_mutex);
                    if (this->chat_currentMessage->GetStatusMessage().empty()) {
                        this->chat_currentMessage->SetStatusMessage(_("LLama process died withour error"));
                        this->m_chatStatus->SetLabel(this->chat_currentMessage->GetStatusMessage());
                        for (auto& helper : this->processHelpers) {
                            helper->clean();
                            break;
                        }
                    }
                }
            }
            llamaRef->Restart();
        };

        llamaRef->onShmMessage = [this](const char* data, size_t size) {
            try {
                const auto msg                       = nlohmann::json::parse(std::string(data, size));
                const sd_gui_utils::llvmMessage lmsg = msg.get<sd_gui_utils::llvmMessage>();
                std::lock_guard<std::mutex> lock(this->chat_mutex);
                if (this->chat_currentMessage->CheckUpdatedAt(lmsg.GetUpdatedAt()) == false) {
                    this->chat_currentMessage->Update(lmsg);
                    auto* event = new wxThreadEvent();
                    event->SetId(10001);
                    wxQueueEvent(this, event);
                    return true;
                }
                return false;  // true to clear the shm false to keep it
            } catch (const std::exception& e) {
                return false;
            }
            return false;
        };

        if (llamaRef->Start()) {
            this->writeLog(wxString::Format(_("Llama process just started: %s"), llamaRef->GetFullCommand()));
            this->llamaGuiHelper->addChatPanel();
        } else {
            this->writeLog(wxString::Format(_("Llama process failed to start: %s"), llamaRef->GetFullCommand()));
        }
        // external process for llama
    }
}
void MainWindowUI::OnClose(wxCloseEvent& event) {
    event.Skip();
}
void MainWindowUI::onSettings(wxCommandEvent& event) {
    if (this->qmanager->IsRunning()) {
        wxMessageDialog(this, _("Please wait to finish the currently running jobs!")).ShowModal();
        return;
    }
    if (this->settingsWindow != nullptr) {
        this->settingsWindow->Destroy();
        this->settingsWindow = nullptr;
    }
    auto bitmap = app_png_to_wx_bitmap();
    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    this->settingsWindow = new MainWindowSettings(this, this->mapp->config, this->mapp->cfg);
    wxPersistenceManager::Get().RegisterAndRestore(this->settingsWindow);
    this->settingsWindow->SetIcon(icon);
    this->Iconize();
    this->Raise();
    this->settingsWindow->Bind(wxEVT_CLOSE_WINDOW, &MainWindowUI::OnCloseSettings, this);
    // this->Maximize(false);
    this->settingsWindow->Show();
}

void MainWindowUI::onModelsRefresh(wxCommandEvent& event) {
    this->modelUiManager->CleanAll();
    this->loadModelList();
    this->loadLoraList();
    this->loadVaeList();
    this->loadTaesdList();
    this->loadControlnetList();
    this->loadEsrganList();
    this->loadEmbeddingList();
    this->LoadPresets();
    this->LoadPromptTemplates();
    // get the list from the active server too
    auto srvlist = this->mapp->cfg->ListRemoteServers();
    for (size_t i = 0; i < srvlist.size(); i++) {
        auto srv = srvlist.at(i);
        if (srv) {
            if (srv->IsEnabled() && srv->IsConnected()) {
                srv->RequestModelList();
            }
        }
    }
}

void MainWindowUI::OnAboutButton(wxCommandEvent& event) {
    if (this->aboutDialog != nullptr) {
        this->aboutDialog->SetFocus();
        this->aboutDialog->RequestUserAttention();
        return;
    }
    this->aboutDialog = new MainWindowAboutDialog(this);

    wxString about = wxString(_("<p><strong>Disclaimer</strong></p><p>Use of this application is at your own risk. The developer assumes no responsibility or liability for any potential data loss, damage, or other issues arising from its usage. By using this application, you acknowledge and accept these terms.</p>"));

    // about.Append(wxString::Format(
    //     "<h2>%s</h2><p>Version: %s </p><p>Git version: %s</p><p>Website: <a target='_blank' href='%s'>%s</a></p><hr/>",
    //     PROJECT_NAME, SD_GUI_VERSION, GIT_HASH, SD_GUI_HOMEPAGE, PROJECT_NAME));

    about.Append(wxString::Format("<h2>%s</h2>", PROJECT_NAME));
    about.Append(wxString::Format(_("<p>Version: %s</p>"), SD_GUI_VERSION));
    about.Append(wxString::Format(_("<p>Git version: %s</p>"), GIT_HASH));
    about.Append(wxString::Format(_("<p>Stable Diffusion CPP version: %s</p>"), SD_CPP_VERSION));
    about.Append(wxString::Format(_("<p>Website: <a target='_blank' href='%s'>%s</a></p>"), SD_GUI_HOMEPAGE, SD_GUI_HOMEPAGE));

    about.Append(wxString::Format(_("<p>Loaded backend: %s</p>"), usingBackend.c_str()));
    about.append(wxString::Format(
        "<p>%s %s</p><p>%s %s</p>", _("Configuration folder:"),
        wxStandardPaths::Get().GetUserConfigDir(), ("Data folder:"), this->mapp->cfg->datapath));

    wxString format_string = "<p>%s %s</p>";
    about.Append(wxString::Format(format_string, _("Model folder:"), this->mapp->cfg->model));
    about.Append(wxString::Format(format_string, _("Embedding folder:"), this->mapp->cfg->embedding));
    about.Append(wxString::Format(format_string, _("Lora folder:"), this->mapp->cfg->lora));
    about.Append(wxString::Format(format_string, _("Vae folder:"), this->mapp->cfg->vae));
    about.Append(wxString::Format(format_string, _("Controlnet folder:"), this->mapp->cfg->controlnet));
    about.Append(wxString::Format(format_string, _("ESRGAN folder:"), this->mapp->cfg->esrgan));
    about.Append(wxString::Format(format_string, _("TAESD folder:"), this->mapp->cfg->taesd));
    about.Append(wxString::Format(format_string, _("Output folder:"), this->mapp->cfg->output));
    about.Append(wxString::Format(format_string, _("Jobs folder:"), this->mapp->cfg->jobs));
    about.Append(wxString::Format(format_string, _("Presets folder:"), this->mapp->cfg->presets));

    about.append(wxString("<pre>"));
    about.append(wxString(_("N/A")));
    about.append(wxString("</pre>"));
    this->aboutDialog->m_about->SetPage(about);
    this->aboutDialog->SetIcon(this->GetIcon());
    this->aboutDialog->ShowModal();
}

void MainWindowUI::OnCivitAitButton(wxCommandEvent& event) {
    if (this->civitwindow == nullptr) {
        this->civitwindow = new MainWindowCivitAiWindow(this);
        auto bitmap       = app_png_to_wx_bitmap();
        wxIcon icon;
        icon.CopyFromBitmap(bitmap);

        this->civitwindow->SetIcon(icon);
        this->civitwindow->SetModelManager(this->ModelManager);
        this->civitwindow->SetCfg(this->mapp->cfg);
        this->civitwindow->SetTitle(wxString::Format("%s | %s", this->GetTitle(), this->civitwindow->GetTitle()));
        this->civitwindow->Bind(wxEVT_THREAD, &MainWindowUI::OnCivitAiThreadMessage, this);
        this->civitwindow->Bind(wxEVT_CLOSE_WINDOW, &MainWindowUI::OnCloseCivitWindow, this);
        this->civitwindow->SetName("CivitAiClient");
        wxPersistenceManager::Get().RegisterAndRestore(this->civitwindow);
        this->civitwindow->Show();
        return;
    }
    this->civitwindow->RequestUserAttention();
    this->civitwindow->SetFocus();
}

void MainWindowUI::OnStopBackgroundProcess(wxCommandEvent& event) {
    // stop the background process
}

void MainWindowUI::onModelSelect(wxCommandEvent& event) {
    event.Skip();
    // check if really selected a model, or just the first element, which is
    // always exists...
    if (this->m_model->GetSelection() == 0) {
        this->m_queue->Disable();
        this->m_statusBar166->SetStatusText(_("No model selected"));
        return;
    }
    auto name = this->m_model->GetStringSelection().utf8_string();
    this->CheckQueueButton();

    this->m_statusBar166->SetStatusText(wxString::Format(_("Model: %s"), name));
    this->m_model->SetToolTip(wxString::Format(_("Model: %s"), name));

    // select the server by the model's server
    auto mindex    = this->m_model->GetCurrentSelection();
    auto data      = this->m_model->GetClientData(mindex);
    auto modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(data);
    if (modelinfo && modelinfo->server_id.empty() == false) {
        for (int i = this->m_server->GetCount() - 1; i > 0; i--) {
            auto srv = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(i));
            if (srv && srv->GetId() == modelinfo->server_id) {
                this->m_server->SetSelection(i);
                break;
            }
        }
    } else {
        this->m_server->SetSelection(0);  // select the local
    }

    this->HighLightPrompts(this->m_prompt);
    this->HighLightPrompts(this->m_prompt2);
    this->HighLightPrompts(this->m_neg_prompt);
    this->HighLightPrompts(this->m_neg_prompt2);
}

void MainWindowUI::onTypeSelect(wxCommandEvent& event) {
    // TODO: Implement onTypeSelect
}

void MainWindowUI::onVaeSelect(wxCommandEvent& event) {
    this->m_vae->SetToolTip(this->m_vae->GetStringSelection());
}

void MainWindowUI::onRandomGenerateButton(wxCommandEvent& event) {
    this->m_seed->SetValue(sd_gui_utils::generateRandomInt(100000000, 999999999));
}

void MainWindowUI::onSd15ResSelect(wxCommandEvent& event) {
    const auto index = this->m_sd15Res->GetSelection();
    if (index < 1) {
        return;
    }
    const auto text = this->m_sd15Res->GetString(index);
    size_t pos      = text.find('x');
    auto w          = text.substr(0, pos);
    auto h          = text.substr(pos + 1);
    this->m_width->SetValue(w);
    this->m_height->SetValue(h);
}

void MainWindowUI::onSdXLResSelect(wxCommandEvent& event) {
    const auto index = this->m_sdXlres->GetSelection();
    if (index < 1) {
        return;
    }
    const auto text = this->m_sdXlres->GetString(index);
    size_t pos      = text.find('x');
    auto w          = text.substr(0, pos);
    auto h          = text.substr(pos + 1);
    this->m_width->SetValue(w);
    this->m_height->SetValue(h);
}

void MainWindowUI::OnWHChange(wxCommandEvent& event) {
    int w, h;
    this->m_width->GetValue().ToInt(&w);
    this->m_height->GetValue().ToInt(&h);

    this->init_width  = w;
    this->init_height = h;
    this->CheckQueueButton();
}

void MainWindowUI::onResolutionSwap(wxCommandEvent& event) {
    auto oldW = this->m_width->GetValue();
    auto oldH = this->m_height->GetValue();

    this->m_height->SetValue(oldW);
    this->m_width->SetValue(oldH);
    this->CheckQueueButton();
}

void MainWindowUI::m_notebook1302OnNotebookPageChanged(wxNotebookEvent& event) {
    event.Skip();

    if (event.GetSelection() == wxNOT_FOUND) {
        return;
    }

    auto obj = dynamic_cast<wxNotebook*>(event.GetEventObject());
    if (obj != this->m_notebook1302) {
        return;
    }

    sd_gui_utils::GuiMainPanels selected = static_cast<sd_gui_utils::GuiMainPanels>(event.GetSelection());
    // show hide side panels

    // show hide prompt inputs
    switch (selected) {
        case sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG: {
            this->m_prompt2->Hide();
            this->m_neg_prompt2->Hide();
            this->m_prompt->Show();
            this->m_neg_prompt->Show();
            this->m_fluxPanel->Show();

            if (this->m_promptAndFluxPanel->Show()) {
                this->bSizer138->Layout();
            }

            if (this->m_rightMainPanel->Show()) {
                this->bSizer138->Layout();
            }

        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_UPSCALER: {
            if (this->m_promptAndFluxPanel->Hide()) {
                this->bSizer138->Layout();
            }
            if (this->m_rightMainPanel->Show()) {
                this->bSizer138->Layout();
            }
            if (this->m_fluxPanel->Hide()) {
                this->m_fluxPanel->GetParent()->GetSizer()->Layout();
            }

        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG: {
            this->m_prompt->Hide();
            this->m_neg_prompt->Hide();
            this->m_prompt2->Show();
            this->m_neg_prompt2->Show();
            if (this->m_fluxPanel->Show()) {
                this->m_fluxPanel->GetParent()->GetSizer()->Layout();
            }

            if (this->m_promptAndFluxPanel->Show()) {
                this->bSizer138->Layout();
            }
            if (this->m_rightMainPanel->Show()) {
                this->bSizer138->Layout();
            }

        } break;
        default: {
            if (this->m_promptAndFluxPanel->Hide()) {
                this->bSizer138->Layout();
            }
            if (this->m_rightMainPanel->Hide()) {
                this->bSizer138->Layout();
            }

        } break;
    }

    this->m_promptPanel->GetSizer()->Layout();
    this->m_fluxPanel->GetSizer()->Layout();
    this->m_rightMainPanel->GetSizer()->Layout();
    this->m_rightMainPanel->Layout();
    this->m_rightMainPanel->Refresh();

    if (selected == sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG)  // on img2img and img2vid the vae_decode_only is false, otherwise true
    {
        this->m_vae_decode_only->SetValue(false);
    } else {
        this->m_vae_decode_only->SetValue(true);
    }

    if (selected == sd_gui_utils::GuiMainPanels::PANEL_QUEUE ||
        selected == sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG ||
        selected == sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG ||
        selected == sd_gui_utils::GuiMainPanels::PANEL_MODELS ||
        selected == sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO) {
        if (this->m_model->GetCount() > 1) {
            this->m_model->Enable();
        }

        this->m_type->Enable();

        if (this->m_vae->GetCount() > 1) {
            this->m_vae->Enable();
        }
        this->m_vae_tiling->Enable();
        this->m_cfg->Enable();
        this->m_clip_skip->Enable();

        if (this->m_taesd->GetCount() > 0) {
            this->m_taesd->Enable();
        }
        // img2img can handle multiple images
        // if (selected == sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG)
        // to only 1 image...
        // {
        //     this->m_batch_count->Disable();
        //     this->m_batch_count->SetValue(1);
        /// } else {
        //     this->m_batch_count->Enable();
        //     this->m_batch_count->SetToolTip("");
        // }
        this->m_batch_count->Enable();
        this->m_sampler->Enable();
        this->m_scheduler->Enable();

        this->m_preset_list->Enable();
        this->m_save_preset->Enable();

        this->m_seed->Enable();
        this->m_steps->Enable();

        if (selected == sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG &&
            !this->m_controlnetImageOpen->GetPath().empty() &&
            !wxFileName(this->m_controlnetImageOpen->GetPath()).IsDir()) {
            this->m_width->Disable();
            this->m_height->Disable();
            this->m_button7->Disable();  // swap button
        } else {
            this->m_width->Enable();
            this->m_height->Enable();
            this->m_button7->Enable();
        }
        // img2img
        if (selected == sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG && !this->m_img2imgOpen->GetPath().empty()) {
            if (!wxFileName(this->m_img2imgOpen->GetPath()).IsDir()) {
                // this->m_width->Disable();
                // this->m_height->Disable();
                // this->m_button7->Disable();  // swap button
            }
        }
        if (selected == sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG || selected == sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO) {
            if (this->m_filePickerDiffusionModel->GetPath().empty() == false && wxFileName(this->m_filePickerDiffusionModel->GetPath()).IsDir() == false) {
                this->m_model->Disable();
                this->m_model->SetSelection(0);
                this->m_controlnetModels->Select(0);
                this->m_controlnetModels->Disable();
                this->m_controlnetImageDelete->Disable();
                this->m_controlnetImageOpen->Disable();
                this->m_controlnetStrength->Disable();
                auto origSize = this->m_controlnetImagePreview->GetSize();
                this->m_controlnetImagePreview->SetBitmap(this->ControlnetOrigPreviewBitmap);
                this->m_controlnetImagePreview->SetSize(origSize);
            }
        }
    }
    // upscaler
    if (selected == sd_gui_utils::GuiMainPanels::PANEL_UPSCALER) {
        this->m_model->Disable();
        this->m_type->Disable();
        this->m_vae->Disable();
        this->m_vae_tiling->Disable();
        this->m_cfg->Disable();
        this->m_clip_skip->Disable();
        this->m_taesd->Disable();
        this->m_batch_count->Disable();
        this->m_sampler->Disable();
        this->m_scheduler->Disable();
        this->m_preset_list->Disable();
        this->m_save_preset->Disable();
        this->m_load_preset->Disable();
        this->m_preset_list->Select(0);
        this->m_seed->Disable();
        this->m_steps->Disable();
        this->m_width->Disable();
        this->m_height->Disable();
        this->m_button7->Disable();  // swap button
    }

    this->CheckQueueButton();
}

void MainWindowUI::OnJobListItemActivated(wxDataViewEvent& event) {
    // implemented in OnJobListItemSelection
}

void MainWindowUI::OnJobListItemKeyDown(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_DELETE)  // Delete
    {
        wxDataViewListCtrl* dataView = static_cast<wxDataViewListCtrl*>(event.GetEventObject());
        wxDataViewItemArray itemsToDelete;
        dataView->GetSelections(itemsToDelete);

        auto list = dataViewListManager->GetSelectedItemsId(itemsToDelete);

        for (auto id : list) {
            auto jobitem = this->qmanager->GetItemPtr(id);
            if (jobitem) {
                if ((jobitem->status & QueueStatusFlags::DELETABLE_FLAG) == false) {
                    continue;
                }
                if (jobitem->server.empty()) {
                    this->dataViewListManager->DeleteItem(id);
                } else {
                    auto srv = this->mapp->cfg->GetTcpServer(jobitem->server);
                    if (srv) {
                        srv->DeleteJob(id);
                    }
                }
            }
        }

        // dataView->GetSelections(itemsToDelete);
        // this->dataViewListManager->DeleteItems(itemsToDelete);
        this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
    }
    event.Skip();
}

/**
 * Handles the context menu event for the job and data model lists.
 *
 * This function determines the source of the event (either the job list or the
 * data model list) and constructs a context menu based on the selected item(s).
 * The menu provides options to perform various actions on the selected item,
 * such as requeuing, loading parameters, copying prompts, model selection,
 * hash calculation, and more. The availability of menu options depends on the
 * item's properties and status.
 *
 * @param event The data view event containing information about the source and
 *              selected item.
 */
void MainWindowUI::onContextMenu(wxDataViewEvent& event) {
    auto* source = (wxDataViewListCtrl*)event.GetEventObject();
    if (source->GetItemCount() == 0) {
        return;
    }

    if (source == this->m_joblist) {
        if (this->m_joblist->GetSelectedItemsCount() == 0) {
            return;
        }
        wxMenu* menu = new wxMenu();
        if (this->m_joblist->GetSelectedItemsCount() > 1) {
            wxDataViewItemArray selections;
            this->m_joblist->GetSelections(selections);
            std::vector<uint64_t> items_to_del;
            std::vector<wxDataViewItem> rows_to_del;
            for (const auto item : selections) {
                auto qitem = this->dataViewListManager->GetQueueItem(item);
                if (qitem == nullptr) {
                    continue;
                }
                if ((qitem->status & QueueStatusFlags::DELETABLE_FLAG) == false) {
                    continue;
                }
                rows_to_del.emplace_back(item);
                items_to_del.emplace_back(qitem->id);
            }
            if (rows_to_del.size() != items_to_del.size()) {
                this->writeLog(_("Error: rows_to_del.size() != items_to_del.size()"));
                items_to_del.clear();
                rows_to_del.clear();
            }
            if (items_to_del.size() > 0) {
                menu->Append(99, wxString::Format(_("Delete %d items"), (int)rows_to_del.size()));
                // menu->Enable(99, false);
                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, rows_to_del, items_to_del](wxCommandEvent& event) {
                    // unselect all before delete
                    this->m_joblist->SetSelections(wxDataViewItemArray());
                    for (auto id : items_to_del) {
                        auto jobitem = this->qmanager->GetItemPtr(id);
                        if (jobitem) {
                            if ((jobitem->status & QueueStatusFlags::DELETABLE_FLAG) == false) {
                                continue;
                            }
                            if (jobitem->server.empty()) {
                                this->dataViewListManager->DeleteItem(id);
                            } else {
                                auto srv = this->mapp->cfg->GetTcpServer(jobitem->server);
                                if (srv && srv->IsConnected()) {
                                    srv->DeleteJob(id);
                                }
                            }
                        }
                    }
                    // this->dataViewListManager->DeleteItems(rows_to_del);
                    this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
                    event.Skip();
                });
            }

        } else {
            auto item  = event.GetItem();
            auto qitem = this->dataViewListManager->GetQueueItem(item);
            if (qitem == nullptr) {
                delete menu;
                return;
            }
            wxMenuItem* requeueItem = new wxMenuItem(menu, wxID_DUPLICATE, _("Requeue"));
            requeueItem->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU));
            menu->Append(requeueItem);
            menu->Enable(wxID_DUPLICATE, false);

            if (qitem->status & QueueStatusFlags::REQUEUEABLE_FLAG) {
                menu->Enable(wxID_DUPLICATE, true);
            }
            if (qitem->mode != SDMode::CONVERT) {
                if (qitem->mode == SDMode::IMG2IMG ||
                    qitem->mode == SDMode::UPSCALE) {
                    if (!std::filesystem::exists(qitem->initial_image)) {
                        menu->Enable(wxID_DUPLICATE, false);
                    }
                }
            } else {
                menu->Enable(wxID_DUPLICATE, false);
            }

            if (qitem->params.controlnet_path.length() > 0 &&
                qitem->params.control_image_path.length() > 0 &&
                !std::filesystem::exists(qitem->params.control_image_path)) {
                menu->Enable(wxID_DUPLICATE, false);
            }

            if (qitem->mode != SDMode::UPSCALE &&
                qitem->mode != SDMode::CONVERT) {
                wxMenuItem* loadParamsItem = new wxMenuItem(menu, 2, _("Load parameters"));
                loadParamsItem->SetBitmap(wxArtProvider::GetBitmap(wxART_PASTE, wxART_MENU));

                menu->Append(loadParamsItem);
                menu->Append(3, _("Copy prompts to text2img"));
                menu->Append(4, _("Copy prompts to img2img"));
                menu->Append(5, wxString::Format(_("Select model %s"), qitem->model));

                if (qitem->image_info.size() > 0) {
                    menu->Append(6, _("Send the last image to the Upscale tab"));
                    menu->Append(7, _("Send the last image to the img2img tab"));
                }
            }
            if (qitem->mode == SDMode::UPSCALE) {
                if (qitem->image_info.size() > 0) {
                    menu->Append(6, wxString::Format(_("Upscale again")));
                }
            }

            if (qitem->status & QueueStatusFlags::PAUSABLE_FLAG) {
                menu->AppendSeparator();
                menu->Append(8, _("Pause"));
            } else if (qitem->status & QueueStatusFlags::RESUMEABLE_FLAG) {
                menu->AppendSeparator();
                menu->Append(8, _("Resume"));
            }

            menu->AppendSeparator();
            menu->Append(wxID_DELETE);
            menu->Enable(wxID_DELETE, false);

            if (qitem->status & QueueStatusFlags::DELETABLE_FLAG) {
                menu->Enable(wxID_DELETE, true);
            }
            menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainWindowUI::OnPopupClick, this);
        }
        PopupMenu(menu);
        delete menu;
    }
}

void MainWindowUI::OnDataModelTreeContextMenu(wxTreeListEvent& event) {
    wxTreeListItems selections;
    if (this->m_modelTreeList->GetSelections(selections) == 0) {
        return;
    }
    // TODO: implement multiple model handling
    auto item = selections.front();  // get the first
    if (item.IsOk() == false) {
        return;
    }
    auto modelInfo = this->modelUiManager->FindItem(item);

    wxMenu* menu = new wxMenu();

    if (modelInfo == nullptr) {  // this is a root item
        wxString name       = this->m_modelTreeList->GetItemText(item);
        wxString parentName = wxEmptyString;
        auto parentItem     = this->m_modelTreeList->GetItemParent(item);
        if (parentItem.IsOk()) {
            while (parentItem.IsOk()) {
                parentName = this->m_modelTreeList->GetItemText(parentItem);

                if (parentName.IsEmpty()) {
                    break;
                }
                name.Prepend(parentName + '/');  // always use unix separator
                parentItem = this->m_modelTreeList->GetItemParent(parentItem);
            }
            name.Replace("//", "/");
            menu->Append(106, wxString::Format(_("Create subfolder in: '%s'"), name));
            menu->Enable(106, false);
        }
    } else {
        if (modelInfo->hash_progress_size == 0 && modelInfo->server_id.empty()) {
            if (!modelInfo->sha256.empty()) {
                menu->Append(100, _("RE-Calculate &Hash"));
            } else {
                menu->Append(100, _("Calculate &Hash"));
            }
        }

        if (this->mapp->cfg->enable_civitai == true && modelInfo->hash_progress_size == 0) {
            if (!modelInfo->civitaiPlainJson.empty()) {
                menu->Append(105, _("Force update info from &CivitAi"));
            } else {
                menu->Append(105, _("Update info from &CivitAi"));
            }
        }

        if (modelInfo->model_type == sd_gui_utils::DirTypes::CHECKPOINT) {
            menu->Append(200, wxString::Format(_("&Select model %s to the next job"), modelInfo->name));
            if (modelInfo->state == sd_gui_utils::CivitAiState::OK && this->mapp->cfg->enable_civitai == true) {
                menu->Append(199, _("Open model on CivitAi.com in default browser"));
            }
        }
        // allow to convert all model files to gguf TODO: allow remote models to be converted
        if (modelInfo->name.find(".safetensors") && modelInfo->server_id.empty()) {
            menu->AppendSeparator();
            menu->Append(201, wxString::Format(_("Convert model to %s gguf format"), this->m_type->GetStringSelection()));
            if (modelInfo->hash_progress_size > 0) {
                menu->Enable(201, false);
            }
        }

        if (modelInfo->model_type == sd_gui_utils::DirTypes::LORA) {
            auto shortname = wxFileName(modelInfo->path).GetName();
            menu->Append(101, wxString::Format(_("Append to text2img prompt <lora:%s:0.5>"), shortname));
            menu->Append(102, wxString::Format(_("Append to text2img neg. prompt <lora:%s:0.5>"), shortname));
            menu->Append(103, wxString::Format(_("Append to img2img prompt <lora:%s:0.5>"), shortname));
            menu->Append(104, wxString::Format(_("Append to img2img neg. prompt <lora:%s:0.5>"), shortname));
        }
        if (modelInfo->model_type == sd_gui_utils::DirTypes::EMBEDDING) {
            auto shortname = wxFileName(modelInfo->path).GetName();
            menu->Append(111, wxString::Format(_("Append to text2img prompt %s"), shortname));
            menu->Append(112, wxString::Format(_("Append to text2img neg. prompt %s"), shortname));
            menu->Append(113, wxString::Format(_("Append to img2img prompt %s"), shortname));
            menu->Append(114, wxString::Format(_("Append to img2img neg. prompt %s"), shortname));

            // large embeddings not supported
            menu->Enable(111, false);
            menu->Enable(112, false);
            menu->Enable(113, false);
            menu->Enable(114, false);
        }

        if (modelInfo->hash_fullsize == 0) {
            menu->AppendSeparator();
            menu->Append(300, _("&Move model into another sub folder"));
            menu->Enable(300, false);
            // submenu for named paths
            wxMenu* submenu = new wxMenu();
            submenu->Append(302, "Checkpoints", wxString::Format(_("Move the model into the checkoints folder: %s"), this->mapp->cfg->model));
            submenu->Append(303, "VAE", wxString::Format(_("Move the model into the VAE folder: %s"), this->mapp->cfg->vae));
            submenu->Append(304, "Lora", wxString::Format(_("Move the model into the Lora folder: %s"), this->mapp->cfg->lora));
            submenu->Append(305, "Embedding", wxString::Format(_("Move the model into the Embedding folder: %s"), this->mapp->cfg->embedding));
            submenu->Append(306, "TaeSD", wxString::Format(_("Move the model into the TaeSD folder: %s"), this->mapp->cfg->taesd));
            submenu->Append(307, "ESRGAN", wxString::Format(_("Move the model into the ESRGAN (upscalers) folder: %s"), this->mapp->cfg->esrgan));
            submenu->Append(308, "ControlNet", wxString::Format(_("Move the model into the ControlNet folder: %s"), this->mapp->cfg->controlnet));
            submenu->Enable(302, false);
            submenu->Enable(303, false);
            submenu->Enable(304, false);
            submenu->Enable(305, false);
            submenu->Enable(306, false);
            submenu->Enable(307, false);
            submenu->Enable(308, false);
            menu->AppendSubMenu(submenu, _("&Move to another folder"));

            menu->Append(310, _("&Delete model file"));
            if (modelInfo->server_id.empty() == false) {
                menu->Enable(310, false);
            } else {
                for (const auto& qitem : this->qmanager->getList()) {
                    if (qitem.second->params.model_path == modelInfo->path &&
                        (qitem.second->status & QueueStatusFlags::DELETABLE_FLAG) == false) {
                        menu->Enable(310, false);
                        break;
                    }
                }
            }
        }
        if (modelInfo->server_id.empty() == true) {
            menu->Append(311, _("Open folder"));
        }
    }

    menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainWindowUI::OnModelListPopUpClick, this);
    PopupMenu(menu);
    delete menu;
}

void MainWindowUI::OnDataModelTreeColSorted(wxTreeListEvent& event) {
}

void MainWindowUI::OnDataModelTreeExpanded(wxTreeListEvent& event) {
    // auto item = event.GetItem();
}

void MainWindowUI::OnJobListItemSelection(wxDataViewEvent& event) {
    if (this->m_joblist->GetSelectedItemsCount() == 0) {
        return;
    }

    auto row = this->m_joblist->GetSelectedRow();
    if (row == wxNOT_FOUND) {
        return;
    }

    auto qitem = this->dataViewListManager->GetQueueItem(row);

    if (qitem != nullptr) {
        this->UpdateJobInfoDetailsFromJobQueueList(qitem);
    }
}

void MainWindowUI::onTxt2ImgFileDrop(wxDropFilesEvent& event) {
    // only just one file.. sry...
    auto files = event.GetFiles();

    auto file = files[0];
    auto path = wxFileName(file);

    if (path.GetExt() != "jpg" && path.GetExt() != "png") {
        wxMessageBox(wxString::Format(_("Image type not supported: %s"), path.GetExt()));
        return;
    }

    wxImage img;
    if (!img.LoadFile(path.GetFullPath())) {
        wxMessageBox(wxString::Format(_("Image type not supported: %s"), path.GetFullPath()));
        // invalid img
        return;
    }

    this->readMetaDataFromImage(file, SDMode::TXT2IMG, true);
}

void MainWindowUI::readMetaDataFromImage(const wxFileName& file, const SDMode mode, bool askForLoadParameters) {
    auto getParams = this->getMetaDataFromImage(file);
    if (getParams.size() > 0 && askForLoadParameters == true) {
        wxMessageDialog dialog(this, _("Load generation parameters from image?"), _("Load parameters?"), wxYES_NO | wxICON_QUESTION);
        if (dialog.ShowModal() == wxID_YES) {
            this->imageCommentToGuiParams(getParams, mode);
            return;
        }
        return;
    }
    this->imageCommentToGuiParams(getParams, mode);
}
std::unordered_map<wxString, wxString> MainWindowUI::getMetaDataFromImage(const wxFileName& file) {
    std::unordered_map<wxString, wxString> results;

    try {
        auto image = Exiv2::ImageFactory::open(file.GetAbsolutePath().utf8_string());
        if (!image->good()) {
            return std::unordered_map<wxString, wxString>{};
        }
        image->readMetadata();

        Exiv2::ExifData& exifData = image->exifData();
        if (!exifData.empty()) {
            std::string ex;
            Exiv2::ExifData::iterator it;
            for (it = exifData.begin(); it != exifData.end(); ++it) {
                std::string key   = it->key();
                std::string value = it->getValue()->toString();

                wxString wxKey   = wxString::FromUTF8(key.c_str());
                wxString wxValue = wxString::FromUTF8Unchecked(value.c_str(), value.size());

                if (wxKey == "Exif.Photo.UserComment" || wxKey == "Exif.Image.UserComment" || wxKey == "Exif.Photo.Parameters") {
                    auto comment    = dynamic_cast<const Exiv2::CommentValue*>(&it->value());
                    auto commentStr = comment->comment("UTF-8");
                    commentStr.erase(std::remove(commentStr.begin(), commentStr.end(), '\0'), commentStr.end());
                    wxValue = wxString::FromAscii(commentStr.c_str(), commentStr.size());
                    if (!wxValue.empty()) {
                        results = sd_gui_utils::parseExifPrompts(wxValue);
                        break;
                    }
                }
            }
        }
    } catch (Exiv2::Error& e) {
        std::cerr << "Err: " << e.what() << std::endl;
    }

    return results;
}
/// TODO: store embeddings like checkpoints and loras, the finetune this
/*void MainWindowUI::OnPromptText(wxCommandEvent& event) {
    event.Skip();  // disabled while really slow
    auto obj    = static_cast<wxTextCtrl*>(event.GetEventObject());
    auto tokens = MainWindowUI::CountTokens(obj->GetValue());
    obj->SetToolTip(wxString::Format(_("Tokens: %d"), tokens));
    return;
}*/

/*void MainWindowUI::OnNegPromptText(wxCommandEvent& event) {
    event.Skip();  // disabled while really slow
    auto obj    = static_cast<wxTextCtrl*>(event.GetEventObject());
    auto tokens = MainWindowUI::CountTokens(obj->GetValue());
    obj->SetToolTip(wxString::Format(_("Tokens: %d"), tokens));
    return;
}*/
void MainWindowUI::OnTextChanged(wxStyledTextEvent& event) {
    wxStyledTextCtrl* stc = (wxStyledTextCtrl*)event.GetEventObject();
    if (!stc) {
        event.Skip();
        return;
    }
    this->HighLightPrompts(stc);

    if (this->m_autoCompJustCompleted) {
        this->m_autoCompJustCompleted = false;
        event.Skip();
        return;
    }

    if (event.GetModificationType() != 20) {
        return;
    }

    int currentPos   = stc->GetCurrentPos();
    int wordStartPos = stc->WordStartPosition(currentPos, true);
    int lenEntered   = currentPos - wordStartPos;
    if (lenEntered <= 0) {
        return;
    }

    wxString loraSearchText;
    size_t loraPos = wxNOT_FOUND;

    loraPos = MainWindowUI::findLoraDistance(stc->GetText(), stc->GetCurrentPos() + 1);
    if (loraPos != wxNOT_FOUND) {
        loraSearchText = stc->GetTextRange(loraPos, stc->GetCurrentPos() + 1);
    }
    wxString enteredText = stc->GetTextRange(wordStartPos, currentPos + 1);

    if (enteredText.EndsWith("\n")) {
        return;
    }
    if (enteredText.Trim().Length() >= 3 || (loraPos != wxNOT_FOUND)) {
        wxString list;

        if (loraPos != wxNOT_FOUND) {
            auto loras = this->ModelManager->ListModelNamesByType(sd_gui_utils::DirTypes::LORA, loraSearchText);

            for (const auto& lora_info : loras) {
                list.Append(wxString::Format("%s:0.5>\r", lora_info.second));
            }
        } else {
            auto embeddings = this->ModelManager->ListModelNamesByType(sd_gui_utils::DirTypes::EMBEDDING, enteredText);
            for (const auto& embedding_info : embeddings) {
                list.Append(wxString::Format("%s \r", embedding_info.second));
            }
        }

        if (!list.empty()) {
            stc->AutoCompSetIgnoreCase(true);
            stc->AutoCompSetSeparator('\r');
            // stc->AutoCompSetTypeSeparator('\n');
            stc->AutoCompSetMaxHeight(4);
            stc->AutoCompSetAutoHide(false);
            stc->AutoCompShow(lenEntered, list);
        }
    }
    event.Skip();
}
void MainWindowUI::OnAutoCompSelection(wxStyledTextEvent& event) {
    event.Skip();
    return;
}

void MainWindowUI::HighLightPrompts(wxStyledTextCtrl* stc) {
    // loras
    wxString text = stc->GetText();
    int length    = text.Length();

    stc->StartStyling(0);
    stc->SetStyling(length, wxSTC_STYLE_DEFAULT);

    int pos = 0;
    while ((pos = text.find("<lora:", pos)) != wxNOT_FOUND) {
        int endPos = text.find(">", pos);
        if (endPos == wxNOT_FOUND)
            break;

        size_t colonPos = text.find(":", pos);
        if (colonPos == std::string::npos)
            break;

        size_t colonPos2 = text.find(":", colonPos + 1);
        if (colonPos2 == std::string::npos)
            break;

        const auto name = text.substr(colonPos + 1, colonPos2 - colonPos - 1);

        // get the current tcp srv
        std::string server_id;
        if (this->m_server->GetSelection() > 0 && this->m_server->GetSelection() != wxNOT_FOUND) {
            auto server = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(this->m_server->GetSelection()));
            if (server) {
                server_id = server->GetId();
            }
        }

        if (this->ModelManager->NameStartsWith(name, sd_gui_utils::DirTypes::LORA, server_id) == nullptr) {
            int tagLength = endPos - pos + 1;
            stc->StartStyling(pos);
            stc->SetStyling(tagLength, +sd_gui_utils::GuiPromptStyles::STYLE_LORA_NOT_FOUND);

            pos = endPos + 1;
            continue;
        }

        int tagLength = endPos - pos + 1;
        stc->StartStyling(pos);
        stc->SetStyling(tagLength, +sd_gui_utils::GuiPromptStyles::STYLE_LORA);

        pos = endPos + 1;
    }
    // loras
    // embeddings
    std::map<std::string, std::string> embeddings = this->ModelManager->ListModelNamesByType(sd_gui_utils::DirTypes::EMBEDDING);

    for (const auto& pair : embeddings) {
        int pos = 0;
        while ((pos = text.find(pair.first, pos)) != wxNOT_FOUND) {
            int endPos = pos + pair.first.length();
            if (endPos == wxNOT_FOUND) {
                break;
            }
            stc->StartStyling(pos);
            stc->SetStyling(endPos - pos, +sd_gui_utils::GuiPromptStyles::STYLE_EMBEDDING);
            pos = endPos;
        }
        pos = 0;
        while ((pos = text.find(pair.second, pos)) != wxNOT_FOUND) {
            int endPos = pos + pair.second.length();
            if (endPos == wxNOT_FOUND) {
                break;
            }
            stc->StartStyling(pos);
            stc->SetStyling(endPos - pos, +sd_gui_utils::GuiPromptStyles::STYLE_EMBEDDING);
            pos = endPos;
        }
    }
    // embeddings
    // highlight the word: BREAK
    pos = 0;
    while ((pos = text.find("BREAK", pos)) != wxNOT_FOUND) {
        int endPos = pos + 5;
        if (endPos == wxNOT_FOUND) {
            break;
        }
        stc->StartStyling(pos);
        stc->SetStyling(endPos - pos, +sd_gui_utils::GuiPromptStyles::STYLE_OTHERS);
        pos = endPos;
    }
    // highlight the word: BREAK
}

void MainWindowUI::OnPromptKeyDown(wxKeyEvent& event) {
    int keyCode = event.GetKeyCode();

    if (keyCode == WXK_TAB) {
        return;
    }

    if (!event.ControlDown()) {
        event.Skip();
        return;
    }
    if (keyCode == WXK_RETURN) {
        this->CheckQueueButton();
        if (this->m_queue->IsEnabled()) {
            wxCommandEvent clickEvent(wxEVT_BUTTON, this->m_queue->GetId());
            wxPostEvent(this->m_queue, clickEvent);
            return;
        }
    }
    if (keyCode != WXK_UP && keyCode != WXK_DOWN) {
        event.Skip();
        return;
    }

    auto obj = static_cast<wxStyledTextCtrl*>(event.GetEventObject());
    if (!obj) {
        event.Skip();
        return;
    }

    long from, to;
    obj->GetSelection(&from, &to);
    wxString text = obj->GetValue();

    if (from == to) {
        from = obj->GetInsertionPoint();
        to   = obj->GetInsertionPoint();

        long bracketStart = from, bracketEnd = from;
        while (bracketStart > 0 && text[bracketStart - 1] != '(') {
            bracketStart--;
        }
        while (bracketEnd < (long)text.Length() && text[bracketEnd] != ')') {
            bracketEnd++;
        }

        if (bracketStart > 0 && bracketEnd < (long)text.Length() && text[bracketStart - 1] == '(' && text[bracketEnd] == ')') {
            from = bracketStart - 1;
            to   = bracketEnd + 1;
        } else {
            // Ha nincs zárójeles rész, csak a szót választjuk ki
            while (from > 0 && wxIsalnum(text[from - 1])) {
                from--;
            }
            while (to < (long)text.Length() && wxIsalnum(text[to])) {
                to++;
            }
        }
    }

    if (from == to) {
        event.Skip();
        return;
    }

    wxString selectedText = text.Mid(from, to - from);
    wxRegEx regex(R"(\(([^:()]+):([0-9]+)\.([0-9]+)\))");

    wxString newText;
    if (regex.Matches(selectedText)) {
        // Ha már van formázás, módosítjuk az értékét
        wxString base = regex.GetMatch(selectedText, 1);
        long intPart, decPart;
        regex.GetMatch(selectedText, 2).ToLong(&intPart);
        regex.GetMatch(selectedText, 3).ToLong(&decPart);

        if (keyCode == WXK_UP) {
            if (intPart == 0 && decPart == 9) {
                newText = base;
            } else if (++decPart > 9) {
                decPart = 0;
                intPart++;
            }
        } else {  // WXK_DOWN
            if (intPart == 1 && decPart == 1) {
                newText = base;  // Visszaáll az eredeti szóra
            } else if (intPart > 0 || decPart > 0) {
                if (--decPart < 0) {
                    decPart = 9;
                    intPart--;
                }
            }
        }

        if (newText.IsEmpty()) {
            newText = wxString::Format("(%s:%ld.%ld)", base, intPart, decPart);
        }
    } else {
        wxRegEx alreadyFormatted(R"(^\(([^:()]+)\)$)");
        if (alreadyFormatted.Matches(selectedText)) {
            selectedText = alreadyFormatted.GetMatch(selectedText, 1);
        }

        if (keyCode == WXK_UP) {
            newText = wxString::Format("(%s:1.1)", selectedText);
        } else {
            newText = wxString::Format("(%s:0.9)", selectedText);
        }
    }

    obj->ChangeValue(text.Mid(0, from) + newText + text.Mid(to));
    obj->SetSelection(from, from + newText.Length());
}
void MainWindowUI::InitPrompts() {
    std::vector<wxStyledTextCtrl*> textCtrls = {m_prompt, m_prompt2, m_neg_prompt, m_neg_prompt2};

    for (auto* textCtrl : textCtrls) {
        sd_gui_utils::ConfigureTextCtrl(textCtrl);
        textCtrl->SetKeyWords(0, wxT("BREAK"));
        textCtrl->Bind(wxEVT_STC_MODIFIED, &MainWindowUI::OnTextChanged, this);
        textCtrl->Bind(wxEVT_STC_AUTOCOMP_SELECTION, &MainWindowUI::OnAutoCompSelection, this);
    }
}

void MainWindowUI::onGenerate(wxCommandEvent& event) {
    this->m_queue->Disable();
    sd_gui_utils::ModelFileInfo* modelinfo = nullptr;
    wxArrayString errorlist;

    this->CheckQueueButton();

    auto type  = SDMode::TXT2IMG;
    int pageId = this->m_notebook1302->GetSelection();
    switch (pageId) {
        case 1:
            type = SDMode::TXT2IMG;
            break;
        case 2:
            type = SDMode::IMG2IMG;
            break;
        case 3:
            type = SDMode::UPSCALE;
            break;
        default:
            this->m_queue->Enable();
            return;
            break;
    }

    if (this->m_scheduler->GetSelection() == wxNOT_FOUND || this->m_scheduler->GetStringSelection().empty()) {
        this->m_scheduler->SetSelection(0);
    }
    if (this->m_sampler->GetSelection() == wxNOT_FOUND || this->m_sampler->GetStringSelection().empty()) {
        this->m_sampler->SetSelection(0);
    }

    auto wClientData  = dynamic_cast<sd_gui_utils::IntClientData*>(this->m_type->GetClientObject(this->m_type->GetSelection()));
    auto sClientData  = dynamic_cast<sd_gui_utils::IntClientData*>(this->m_scheduler->GetClientObject(this->m_scheduler->GetSelection()));
    auto smClientData = dynamic_cast<sd_gui_utils::IntClientData*>(this->m_sampler->GetClientObject(this->m_sampler->GetSelection()));

    if (type == SDMode::UPSCALE) {
        auto esrganModel = static_cast<sd_gui_utils::ModelFileInfo*>(this->m_upscaler_model->GetClientData(this->m_upscaler_model->GetSelection()));
        if (esrganModel == nullptr) {
            wxLogError(_("No upscaler model found!"));
            this->m_queue->Enable();
            return;
        }
        wxString esrganPath = esrganModel->path;
        wxString modelName  = esrganModel->path;

        std::shared_ptr<QueueItem> item = std::make_shared<QueueItem>();

        if (this->m_server->GetSelection() > 0) {
            int id = this->m_server->GetSelection();
            if (id != wxNOT_FOUND) {
                auto server = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(id));
                if (server != nullptr) {
                    if (server->IsConnected() == true) {
                        item->server = server->GetId();
                        modelName.Replace(item->server + ":", "");
                        esrganPath = esrganModel->sha256;
                    } else {
                        errorlist.Add(wxString::Format(_("The selected server (%s) not connected"), server->GetName()));
                    }
                }
            }
        }

        if (!esrganModel->sha256.empty()) {
            item->hashes.esrgan_hash = esrganModel->sha256;
        }
        item->model              = wxFileName(modelName).GetFullName();
        item->mode               = type;
        item->params.esrgan_path = esrganPath;

        item->initial_image             = this->m_upscaler_filepicker->GetPath();
        item->params.mode               = SDMode::UPSCALE;
        item->params.n_threads          = this->mapp->cfg->n_threads;
        item->keep_checkpoint_in_memory = this->m_keep_other_models_in_memory->GetValue();
        item->keep_upscaler_in_memory   = this->m_keep_upscaler_in_memory->GetValue();

        item->params.wtype    = static_cast<sd_type_t>(wClientData->getId());
        item->params.schedule = static_cast<schedule_t>(sClientData->getId());

        wxImage initialImage;
        if (initialImage.LoadFile(item->initial_image)) {
            sd_gui_utils::networks::ImageInfo info(sd_gui_utils::networks::ImageType::INITIAL | sd_gui_utils::networks::ImageType::COPYABLE, item->initial_image);
            info.width    = initialImage.GetWidth();
            info.height   = initialImage.GetHeight();
            info.md5_hash = sd_gui_utils::calculateMD5(item->initial_image);
            item->image_info.emplace_back(info);
        }

        if (errorlist.size() > 0) {
            wxString error_strs;
            for (const auto str : errorlist) {
                error_strs.Append(wxGetTranslation(str));
                error_strs.Append("\n");
            }
            wxMessageDialog dialog(this, error_strs, _("Error"), wxOK | wxICON_ERROR);
            dialog.ShowModal();
            this->m_queue->Enable();
            return;
        }
        this->qmanager->AddItem(item);
        this->m_queue->Enable();
        return;
    }

    std::shared_ptr<QueueItem> item = std::make_shared<QueueItem>();

    item->params.wtype         = static_cast<sd_type_t>(wClientData->getId());
    item->params.schedule      = static_cast<schedule_t>(sClientData->getId());
    item->params.sample_method = static_cast<sample_method_t>(smClientData->getId());

    if (this->m_server->GetSelection() > 0) {
        int id = this->m_server->GetSelection();
        if (id != wxNOT_FOUND) {
            auto server = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(id));
            if (server != nullptr) {
                if (server->IsConnected()) {
                    item->server = server->GetId();
                } else {
                    errorlist.Add(wxString::Format(_("The selected server (%s) not connected"), server->GetName()));
                }
            }
        }
    }

    auto diffusionModel = this->m_filePickerDiffusionModel->GetPath().utf8_string();

    if (diffusionModel.empty() == false) {
        item->params.diffusion_model_path = diffusionModel;
        item->model                       = wxFileName(item->params.diffusion_model_path).GetFullName();
    } else {
        auto mindex = this->m_model->GetCurrentSelection();
        auto data   = this->m_model->GetClientData(mindex);
        modelinfo   = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(data);
        if (modelinfo == nullptr) {
            errorlist.Add(_("No model found!"));
        } else {
            item->model = modelinfo->name;
            if (item->server.empty()) {
                if (modelinfo->server_id.empty()) {
                    item->params.model_path = modelinfo->path;
                } else {
                    errorlist.Add(_("Please select a local model!"));
                }

            } else {
                if (modelinfo->server_id != item->server) {
                    errorlist.Add(_("The selected model not available on the selected server!"));
                } else {
                    item->params.model_path = modelinfo->sha256;
                }
            }
            if (!modelinfo->sha256.empty()) {
                item->hashes.model_hash = modelinfo->sha256;
            }
        }
    }

    if (this->m_taesd->GetCurrentSelection() > 0) {
        auto taesdModel = static_cast<sd_gui_utils::ModelFileInfo*>(this->m_taesd->GetClientData(this->m_taesd->GetCurrentSelection()));
        if (taesdModel == nullptr) {
            wxLogError(_("No TAESD model found!"));
            this->m_queue->Enable();
            return;
        }
        if (!item->server.empty()) {
            if (taesdModel->server_id != item->server) {
                errorlist.Add(_("The selected taesd model not available on the selected server!"));
            } else {
                item->params.taesd_path = taesdModel->sha256;
            }
        } else {
            item->params.taesd_path = taesdModel->path;
        }
        if (taesdModel && !taesdModel->sha256.empty()) {
            item->hashes.taesd_hash = taesdModel->sha256;
        }
    } else {
        item->params.taesd_path = "";
    }
    if (this->m_vae->GetCurrentSelection() > 0) {
        auto vaeModel = static_cast<sd_gui_utils::ModelFileInfo*>(this->m_vae->GetClientData(this->m_vae->GetCurrentSelection()));
        if (vaeModel == nullptr) {
            wxLogError(_("No VAE model found!"));
            this->m_queue->Enable();
            return;
        }
        if (!item->server.empty()) {
            if (vaeModel->server_id != item->server) {
                errorlist.Add(_("The selected vae model not available on the selected server!"));
            } else {
                item->params.vae_path = vaeModel->sha256;
            }
        } else {
            item->params.vae_path = vaeModel->path;
        }
        if (vaeModel && !vaeModel->sha256.empty()) {
            item->hashes.vae_hash = vaeModel->sha256;
        }

    } else {
        item->params.vae_path = "";
    }

    if (type == SDMode::TXT2IMG || type == SDMode::IMG2IMG) {
        if (diffusionModel.empty() == false) {
            item->model = wxFileName(item->params.diffusion_model_path).GetFullName();
        }

        auto slgscale = this->slgScale->GetValue();

        if (slgscale > 0.f) {
            item->params.slg_scale = slgscale;
        }

        auto gui_skip_layers = this->m_skipLayers->GetValue();

        auto skip_layers = sd_gui_utils::splitStr2int(gui_skip_layers.utf8_string(), ',');

        if (skip_layers != item->params.skip_layers) {
            item->params.skip_layers = skip_layers;
        }

        auto skip_layer_start = this->skipLayerStart->GetValue();
        auto skip_layer_end   = this->skipLayerEnd->GetValue();

        // TODO: there is no better validation for now
        // but we need to check if skip_layer_start is less than skip_layer_end and if skip_layer_end is greater than skip_layer_start etc...
        if (skip_layer_start != item->params.skip_layer_start) {
            item->params.skip_layer_start = skip_layer_start;
        }

        if (skip_layer_end != item->params.skip_layer_end) {
            item->params.skip_layer_end = skip_layer_end;
        }

        // TODO: need more validation, but no model inspection for now so it's ok for now
        auto clipLPath = this->m_filePickerClipL->GetPath();

        if (clipLPath.length() > 0) {
            item->params.clip_l_path = clipLPath.utf8_string();
        }

        auto clipGPath = this->m_filePickerClipG->GetPath();

        if (clipGPath.length() > 0) {
            item->params.clip_g_path = clipGPath.utf8_string();
        }

        auto t5xxlPath = this->m_filePickerT5XXL->GetPath();

        if (t5xxlPath.length() > 0) {
            item->params.t5xxl_path = t5xxlPath.utf8_string();
        }
    }
    if (item->server.empty()) {
        item->params.lora_model_dir  = this->mapp->cfg->lora;
        item->params.embeddings_path = this->mapp->cfg->embedding;
        item->params.n_threads       = this->mapp->cfg->n_threads;
    }

    item->keep_checkpoint_in_memory = this->m_keep_other_models_in_memory->GetValue();
    item->keep_upscaler_in_memory   = this->m_keep_upscaler_in_memory->GetValue();

    item->params.vae_on_cpu           = this->vaeOnCpu->GetValue();
    item->params.clip_on_cpu          = this->clipOnCpu->GetValue();
    item->params.diffusion_flash_attn = this->diffusionFlashAttn->GetValue();

    if (type == SDMode::TXT2IMG) {
        item->params.prompt          = this->m_prompt->GetValue().utf8_string();
        item->params.negative_prompt = this->m_neg_prompt->GetValue().utf8_string();

        if (this->m_controlnetModels->GetSelection() > 0) {
            item->params.control_net_cpu = this->cnOnCpu->GetValue();
        }
    }
    // prompt template into txt2img and img2img too
    if (this->m_promptPresets->GetSelection() > 0) {
        for (const auto& preset : this->PromptTemplates) {
            if (preset.second.name == this->m_promptPresets->GetStringSelection().utf8_string()) {
                if (preset.second.prompt.empty() == false) {
                    wxString _prompt      = wxString::FromUTF8Unchecked(preset.second.prompt);
                    item->original_prompt = item->params.prompt;
                    if (_prompt.Contains("{prompt}")) {
                        _prompt.Replace("{prompt}", wxString::FromUTF8Unchecked(item->params.prompt));
                        item->params.prompt = _prompt.utf8_string();
                    } else {
                        item->params.prompt = item->params.prompt + " " + preset.second.prompt;
                    }
                }

                if (preset.second.negative_prompt.empty() == false) {
                    wxString _neg_prompt           = wxString::FromUTF8Unchecked(preset.second.negative_prompt);
                    item->original_negative_prompt = item->params.negative_prompt;
                    if (_neg_prompt.Contains("{prompt}")) {
                        _neg_prompt.Replace("{prompt}", wxString::FromUTF8Unchecked(item->params.negative_prompt));
                        item->params.negative_prompt = _neg_prompt.utf8_string();
                    } else {
                        item->params.negative_prompt = item->params.negative_prompt + " " + preset.second.negative_prompt;
                    }
                }
            }
        }
    }

    if (type == SDMode::IMG2IMG) {
        item->params.prompt          = this->m_prompt2->GetValue().utf8_string();
        item->params.negative_prompt = this->m_neg_prompt2->GetValue().utf8_string();
        wxImage originalImage        = this->inpaintHelper->GetOriginalImage();
        wxImage initialImage         = this->inpaintHelper->GetOriginalImage();
        wxFileName initialImageName(this->mapp->cfg->tmppath, wxString::Format("initial_image_%" PRIu64 "_%sx%s", this->qmanager->GetNextId(), this->m_width->GetValue(), this->m_height->GetValue()));
        initialImageName.SetExt("png");
        wxFileName originalImageName(this->mapp->cfg->tmppath, wxString::Format("original_image_%" PRIu64 "_%sx%s", this->qmanager->GetNextId(), this->m_width->GetValue(), this->m_height->GetValue()));
        originalImageName.SetExt("png");
        wxFileName maskImagename(this->mapp->cfg->tmppath, wxString::Format("inpaint_mask_image_%" PRIu64 "_%sx%s", this->qmanager->GetNextId(), this->m_width->GetValue(), this->m_height->GetValue()));
        maskImagename.SetExt("png");
        wxFileName outpaintMaskImageName(this->mapp->cfg->tmppath, wxString::Format("outpaint_mask_image_%" PRIu64 "_%sx%s", this->qmanager->GetNextId(), this->m_width->GetValue(), this->m_height->GetValue()));
        outpaintMaskImageName.SetExt("png");

        if (this->inpaintHelper->inPaintImageLoaded() == true) {
            auto maskImage = this->inpaintHelper->GetMaskImage();
            if (this->inpaintHelper->inPaintCanvasEmpty() == false) {
                maskImage.SaveFile(maskImagename.GetAbsolutePath(), wxBITMAP_TYPE_PNG);
                sd_gui_utils::networks::ImageType _type = sd_gui_utils::networks::ImageType::MASK_INPAINT | sd_gui_utils::networks::ImageType::MOVEABLE | sd_gui_utils::networks::ImageType::MASK;
                if (this->inpaintHelper->isOutPainted() == false) {
                    _type |= sd_gui_utils::networks::ImageType::MASK_USED;
                }
                item->image_info.emplace_back(_type, maskImagename.GetAbsolutePath());
                item->image_info.back().width           = maskImage.GetWidth();
                item->image_info.back().height          = maskImage.GetHeight();
                item->image_info.back().md5_hash        = sd_gui_utils::calculateMD5(maskImagename.GetAbsolutePath().ToStdString());
                item->image_info.back().target_filename = maskImagename.GetAbsolutePath();
                item->mask_image                        = maskImagename.GetAbsolutePath();
            }
            if (this->inpaintHelper->isOutPainted()) {
                wxImage outPaintedArea      = originalImage;
                wxImage outPaintedMaskImage = maskImage;
                int canvasTop               = wxAtoi(this->m_inpaintCanvasTop->GetValue());
                int canvasLeft              = wxAtoi(this->m_inpaintCanvasLeft->GetValue());
                int canvasRight             = wxAtoi(this->m_inpaintCanvasRight->GetValue());
                int canvasBottom            = wxAtoi(this->m_inpaintCanvasBottom->GetValue());
                sd_gui_utils::CropOrFillImage(outPaintedArea, canvasTop, canvasRight, canvasBottom, canvasLeft, wxColour(128, 128, 128));
                outPaintedArea.SaveFile(initialImageName.GetAbsolutePath(), wxBITMAP_TYPE_PNG);

                sd_gui_utils::CropOrFillImage(outPaintedMaskImage, canvasTop, canvasRight, canvasBottom, canvasLeft, wxColour(255, 255, 255));
                outPaintedMaskImage.SaveFile(outpaintMaskImageName.GetAbsolutePath(), wxBITMAP_TYPE_PNG);

                originalImage.SaveFile(originalImageName.GetAbsolutePath(), wxBITMAP_TYPE_PNG);

                item->image_info.emplace_back(
                    sd_gui_utils::networks::ImageType::INITIAL | sd_gui_utils::networks::ImageType::COPYABLE,
                    initialImageName.GetAbsolutePath());
                item->image_info.back().width           = initialImage.GetWidth();
                item->image_info.back().height          = initialImage.GetHeight();
                item->image_info.back().md5_hash        = sd_gui_utils::calculateMD5(initialImageName.GetAbsolutePath().ToStdString());
                item->image_info.back().target_filename = initialImageName.GetAbsolutePath();

                item->image_info.emplace_back(
                    sd_gui_utils::networks::ImageType::ORIGINAL | sd_gui_utils::networks::ImageType::COPYABLE,
                    originalImageName.GetAbsolutePath());
                item->image_info.back().width           = originalImage.GetWidth();
                item->image_info.back().height          = originalImage.GetHeight();
                item->image_info.back().md5_hash        = sd_gui_utils::calculateMD5(originalImageName.GetAbsolutePath().ToStdString());
                item->image_info.back().target_filename = originalImageName.GetAbsolutePath();

                item->image_info.emplace_back(
                    sd_gui_utils::networks::ImageType::MASK_OUTPAINT | sd_gui_utils::networks::ImageType::MASK | sd_gui_utils::networks::ImageType::MASK_USED | sd_gui_utils::networks::ImageType::MOVEABLE,
                    outpaintMaskImageName.GetAbsolutePath());
                item->image_info.back().width           = outPaintedMaskImage.GetWidth();
                item->image_info.back().height          = outPaintedMaskImage.GetHeight();
                item->image_info.back().md5_hash        = sd_gui_utils::calculateMD5(outpaintMaskImageName.GetAbsolutePath().ToStdString());
                item->image_info.back().target_filename = outpaintMaskImageName.GetAbsolutePath();

                item->mask_image = outpaintMaskImageName.GetAbsolutePath();
            } else {
                originalImage.SaveFile(initialImageName.GetAbsolutePath(), wxBITMAP_TYPE_PNG);
                item->image_info.emplace_back(
                    sd_gui_utils::networks::ImageType::INITIAL | sd_gui_utils::networks::ImageType::COPYABLE,
                    initialImageName.GetAbsolutePath());
                item->image_info.back().width           = initialImage.GetWidth();
                item->image_info.back().height          = initialImage.GetHeight();
                item->image_info.back().md5_hash        = sd_gui_utils::calculateMD5(initialImageName.GetAbsolutePath().ToStdString());
                item->image_info.back().target_filename = initialImageName.GetAbsolutePath();
            }
        }
        item->initial_image = initialImageName.GetAbsolutePath();
    }

    item->params.cfg_scale    = static_cast<float>(this->m_cfg->GetValue());
    item->params.seed         = this->m_seed->GetValue();
    item->params.clip_skip    = this->m_clip_skip->GetValue();
    item->params.sample_steps = this->m_steps->GetValue();

    if (type == SDMode::TXT2IMG) {
        item->params.control_strength = this->m_controlnetStrength->GetValue();
    }

    if (type == SDMode::IMG2IMG) {
        item->params.strength = this->m_strength->GetValue();
    }

    if (this->m_controlnetModels->GetCurrentSelection() > 0 && type == SDMode::TXT2IMG) {
        if (diffusionModel.empty() == false) {
            this->writeLog(wxString::Format(_("Skipping controlnet with diffusion model: %s"), diffusionModel.c_str()));
        } else {
            auto controlnetModel = static_cast<sd_gui_utils::ModelFileInfo*>(this->m_controlnetModels->GetClientData(this->m_controlnetModels->GetSelection()));
            if (controlnetModel == nullptr) {
                wxLogError(_("No controlnet model found!"));
                this->m_queue->Enable();
                return;
            }
            if (controlnetModel->sha256.empty() == false) {
                item->hashes.controlnet_hash = controlnetModel->sha256;
            }
            if (item->server.empty() == false && controlnetModel->server_id != item->server) {
                errorlist.Add(_("The selected controlnet model not available on the selected server!"));
            } else {
                item->params.controlnet_path = controlnetModel->path;
                if (item->server.empty() == false) {
                    item->params.controlnet_path = controlnetModel->sha256;
                }

                if (this->m_controlnetImageOpen->GetPath().length() > 0) {
                    item->params.control_image_path = this->m_controlnetImageOpen->GetPath().utf8_string();
                    sd_gui_utils::ImageInfo ctrnetImageInfo(
                        sd_gui_utils::networks::ImageType::CONTROLNET | sd_gui_utils::networks::ImageType::COPYABLE,
                        item->params.control_image_path);

                    wxImage ctrnetImage;
                    if (ctrnetImage.LoadFile(item->params.control_image_path)) {
                        ctrnetImageInfo.width           = ctrnetImage.GetWidth();
                        ctrnetImageInfo.height          = ctrnetImage.GetHeight();
                        ctrnetImageInfo.md5_hash        = sd_gui_utils::calculateMD5(item->params.control_image_path);
                        ctrnetImageInfo.target_filename = item->params.control_image_path;
                        item->image_info.emplace_back(ctrnetImageInfo);
                    } else {
                        this->writeLog(wxString::Format(_("Can not load controlnet image: '%s', removing it"), item->params.control_image_path.c_str()), true);
                        item->params.control_image_path = "";
                        item->params.controlnet_path    = "";
                    }
                }
            }
        }
    } else {
        item->params.controlnet_path    = "";
        item->params.control_image_path = "";
        item->params.control_strength   = 0;
    }

    item->params.batch_count = this->m_batch_count->GetValue();
    this->m_width->GetValue().ToInt(&item->params.width);
    this->m_height->GetValue().ToInt(&item->params.height);
    item->params.vae_tiling = this->m_vae_tiling->GetValue();

    item->mode = type;

    if (item->params.seed == -1) {
        item->params.seed = sd_gui_utils::generateRandomInt(100000000, this->m_seed->GetMax());
        this->m_seed->SetValue(item->params.seed);
    }
    if (item->server.empty()) {
        if (modelinfo != nullptr && modelinfo->sha256.empty() && this->mapp->cfg->auto_gen_hash) {
            item->hash_fullsize = modelinfo->size;
            item->need_sha256   = true;
        }
    }
    if (errorlist.empty()) {
        if (item->mode == SDMode::IMG2IMG) {
            this->handleSdImages(item);
        }
        this->qmanager->AddItem(item);
    } else {
        if (errorlist.size() > 0) {
            wxString error_strs;
            for (const auto str : errorlist) {
                error_strs.Append(wxGetTranslation(str));
                error_strs.Append("\n");
            }
            wxMessageDialog dialog(this, error_strs, _("Error"), wxOK | wxICON_ERROR);
            dialog.ShowModal();
            this->m_queue->Enable();
            return;
        }
        this->m_queue->Enable();
        return;
    }

    this->mapp->config->Write("/last_prompt", this->m_prompt->GetValue());
    this->mapp->config->Write("/last_neg_prompt", this->m_neg_prompt->GetValue());
    if (modelinfo != nullptr) {
        this->mapp->config->Write("/last_model", wxString(modelinfo->path.data(), modelinfo->path.size()));
    }

    this->mapp->config->Write("/last_diffusion_model", this->m_filePickerDiffusionModel->GetPath());
    this->mapp->config->Write("/last_generation_mode", wxString::FromUTF8Unchecked(GenerationMode_str.at(type)));
    this->mapp->config->Flush(true);
    this->m_queue->Enable();
}

void MainWindowUI::OnControlnetImageOpen(wxFileDirPickerEvent& event) {
    this->onControlnetImageOpen(event.GetPath());
}

void MainWindowUI::OnControlnetImagePreviewButton(wxCommandEvent& event) {
    MainWindowImageDialog* dialog = new MainWindowImageDialog(this);
    dialog->SetName("controlnet_preview");
    wxImage img;
    img.LoadFile(this->m_controlnetImageOpen->GetPath());
    auto size = img.GetSize();

    dialog->SetSize(size.GetWidth() + 100, size.GetHeight() + 100);
    wxString title = wxString::Format(_("Controlnet Image %dx%dpx"), size.GetWidth(), size.GetHeight());
    dialog->SetTitle(title);
    dialog->SetIcon(this->GetIcon());
    dialog->m_bitmap->SetBitmap(img);

    if (wxPersistenceManager::Get().RegisterAndRestore(dialog) == false) {
        dialog->Center();
    }
    dialog->ShowModal();
}

void MainWindowUI::OnControlnetImageDelete(wxCommandEvent& event) {
    this->DisableControlNet();
    this->EnableControlNet();
    this->m_width->Enable();
    this->m_height->Enable();
    this->m_button7->Enable();  // swap resolution
}

void MainWindowUI::onFilePickerDiffusionModel(wxFileDirPickerEvent& event) {
    this->DisableModelSelect();
    this->DisableControlNet();

    this->m_cfg->SetValue(1.0f);
    this->m_filePickerClipG->SetPath("");
    this->m_filePickerClipG->Disable();
    this->m_neg_prompt->Disable();

    auto vaeModel = this->ModelManager->searchByName(std::vector<std::string>{{"/ae.safetensors"}, {"ae.safetensors"}}, sd_gui_utils::DirTypes::VAE);

    if (vaeModel != nullptr) {
        this->m_vae->SetStringSelection(vaeModel->path);
    }

    auto clipModel = this->ModelManager->searchByName(std::vector<std::string>{{"/clip_l.safetensors"}, {"clip_l.safetensors"}, {"clip_l"}}, sd_gui_utils::DirTypes::VAE);
    if (clipModel != nullptr) {
        this->m_filePickerClipL->SetPath(clipModel->path);
    }

    auto t5xxlModel = this->ModelManager->searchByName(std::vector<std::string>{{"/t5xxl.safetensors"}, {"t5xxl.safetensors"}, {"t5xxl"}}, sd_gui_utils::DirTypes::VAE);
    if (t5xxlModel != nullptr) {
        this->m_filePickerT5XXL->SetPath(t5xxlModel->path);
    }

    if (this->m_filePickerClipL->GetPath().empty() == false && this->m_filePickerT5XXL->GetPath().empty() == false && this->m_filePickerDiffusionModel->GetPath().empty() == false) {
        this->m_queue->Enable();
    }
}
void MainWindowUI::onFilePickerClipL(wxFileDirPickerEvent& event) {
    if (this->m_filePickerClipL->GetPath().empty() == false && this->m_filePickerT5XXL->GetPath().empty() == false && this->m_filePickerDiffusionModel->GetPath().empty() == false) {
        this->DisableControlNet();
    }
}
void MainWindowUI::onFilePickerT5XXL(wxFileDirPickerEvent& event) {
    if (this->m_filePickerClipL->GetPath().empty() == false && this->m_filePickerT5XXL->GetPath().empty() == false && this->m_filePickerDiffusionModel->GetPath().empty() == false) {
        this->m_queue->Enable();
    }
}
void MainWindowUI::onCleanDiffusionModel(wxCommandEvent& event) {
    this->EnableModelSelect();
    this->m_filePickerDiffusionModel->SetPath("");
    this->m_filePickerClipG->Enable();
    this->m_neg_prompt->Enable();
    this->m_filePickerClipL->SetPath("");
    this->m_filePickerT5XXL->SetPath("");
    this->EnableControlNet();
}

void MainWindowUI::Onimg2imgDropFile(wxDropFilesEvent& event) {
    // only just one file.. sry...
    auto files = event.GetFiles();
    auto file  = files[0];

    this->onimg2ImgImageOpen(file);
}

void MainWindowUI::OnImageOpenFileChanged(wxFileDirPickerEvent& event) {
    this->onimg2ImgImageOpen(event.GetPath());
}

void MainWindowUI::OnImg2ImgPreviewButton(wxCommandEvent& event) {
    MainWindowImageDialog* dialog = new MainWindowImageDialog(this);
    dialog->SetName("img2img_preview");
    wxImage img;
    img.LoadFile(this->m_img2imgOpen->GetPath());

    auto size = img.GetSize();

    dialog->SetSize(size.GetWidth() + 100, size.GetHeight() + 100);
    dialog->SetTitle(_("IMG2IMG - original image"));
    dialog->m_bitmap->SetBitmap(img);
    wxPersistenceManager::Get().RegisterAndRestore(dialog);
    dialog->ShowModal();
}

void MainWindowUI::OnDeleteInitialImage(wxCommandEvent& event) {
    this->m_inpaintResizeToSdSize->Disable();
    this->m_inpaintSaveMask->Disable();

    this->m_inpaintCanvasTop->SetValue("0");
    this->m_inpaintCanvasBottom->SetValue("0");
    this->m_inpaintCanvasLeft->SetValue("0");
    this->m_inpaintCanvasRight->SetValue("0");

    this->inpaintHelper->OnDeleteInitialImage();

    this->m_img2im_preview_img->Disable();
    this->m_delete_initial_img->Disable();
    this->m_inpaintBrushSizeSlider->SetValue(this->inpaintHelper->GetCurrentBrushSize());
    this->m_inpaintZoomSlider->SetValue(this->inpaintHelper->GetZoomFactor() * 100);
    this->m_inpaintClearMask->Disable();

    this->mapp->cfg->lastImg2ImgPath = this->m_img2imgOpen->GetPath();
    this->CheckQueueButton();
    this->m_inpaintImageResolution->SetLabel(wxEmptyString);
}

void MainWindowUI::OnUpscalerDropFile(wxDropFilesEvent& event) {
    // only just one file.. sry...
    auto files = event.GetFiles();
    auto file  = files[0];
    this->onUpscaleImageOpen(file);
}

void MainWindowUI::OnImageOpenFilePickerChanged(wxFileDirPickerEvent& event) {
    this->onUpscaleImageOpen(event.GetPath());
}

void MainWindowUI::OnDeleteUpscaleImage(wxCommandEvent& event) {
    this->m_static_upscaler_height->SetLabel("");
    this->m_static_upscaler_width->SetLabel("");
    this->m_static_upscaler_target_height->SetLabel("");
    this->m_static_upscaler_target_width->SetLabel("");
    auto origSize = this->m_upscaler_source_image->GetSize();
    this->m_upscaler_source_image->SetBitmap(this->AppOrigPlaceHolderBitmap);
    this->m_upscaler_source_image->SetSize(origSize);
    this->m_upscaler_filepicker->SetPath("");
    this->Layout();
    this->CheckQueueButton();
}

void MainWindowUI::OnUpscalerModelSelection(wxCommandEvent& event) {
    this->CheckQueueButton();
}

void MainWindowUI::OnUpscalerFactorChange(wxSpinEvent& event) {
    if (this->m_upscaler_filepicker->GetPath().empty()) {
        return;
    }

    wxImage image;
    image.LoadFile(this->m_upscaler_filepicker->GetPath());
    if (image.IsOk()) {
        this->m_static_upscaler_height->SetLabel(wxString::Format(_("%dpx"), image.GetHeight()));
        this->m_static_upscaler_width->SetLabel(wxString::Format(_("%dpx"), image.GetWidth()));

        int factor        = this->m_upscaler_factor->GetValue();
        int target_width  = factor * image.GetWidth();
        int target_height = factor * image.GetHeight();
        this->m_static_upscaler_target_height->SetLabel(wxString::Format(_("%dpx"), target_height));
        this->m_static_upscaler_target_width->SetLabel(wxString::Format(_("%dpx"), target_width));
    }
}

void MainWindowUI::OnDataModelTreeSelected(wxTreeListEvent& event) {
    wxTreeListItems selections;
    if (this->m_modelTreeList->GetSelections(selections) == 0) {
        return;
    }
    auto item      = selections.front();  // get the first
    auto modelInfo = this->modelUiManager->FindItem(item);

    if (!modelInfo) {
        return;
    }

    this->m_ModelFavorite->SetValue(sd_gui_utils::HasTag(modelInfo->tags, sd_gui_utils::ModelInfoTag::Favorite));
    this->UpdateModelInfoDetailsFromModelList(modelInfo);
    this->mapp->config->Write("/model_list/last_selected_model", wxString::FromUTF8Unchecked(modelInfo->path));
}

void MainWindowUI::OnModelFavoriteChange(wxCommandEvent& event) {
    wxTreeListItems selections;
    if (this->m_modelTreeList->GetSelections(selections) == 0) {
        return;
    }
    auto model = this->modelUiManager->SetSelectedAsFavorite(this->m_ModelFavorite->GetValue());
    if (model != nullptr) {
        this->modelUiManager->AddOrRemoveModelByDirType(model);
        this->ModelManager->UpdateInfo(model);
    }
}

void MainWindowUI::onCnOnCpu(wxCommandEvent& event) {
    this->mapp->config->Write("/controlnet_on_cpu", this->cnOnCpu->GetValue());
    this->mapp->config->Flush(true);
}
void MainWindowUI::onClipOnCpu(wxCommandEvent& event) {
    this->mapp->config->Write("/clip_on_cpu", this->clipOnCpu->GetValue());
    this->mapp->config->Flush(true);
}
void MainWindowUI::onVAEOnCpu(wxCommandEvent& event) {
    this->mapp->config->Write("/vae_on_cpu", this->vaeOnCpu->GetValue());
    this->mapp->config->Flush(true);
}
void MainWindowUI::onDiffusionFlashAttn(wxCommandEvent& event) {
    this->mapp->config->Write("/diffusion_flash_attn", this->diffusionFlashAttn->GetValue());
    this->mapp->config->Flush(true);
}

void MainWindowUI::OnCleanImageInfo(wxCommandEvent& event) {
    this->cleanUpImageInformations();
    this->m_imageInfoOpen->SetPath(this->mapp->cfg->lastImageInfoPath.empty() ? this->mapp->cfg->output : this->mapp->cfg->lastImageInfoPath);
}
void MainWindowUI::OnImageInfoLoadTxt2img(wxCommandEvent& event) {
    imageCommentToGuiParams(this->lastImageInfoParams, SDMode::TXT2IMG);
}
void MainWindowUI::OnImageInfoLoadImg2img(wxCommandEvent& event) {
    imageCommentToGuiParams(this->lastImageInfoParams, SDMode::IMG2IMG);
}
void MainWindowUI::OnImageInfoCopyPrompt(wxCommandEvent& event) {
    auto obj = dynamic_cast<wxButton*>(event.GetEventObject());

    if (obj == this->m_imageInfoPromptTo2txt2img) {
        auto prompt = this->m_imageInfoPrompt->GetValue();
        if (!prompt.empty()) {
            this->m_prompt->SetValue(prompt);
        }
    }
    if (obj == this->m_imageInfoPromptTo2img2img) {
        auto prompt = this->m_imageInfoPrompt->GetValue();
        if (!prompt.empty()) {
            this->m_prompt2->SetValue(prompt);
        }
    }

    if (obj == this->m_imageInfoNegPromptTo2txt2img) {
        auto prompt = this->m_imageInfoNegPrompt->GetValue();
        if (!prompt.empty()) {
            this->m_neg_prompt->SetValue(prompt);
        }
    }

    if (obj == this->m_imageInfoNegPromptTo2img2img) {
        auto prompt = this->m_imageInfoNegPrompt->GetValue();
        if (!prompt.empty()) {
            this->m_neg_prompt2->SetValue(prompt);
        }
    }
}

void MainWindowUI::OnImageInfoTryFindModel(wxCommandEvent& event) {
    auto loadedImage = this->m_imageInfoOpen->GetPath();
    if (wxFileName(loadedImage).FileExists() == false || wxFileName(loadedImage).IsDir()) {
        return;
    }

    auto model = this->ModelManager->findModelByImageParams(this->lastImageInfoParams);
    if (model == nullptr) {
        return;
    }

    this->ChangeModelByInfo(model);
}
void MainWindowUI::OnShowWidget(wxCommandEvent& event) {
    if (this->widget == nullptr) {
        this->widget = new MainWindowDesktopWidget(NULL);
        this->widget->PassParentEventHandler(this->GetEventHandler());
        wxPersistenceManager::Get().RegisterAndRestore(this->widget);
        widget->ShowWithEffect(wxShowEffect::wxSHOW_EFFECT_BLEND, 1000);
        this->m_showWidget->SetToolTip(_("Hide Widget"));
        this->mapp->cfg->widgetVisible = true;
    } else {
        widget->HideWithEffect(wxShowEffect::wxSHOW_EFFECT_BLEND, 1000);
        this->m_showWidget->SetToolTip(_("Show Widget"));
        this->mapp->cfg->widgetVisible = false;
        this->widget->Destroy();
        this->widget = nullptr;
    }
}
void MainWindowUI::OnServerSelect(wxCommandEvent& event) {
    this->HighLightPrompts(this->m_prompt);
    this->HighLightPrompts(this->m_prompt2);
    this->HighLightPrompts(this->m_neg_prompt);
    this->HighLightPrompts(this->m_neg_prompt2);
}

void MainWindowUI::cleanUpImageInformations() {
    this->m_imageinfo_preview->SetBitmap(blankimage_png_to_wx_bitmap());
    this->m_imageInfoList->SetValue(wxEmptyString);
    this->m_imageInfoPrompt->SetValue(wxEmptyString);
    this->m_imageInfoNegPrompt->SetValue(wxEmptyString);
    this->m_imageInfoLoadTotxt->Disable();
    this->m_imageInfoLoadToimg2img->Disable();
}

void MainWindowUI::OnImageInfoOpen(wxFileDirPickerEvent& event) {
    this->onimgInfoOpen(event.GetPath());
}

void MainWindowUI::onSamplerSelect(wxCommandEvent& event) {
}

void MainWindowUI::onSavePreset(wxCommandEvent& event) {
    wxTextEntryDialog dlg(this, _("Please specify a name (only alphanumeric)"));
    dlg.SetTextValidator(wxFILTER_ALPHA | wxFILTER_DIGITS);
    if (dlg.ShowModal() == wxID_OK) {
        sd_gui_utils::generator_preset preset;

        wxString preset_name = dlg.GetValue();
        preset.cfg           = this->m_cfg->GetValue();
        preset.clip_skip     = this->m_clip_skip->GetValue();
        preset.steps         = this->m_steps->GetValue();
        this->m_width->GetValue().ToInt(&preset.width);
        this->m_height->GetValue().ToInt(&preset.height);

        for (auto sampler : sd_gui_utils::samplerUiName) {
            if (this->m_sampler->GetStringSelection() == sampler.second) {
                preset.sampler = sampler.first;
                break;
            }
        }
        for (auto scheduler : sd_gui_utils::sd_scheduler_gui_names) {
            if (this->m_scheduler->GetStringSelection() == scheduler.second) {
                preset.scheduler = scheduler.first;
                break;
            }
        }
        preset.batch = this->m_batch_count->GetValue();
        preset.name  = preset_name.utf8_string();
        preset.type  = this->m_type->GetStringSelection().utf8_string();
        if (this->m_notebook1302->GetSelection() == (int)sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG) {
            preset.mode = modes_str[SDMode::TXT2IMG];
        }
        if (this->m_notebook1302->GetSelection() == (int)sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG) {
            preset.mode = modes_str[SDMode::IMG2IMG];
        }

        try {
            nlohmann::json j(preset);
            std::string presetfile = wxString::Format("%s%s%s.json", this->mapp->cfg->presets, wxString(wxFileName::GetPathSeparator()), preset.name).utf8_string();

            std::ofstream file(presetfile);
            file << j;
            file.close();
            this->LoadPresets();
        } catch (const std::exception& e) {
            this->writeLog(wxString::Format(_("Failed to save preset: %s"), e.what()));
        }
    }
}

void MainWindowUI::onLoadPreset(wxCommandEvent& event) {
    auto selected = this->m_preset_list->GetCurrentSelection();
    auto name     = this->m_preset_list->GetString(selected);

    for (auto preset : this->Presets) {
        if (preset.second.name == name) {
            this->m_cfg->SetValue(preset.second.cfg);
            this->m_clip_skip->SetValue(preset.second.clip_skip);
            // do not save seed
            // this->m_seed->SetValue(preset.second.seed);
            this->m_steps->SetValue(preset.second.steps);
            // when the width || height input is disabled, do not modify it (eg.:
            // controlnet works...)
            if (this->m_width->IsEnabled() || this->m_height->IsEnabled()) {
                this->m_width->SetValue(wxString::Format(wxT("%i"), preset.second.width));
                this->m_height->SetValue(wxString::Format(wxT("%i"), preset.second.height));
            }
            this->SetSamplerByType(preset.second.sampler);
            this->SetSchedulerByType(preset.second.scheduler);
            this->m_batch_count->SetValue(preset.second.batch);
        }
    }
}

void MainWindowUI::onSelectPreset(wxCommandEvent& event) {
    if (this->m_preset_list->GetCurrentSelection() == 0) {
        this->m_load_preset->Disable();
        this->m_delete_preset->Disable();
    } else {
        this->m_load_preset->Enable();
        this->m_delete_preset->Enable();
    }
}

void MainWindowUI::onDeletePreset(wxCommandEvent& event) {
    auto name = this->m_preset_list->GetStringSelection().utf8_string();

    if (this->Presets.find(name) != this->Presets.end()) {
        auto preset = this->Presets[name];
        std::remove(preset.path.c_str());
        this->LoadPresets();
    }
}

/**
 * @brief Set the GUI from a QueueItem
 * @param item QueueItem to set the GUI from
 *
 * This function will set the GUI controls from a QueueItem. It will only enable the generate button
 * if the model path is not empty and the file exists. It will also enable the generate button if the
 * open image is not empty.
 *
 * @note This function will also set the TAESD, VAE, and ControlNet models if they are not empty.
 */
void MainWindowUI::ChangeGuiFromQueueItem(QueueItem item) {
    if (item.id == 0) {
        return;
    }
    this->m_seed->SetValue(item.params.seed);
    this->m_width->SetValue(wxString::Format(wxT("%i"), item.params.width));
    this->m_height->SetValue(wxString::Format(wxT("%i"), item.params.height));
    this->m_steps->SetValue(item.params.sample_steps);
    this->m_clip_skip->SetValue(item.params.clip_skip);
    this->m_controlnetStrength->SetValue(item.params.control_strength);
    this->m_cfg->SetValue(item.params.cfg_scale);
    this->m_batch_count->SetValue(item.params.batch_count);

    if (item.mode == SDMode::TXT2IMG) {
        auto currentSkipLayers = sd_gui_utils::splitStr2int(this->m_skipLayers->GetValue().utf8_string(), ',');
        if (currentSkipLayers.size() != item.params.skip_layers.size()) {
            wxString newSkipLayers;
            for (auto layer : item.params.skip_layers) {
                newSkipLayers += wxString::Format("%d,", layer);
            }
            this->m_skipLayers->SetValue(newSkipLayers);
        } else {
            for (size_t i = 0; i < item.params.skip_layers.size(); i++) {
                if (currentSkipLayers[i] != item.params.skip_layers[i]) {
                    wxString newSkipLayers;
                    for (auto layer : item.params.skip_layers) {
                        newSkipLayers += wxString::Format("%d,", layer);
                    }
                    this->m_skipLayers->SetValue(newSkipLayers);
                }
            }
        }

        if (this->skipLayerStart->GetValue() != item.params.skip_layer_start) {
            this->skipLayerStart->SetValue(item.params.skip_layer_start);
        }

        if (this->skipLayerEnd->GetValue() != item.params.skip_layer_end) {
            this->skipLayerEnd->SetValue(item.params.skip_layer_end);
        }
        if (this->slgScale->GetValue() != item.params.slg_scale) {
            this->slgScale->SetValue(item.params.slg_scale);
        }
    }
    /*  if (item.mode == SDMode::IMG2IMG) {
          for (const auto& img : item.images) {
              if (wxFileExists(wxString::FromUTF8Unchecked(img.pathname)) == false) {
                  continue;
              }
              if (img.type == sd_gui_utils::networks::ImageType::INITIAL) {
                  // TODO send image to img2img
              }
              if (img.type == sd_gui_utils::networks::ImageType::MASK) {
                  // load mask from file
              }
          }
      }
  */

    if (!item.hashes.model_hash.empty()) {
        auto model = this->ModelManager->getIntoPtrByHash(item.hashes.model_hash, item.server);
        if (model != nullptr) {
            this->ChangeModelByInfo(model);
        }
    } else if (!item.params.model_path.empty()) {
        sd_gui_utils::ModelFileInfo* model = this->ModelManager->getIntoPtr(item.params.model_path);
        if (model != nullptr) {
            this->ChangeModelByInfo(model);
        }
    }

    if (item.params.clip_g_path.empty() == false && std::filesystem::exists(item.params.clip_g_path)) {
        this->m_filePickerClipG->SetPath(wxString::FromUTF8Unchecked(item.params.clip_g_path));
    }

    if (item.params.clip_l_path.empty() == false && std::filesystem::exists(item.params.clip_l_path)) {
        this->m_filePickerClipL->SetPath(wxString::FromUTF8Unchecked(item.params.clip_l_path));
    }

    if (item.params.t5xxl_path.empty() == false && std::filesystem::exists(item.params.t5xxl_path)) {
        this->m_filePickerT5XXL->SetPath(wxString::FromUTF8Unchecked(item.params.t5xxl_path));
    }

    if (!item.params.diffusion_model_path.empty() && std::filesystem::exists(item.params.diffusion_model_path)) {
        this->m_filePickerClipG->SetPath("");
        this->m_filePickerClipG->Disable();
        this->m_model->SetSelection(0);
        this->m_model->Disable();
        this->m_filePickerDiffusionModel->SetPath(wxString::FromUTF8Unchecked(item.params.diffusion_model_path));

        this->DisableControlNet();
        this->m_neg_prompt->Disable();

        if (this->m_filePickerClipL->GetPath().empty() == false && this->m_filePickerT5XXL->GetPath().empty() == false) {
            this->m_queue->Enable();
        }
    }

    this->SetSamplerByType(item.params.sample_method);
    this->SetSchedulerByType(item.params.schedule);
    this->SetTypeByType(item.params.wtype);

    if (!item.params.taesd_path.empty()) {
        sd_gui_utils::ModelFileInfo* taesd = nullptr;

        if (taesd == nullptr && item.hashes.taesd_hash.empty() == false) {
            taesd = this->ModelManager->getIntoPtrByHash(item.hashes.taesd_hash, item.server);
        }

        if (taesd == nullptr) {
            taesd = this->ModelManager->getIntoPtr(item.params.taesd_path);
        }

        if (taesd == nullptr) {
            this->writeLog(_("Taesd file not found: " + item.params.taesd_path), true);
            // TODO: search by hash if this is a remote job
        }
        if (taesd) {
            for (int i = 0; i < this->m_taesd->GetCount(); i++) {
                auto teasdModel = this->m_taesd->GetClientData(i);
                if (teasdModel != nullptr) {
                    if (taesd == teasdModel) {
                        this->m_taesd->SetSelection(i);
                        break;
                    }
                }
            }
        }
    }

    if (!item.params.vae_path.empty() &&
        std::filesystem::exists(item.params.vae_path)) {
        auto vae = this->ModelManager->getIntoPtr(item.params.vae_path);
        if (vae == nullptr) {
            this->writeLog(_("Vae file not found: " + item.params.vae_path), true);
        } else {
            for (int i = 0; i < this->m_vae->GetCount(); i++) {
                auto vaeModel = this->m_vae->GetClientData(i);
                if (vaeModel != nullptr) {
                    if (vaeModel == vae) {
                        this->m_vae->SetSelection(i);
                        break;
                    }
                }
            }
        }
    }

    this->m_vae_tiling->SetValue(item.params.vae_tiling);

    if (!item.params.control_image_path.empty()) {
        if (std::filesystem::exists(item.params.control_image_path)) {
            this->onControlnetImageOpen(item.params.control_image_path);
        }

        auto controlnetModel = this->ModelManager->getIntoPtr(item.params.controlnet_path);
        if (controlnetModel == nullptr) {
            this->writeLog(_("Controlnet file not found: " + item.params.controlnet_path), true);
        } else {
            this->m_controlnetModels->SetStringSelection(controlnetModel->name);
        }
    }
    this->CheckQueueButton();
}

void MainWindowUI::UpdateModelInfoDetailsFromModelList(sd_gui_utils::ModelFileInfo* modelinfo) {
    if (modelinfo == nullptr) {
        std::cerr << __FILE__ << ":" << __LINE__ << "Model is null" << std::endl;
        return;
    }
    this->m_model_details->DeleteAllItems();
    this->m_model_details_description->Hide();
    this->m_model_details_description->SetPage("");

    for (auto bm : this->modelImagePreviews) {
        bm->Destroy();
    }
    this->modelImagePreviews.clear();
    this->m_modelDetailsImageList->FitInside();

    if (modelinfo->model_type == sd_gui_utils::DirTypes::CHECKPOINT) {
        this->m_ModelFavorite->Enable();
    } else {
        this->m_ModelFavorite->Disable();
        this->m_ModelFavorite->SetValue(false);
    }

    wxVector<wxVariant> data;

    if (modelinfo->CivitAiInfo.model.name.empty() == false) {
        data.push_back(wxVariant(_("Name")));
        data.push_back(wxVariant(wxString::Format("%s %s", modelinfo->CivitAiInfo.model.name, modelinfo->CivitAiInfo.name)));
        this->m_model_details->AppendItem(data);
        data.clear();
    }

    data.push_back(wxVariant(_("File name")));
    data.push_back(wxVariant(wxString::Format("%s", modelinfo->name)));

    this->m_model_details->AppendItem(data);
    data.clear();

    if (modelinfo->sha256.empty() == false) {
        data.push_back(wxVariant(_("Hash")));
        data.push_back(wxVariant(wxString::Format("%s", modelinfo->sha256)));
        this->m_model_details->AppendItem(data);
        data.clear();
    }

    data.push_back(wxVariant(_("Type")));

    if (modelinfo->CivitAiInfo.model.type.empty() == false) {
        data.push_back(wxVariant(wxString::Format("%s", modelinfo->CivitAiInfo.model.type)));
    } else {
        data.push_back(wxVariant(sd_gui_utils::dirtypes_str.at(modelinfo->model_type)));
    }
    this->m_model_details->AppendItem(data);
    data.clear();

    if (wxFileExists(modelinfo->path)) {
        auto last_modified = wxFileName(modelinfo->path).GetModificationTime();
        data.push_back(wxVariant(_("Last modified")));
        data.push_back(wxVariant(wxString::Format("%s", last_modified.Format())));
        this->m_model_details->AppendItem(data);
        data.clear();
    }

    if (modelinfo->server_id.empty() == false) {
        auto srv = this->mapp->cfg->GetTcpServer(modelinfo->server_id);
        data.push_back(wxVariant(_("Server")));
        data.push_back(wxVariant(wxString::Format("%s", srv == nullptr ? _("deleted server") : srv->GetName())));
        this->m_model_details->AppendItem(data);
        data.clear();
    }

    if (modelinfo->CivitAiInfo.description.empty()) {
        this->m_model_details_description->Hide();
        this->m_model_details_description->SetPage("");
    } else {
        this->m_model_details_description->Show();
        // this->m_model_details_description->SetLabelMarkup(modelinfo->CivitAiInfo.description);
        this->m_model_details_description->SetPage(modelinfo->CivitAiInfo.description);
    }

    int idx = 0;
    for (auto file : modelinfo->CivitAiInfo.files) {
        data.push_back(wxVariant(wxString::Format(_("#%d id"), idx)));
        data.push_back(wxVariant(wxString::Format("%d", file.id)));
        this->m_model_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(wxString::Format(_("#%d name"), idx)));
        data.push_back(wxVariant(wxString::Format("%s", file.name)));
        this->m_model_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(wxString::Format(_("#%d format"), idx)));
        data.push_back(wxVariant(wxString::Format("%s", file.metadata.format)));
        this->m_model_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(wxString::Format(_("#%d type"), idx)));
        data.push_back(wxVariant(wxString::Format("%s", file.metadata.fp)));
        this->m_model_details->AppendItem(data);
        data.clear();
        idx++;
    }

    int index = 0;
    for (auto img : modelinfo->CivitAiInfo.images) {
        if (img.local_path.empty()) {
            continue;
        }
        if (std::filesystem::exists(img.local_path)) {
            wxImage resized        = sd_gui_utils::cropResizeImage(wxString::FromUTF8Unchecked(img.local_path), 256, 256, wxColour(51, 51, 51, wxALPHA_TRANSPARENT), wxString::FromUTF8Unchecked(this->mapp->cfg->thumbs_path));
            wxStaticBitmap* bitmap = new wxStaticBitmap(this->m_modelDetailsImageList, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(resized.GetSize()), 0);
            bitmap->Hide();
            bitmap->SetBitmap(resized);
            auto tooltip = wxString::Format(_("Resolution: %s Seed: %" PRId64 " Steps: %d"), (img.meta.Size.empty() ? _("Unknown") : img.meta.Size), img.meta.seed, img.meta.steps);
            this->bSizer891->Add(bitmap, 0, wxALL, 2);
            this->modelImagePreviews.emplace_back(bitmap);
            bitmap->Show();
            bitmap->SetToolTip(tooltip);
            bitmap->Bind(wxEVT_RIGHT_UP, [img, bitmap, modelinfo, tooltip, this](wxMouseEvent& event) {
                event.Skip();
                wxMenu* menu = new wxMenu();
                menu->Append(99, tooltip);
                menu->Enable(99, false);

                menu->Append(0, _("Copy prompts to text2img"));
                menu->Append(1, _("Copy prompts to img2img"));
                menu->Append(2, _("Send to img2img"));

                menu->Enable(0, !img.meta.prompt.empty());
                menu->Enable(1, !img.meta.negativePrompt.empty());
                menu->Enable(2, wxFileExists(img.local_path));

                menu->Append(3, _("Load available parameters"));
                menu->Append(10, _("Open the original image in the web browser"));

                menu->Enable(10, !img.url.empty());

                if (!img.meta.hashes.model.empty()) {
                    auto imgsModel = this->ModelManager->getIntoPtrByHash(img.meta.hashes.model);
                    if (imgsModel != nullptr && !imgsModel->civitaiPlainJson.empty()) {
                        menu->Append(4, wxString::Format(_("Select model %s"), imgsModel->name));
                    }
                }

                menu->Append(11, _("Send to Image Info tab"));
                menu->Enable(11, wxFileExists(img.local_path));

                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, img, modelinfo](wxCommandEvent& evt) {
                    evt.Skip();
                    auto id = evt.GetId();
                    switch (id) {
                        case 0: {
                            this->m_prompt->SetValue(img.meta.prompt);
                            this->m_neg_prompt->SetValue(img.meta.negativePrompt);
                        } break;
                        case 1: {
                            this->m_prompt2->SetValue(img.meta.prompt);
                            this->m_neg_prompt2->SetValue(img.meta.negativePrompt);
                        } break;
                        case 2: {
                            this->onimg2ImgImageOpen(img.local_path, true);
                            this->m_prompt2->SetValue(img.meta.prompt);
                            this->m_neg_prompt2->SetValue(img.meta.negativePrompt);
                        } break;
                        case 3: {
                            if (img.meta.cfgScale > 0) {
                                this->m_cfg->SetValue(img.meta.cfgScale);
                            }

                            int seed = static_cast<int>(img.meta.seed);
                            if (img.meta.seed > 0 && (seed > this->m_seed->GetMin() && seed < this->m_seed->GetMax())) {
                                this->m_seed->SetValue(seed);
                            }
                            if (img.meta.steps > -1) {
                                this->m_steps->SetValue(img.meta.steps);
                            }
                            if (!img.meta.Size.empty()) {
                                size_t pos = img.meta.Size.find("x");
                                if (pos != std::string::npos) {
                                    std::string part1 = img.meta.Size.substr(0, pos);
                                    std::string part2 = img.meta.Size.substr(pos + 1);

                                    int width, height;
                                    std::istringstream(part1) >> width;
                                    std::istringstream(part2) >> height;
                                    this->m_width->SetValue(wxString::Format(wxT("%i"), width));
                                    this->m_height->SetValue(wxString::Format(wxT("%i"), height));
                                }
                            }
                        } break;
                        case 4: {
                            this->ChangeModelByInfo(modelinfo);
                        } break;
                        case 10: {
                            // dialog to ask user to open the image
                            wxMessageDialog dialog(this, wxString::Format(_("Open the original image in the web browser?\n %s"), img.url), _("Open original image"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
                            if (dialog.ShowModal() == wxID_YES) {
                                wxLaunchDefaultBrowser(img.url);
                            }
                        } break;
                        case 11: {
                            this->onimgInfoOpen(wxString::FromUTF8Unchecked(img.local_path));
                        } break;
                        default: {
                            return;
                        } break;
                    }
                });
                bitmap->PopupMenu(menu);
                delete menu;
            });
        }
        index++;
    }

    this->m_modelDetailsImageList->FitInside();
    this->m_modelDetailsImageList->SetScrollbar(wxVERTICAL, 0, 0, 0);
}

MainWindowUI::~MainWindowUI() {
    if (this->civitwindow != nullptr) {
        this->civitwindow->Destroy();
    }
    if (this->widget != nullptr) {
        this->widget->Destroy();
    }

    for (auto& threadPtr : this->threads) {
        if (threadPtr->joinable()) {
            threadPtr->join();
        }
        delete threadPtr;
    }
    this->threads.clear();

    for (auto& v : this->voids) {
        delete v;
    }

    this->voids.clear();

    for (auto& b : this->modelImagePreviews) {
        delete b;
    }
    this->modelImagePreviews.clear();

    for (auto& b : this->jobImagePreviews) {
        b.second.bitmap->Destroy();
    }
    for (auto& h : this->processHelpers) {
        h->Stop(true);
    }
    this->jobImagePreviews.clear();
    this->TaskBar->Destroy();
    this->deInitLog();
}

void MainWindowUI::loadControlnetList() {
    this->LoadFileList(sd_gui_utils::DirTypes::CONTROLNET);
    auto lastSelection = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);
    this->modelUiManager->SelectItemByModelPath(lastSelection.utf8_string());
}

void MainWindowUI::loadEmbeddingList() {
    this->LoadFileList(sd_gui_utils::DirTypes::EMBEDDING);
    auto lastSelection = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);
    this->modelUiManager->SelectItemByModelPath(lastSelection.utf8_string());
}

void MainWindowUI::OnModelListPopUpClick(wxCommandEvent& evt) {
    evt.Skip();
    auto itemId = evt.GetId();
    if (itemId == wxNOT_FOUND) {
        return;
    }

    wxTreeListItems selections;
    if (this->m_modelTreeList->GetSelections(selections) == 0) {
        return;
    }

    auto item      = selections.front();  // get the first
    auto modelinfo = this->modelUiManager->FindItem(item);

    wxString shortname;
    // only allow the id 106, because no model needed to it
    if (modelinfo == nullptr && itemId != 106 && itemId != 311) {
        return;
    }
    if (modelinfo != nullptr) {
        shortname = wxFileName(modelinfo->path).GetName();
    }

    switch (itemId) {
        case 100: {
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelHashCalc, this, this->GetEventHandler(), modelinfo));
        } break;
        case 101: {
            this->m_prompt->SetValue(wxString::Format("%s <lora:%s:0.5>", this->m_prompt->GetValue(), shortname));
        } break;
        case 102: {
            this->m_neg_prompt->SetValue(wxString::Format("%s <lora:%s:0.5>", this->m_neg_prompt->GetValue(), shortname));
        } break;
        case 103: {
            this->m_prompt2->SetValue(wxString::Format("%s <lora:%s:0.5>", this->m_prompt2->GetValue(), shortname));
        } break;
        case 104: {
            this->m_neg_prompt2->SetValue(wxString::Format("%s <lora:%s:0.5>", this->m_neg_prompt2->GetValue(), shortname));
        } break;
        case 105: {
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
        } break;
        case 106: {  // create subfolder in three
            // modal dialog to ask the folder name
            wxTreeListItems selections;
            if (this->m_modelTreeList->GetSelections(selections) == 0) {
                return;
            }
            wxTreeListItem selected = selections.front();
            wxString basePath       = this->modelUiManager->findParentPath(selected, this->mapp->cfg);

            if (basePath == wxEmptyString) {
                return;
            }

            wxTextEntryDialog dlg(this, wxString::Format(_("Create subfolder in: \n%s\nThe folder will remain hidden within the application until a compatible model file is added to it."), basePath), _("Create folder"));
            if (dlg.ShowModal() == wxID_OK) {
                auto folderName = dlg.GetValue();
                if (folderName.empty()) {
                    return;
                }

                if (folderName.IsEmpty() == false) {
                    basePath.Append(wxFileName::GetPathSeparator() + folderName);
                    if (wxDir::Exists(basePath)) {
                        wxMessageBox(_("Folder already exists"), _("Error"), wxICON_ERROR);
                    } else {
                        if (wxFileName::Mkdir(basePath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL)) {
                            this->writeLog(wxString::Format(_("Created folder: %s"), basePath), true);
                            // auto newItem = this->treeListManager->GetOrCreateParent(groupFolderName);
                            // this->m_modelTreeList->AppendItem(item, folderName);
                        } else {
                            this->writeLog(wxString::Format(_("Can not create folder: %s"), basePath), true);
                        }
                    }
                }
            }
        } break;
        case 111: {
            this->m_prompt->SetValue(wxString::Format("%s %s", this->m_prompt->GetValue(), shortname));
        } break;
        case 112: {
            this->m_neg_prompt->SetValue(wxString::Format("%s %s", this->m_neg_prompt->GetValue(), shortname));
        } break;
        case 113: {
            this->m_prompt2->SetValue(wxString::Format("%s %s", this->m_prompt2->GetValue(), shortname));
        } break;
        case 114: {
            this->m_neg_prompt2->SetValue(wxString::Format("%s %s", this->m_neg_prompt2->GetValue(), shortname));
        } break;
        case 115: {
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
        } break;
        case 199: {
            wxString url = wxString::Format("https://civitai.com/models/%d", modelinfo->CivitAiInfo.modelId);
            wxLaunchDefaultBrowser(url);
        } break;
        case 200: {
            this->ChangeModelByInfo(modelinfo);
        } break;
        // model converter
        case 201: {
            this->PrepareModelConvert(modelinfo);
        } break;
        case 300: {  // move the model
            wxArrayString choices;
            for (auto& dir : this->ModelManager->getFolderGroups()) {
                if (dir.second.DirExists()) {
                    wxString dirName = wxFileName(dir.second.GetPath()).GetName();
                    dirName.Append(wxFileName::GetPathSeparator());
                    dirName.Append(dir.second.GetName());
                    choices.Add(dirName);
                }
            }

            choices.Sort(false);
            auto dialog = new wxSingleChoiceDialog(this, _("Select the directory to move the model"), _("Move Model"), choices);

            if (dialog->ShowModal() == wxID_OK) {
            }
        } break;
        case 310: {  // delete the model file
                     // modal for asking
            auto dlg = new wxMessageDialog(this, wxString::Format(_("Are you sure you want to delete this model?\n%s"), modelinfo->name), _("Delete Model"), wxYES_NO);
            if (dlg->ShowModal() == wxID_YES) {
                // remove from the gui
                if (modelinfo->model_type == sd_gui_utils::DirTypes::CHECKPOINT) {
                    for (int i = 0; i < this->m_model->GetCount(); i++) {
                        auto item = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_model->GetClientData(i));
                        if (item != nullptr && item->path == modelinfo->path) {
                            if (this->m_model->GetSelection() == i) {
                                this->m_model->SetSelection(0);
                            }
                            this->m_model->Delete(i);
                            break;
                        }
                    }
                }
                if (modelinfo->model_type == sd_gui_utils::DirTypes::VAE) {
                    for (int i = 0; i < this->m_vae->GetCount(); i++) {
                        auto item = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_vae->GetClientData(i));
                        if (item != nullptr && item->path == modelinfo->path) {
                            if (this->m_vae->GetSelection() == i) {
                                this->m_vae->SetSelection(0);
                            }
                            this->m_vae->Delete(i);
                            break;
                        }
                    }
                }
                if (modelinfo->model_type == sd_gui_utils::DirTypes::TAESD) {
                    for (int i = 0; i < this->m_taesd->GetCount(); i++) {
                        auto item = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_taesd->GetClientData(i));
                        if (item != nullptr && item->path == modelinfo->path) {
                            if (this->m_taesd->GetSelection() == i) {
                                this->m_taesd->SetSelection(0);
                            }
                            this->m_taesd->Delete(i);
                            break;
                        }
                    }
                }
                if (modelinfo->model_type == sd_gui_utils::DirTypes::CONTROLNET) {
                    for (int i = 0; i < this->m_controlnetModels->GetCount(); i++) {
                        auto item = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_controlnetModels->GetClientData(i));
                        if (item != nullptr && item->path == modelinfo->path) {
                            if (this->m_controlnetModels->GetSelection() == i) {
                                this->m_controlnetModels->SetSelection(0);
                            }
                            this->m_controlnetModels->Delete(i);
                            break;
                        }
                    }
                }
                if (modelinfo->model_type == sd_gui_utils::DirTypes::ESRGAN) {
                    for (int i = 0; i < this->m_upscaler_model->GetCount(); i++) {
                        auto item = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_upscaler_model->GetClientData(i));
                        if (item != nullptr && item->path == modelinfo->path) {
                            if (this->m_upscaler_model->GetSelection() == i) {
                                this->m_upscaler_model->SetSelection(0);
                            }
                            this->m_upscaler_model->Delete(i);
                            break;
                        }
                    }
                }
                this->modelUiManager->RemoveItem(modelinfo->path);
                this->ModelManager->deleteModel(modelinfo->path);
            }
        } break;
        case 311: {
            if (modelinfo != nullptr) {
                wxLaunchDefaultApplication(wxFileName(modelinfo->path).GetPath());
            } else {
                wxTreeListItems selections;
                if (this->m_modelTreeList->GetSelections(selections) > 0) {
                    wxString path = this->modelUiManager->findParentPath(selections.front(), this->mapp->cfg);
                    if (path != wxEmptyString) {
                        wxLaunchDefaultApplication(path);
                    }
                }
            }
        } break;
        default:
            break;
    }
}

void MainWindowUI::OnPopupClick(wxCommandEvent& evt) {
    evt.Skip();
    auto tu = evt.GetId();

    wxDataViewListStore* store = this->m_joblist->GetStore();
    auto currentRow            = this->m_joblist->GetSelectedRow();
    if (currentRow == wxNOT_FOUND) {
        return;
    }
    auto currentItem = this->m_joblist->RowToItem(currentRow);
    if (currentItem.IsOk() == false) {
        return;
    }
    auto id                          = store->GetItemData(currentItem);
    std::shared_ptr<QueueItem> qitem = this->qmanager->GetItemPtr(id);

    /*
            1 Copy and queue
            2 copy to text2img
            3 copy prompts to text2image
            4 copy prompts to img2img
            5 Details
            99 delete
    */

    switch (tu) {
        case wxID_DUPLICATE: {
            if (qitem->server.empty() == false) {
                auto srv = this->mapp->cfg->GetTcpServer(qitem->server);
                if (srv) {
                    if (srv->IsEnabled() && srv->IsConnected()) {
                        srv->DuplicateJob(qitem->id);
                        return;
                    }
                }
                return;
            }
            this->qmanager->Duplicate(qitem);
        }
        case 2:
            this->ChangeGuiFromQueueItem(*qitem);
            break;
        case 3:
            this->m_prompt->SetValue(wxString::FromUTF8Unchecked(qitem->original_prompt.empty() ? qitem->params.prompt : qitem->original_prompt));
            this->m_neg_prompt->SetValue(wxString::FromUTF8Unchecked(qitem->original_negative_prompt.empty() ? qitem->params.negative_prompt : qitem->original_negative_prompt));
            break;
        case 4:
            this->m_prompt2->SetValue(wxString::FromUTF8Unchecked(qitem->original_prompt.empty() ? qitem->params.prompt : qitem->original_prompt));
            this->m_neg_prompt2->SetValue(wxString::FromUTF8Unchecked(qitem->original_negative_prompt.empty() ? qitem->params.negative_prompt : qitem->original_negative_prompt));
            break;
        case 5: {
            if (qitem->params.model_path.empty() && qitem->params.diffusion_model_path.empty() == false) {
                if (std::filesystem::exists(qitem->params.diffusion_model_path)) {
                    this->m_filePickerDiffusionModel->SetPath(qitem->params.diffusion_model_path);
                } else {
                    wxMessageDialog dialog(this, _("Diffusion model not found"), _("Error"), wxOK | wxICON_ERROR);
                    dialog.ShowModal();
                }
            } else {
                if (!qitem->hashes.model_hash.empty()) {
                    auto model = this->ModelManager->getIntoPtrByHash(qitem->hashes.model_hash, qitem->server);
                    this->ChangeModelByInfo(model);
                } else {
                    auto model = this->ModelManager->getIntoPtr(qitem->params.model_path);
                    this->ChangeModelByInfo(model);
                }
            }
        } break;
        case 6: {
            this->m_upscaler_filepicker->SetPath(wxString::FromUTF8Unchecked(qitem->image_info.back().target_filename));
            this->onUpscaleImageOpen(wxString::FromUTF8Unchecked(qitem->image_info.back().target_filename));
            this->m_notebook1302->SetSelection(+sd_gui_utils::GuiMainPanels::PANEL_UPSCALER);  // switch to the upscaler
        } break;
        case 7: {
            this->onimg2ImgImageOpen(wxString::FromUTF8Unchecked(qitem->image_info.back().target_filename), true);
        } break;
        case 8: {
            if (qitem->server.empty()) {
                if (qitem->status == QueueStatus::PAUSED) {
                    this->qmanager->UnPauseItem(qitem);
                    return;
                }
                if (qitem->status == QueueStatus::PENDING) {
                    this->qmanager->PauseItem(qitem);
                    return;
                }
            } else {
                auto srv = this->mapp->cfg->GetTcpServer(qitem->server);
                if (srv && srv->IsConnected()) {
                    if (qitem->status == QueueStatus::PAUSED) {
                        srv->ResumeJob(qitem->id);
                        return;
                    }
                    if (qitem->status == QueueStatus::PENDING) {
                        srv->PauseJob(qitem->id);
                        return;
                    }
                }
            }
        } break;
        case wxID_DELETE: {
            if (qitem->server.empty() == false) {
                auto srv = this->mapp->cfg->GetTcpServer(qitem->server);
                if (srv && srv->IsConnected()) {
                    srv->DeleteJob(qitem->id);
                }
            } else {
                this->dataViewListManager->DeleteItem(qitem->id);
            }
            this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
        } break;
    }
}

void MainWindowUI::loadVaeList() {
    this->LoadFileList(sd_gui_utils::DirTypes::VAE);
    this->m_vae->SetToolTip(this->m_vae->GetStringSelection());
    auto lastSelection = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);
    this->modelUiManager->SelectItemByModelPath(lastSelection.utf8_string());
}

void MainWindowUI::loadEsrganList() {
    this->LoadFileList(sd_gui_utils::DirTypes::ESRGAN);
    auto lastSelection = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);
    this->modelUiManager->SelectItemByModelPath(lastSelection.utf8_string());
}

void MainWindowUI::loadTaesdList() {
    this->LoadFileList(sd_gui_utils::DirTypes::TAESD);
    auto lastSelection = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);
    this->modelUiManager->SelectItemByModelPath(lastSelection.utf8_string());
}

wxString MainWindowUI::paramsToImageComment(const QueueItem& myItem) {
    // TODO: copy original image's exif...
    if (myItem.mode == SDMode::UPSCALE) {
        return "";
    }
    if (myItem.params.model_path.empty() && myItem.params.diffusion_model_path.empty()) {
        return std::string();
    }

    wxFileName modelPath;
    wxString sha256;

    if (!myItem.params.model_path.empty()) {
        sd_gui_utils::ModelFileInfo* modelInfo = nullptr;
        if (myItem.hashes.model_hash.empty() == false) {
            modelInfo = this->ModelManager->getIntoPtrByHash(myItem.hashes.model_hash, myItem.server);
        }
        if (!modelInfo) {
            modelInfo = this->ModelManager->getIntoPtr(myItem.params.model_path);
        }

        if (modelInfo) {
            modelPath = wxFileName(modelInfo->path);
            sha256    = wxString::FromUTF8(modelInfo->sha256);
        }
        if (sha256.empty() == false) {
            sha256 = sha256.substr(0, 10);
        }
    } else {
        modelPath = wxFileName(myItem.params.diffusion_model_path);
    }

    wxString comment = wxString::FromUTF8Unchecked(myItem.params.prompt);

    if (!myItem.params.negative_prompt.empty()) {
        comment.append(wxString::Format("\nNegative prompt: %s", myItem.params.negative_prompt));
    }

    wxString cfgScale = wxString::Format("%.2f", myItem.params.cfg_scale);
    cfgScale.Replace(wxT(","), wxT("."));

    comment.append(wxString::Format("\nSteps: %d", myItem.params.sample_steps).utf8_string());
    comment.append(wxString::Format(", Seed: %" PRId64, myItem.params.seed).utf8_string());
    comment.append(wxString::Format(", Sampler: %s", sd_gui_utils::samplerSdWebuiNames.at(myItem.params.sample_method)));
    comment.append(wxString::Format(", Scheduler: %s", sd_gui_utils::schedule_str[myItem.params.schedule]));
    comment.append(wxString::Format(", Schedule type: %s", sd_gui_utils::schedulerSdWebuiNames.at(myItem.params.schedule)));
    comment.append(wxString::Format(", CFG scale: %s", cfgScale).utf8_string());
    comment.append(wxString::Format(", Size: %dx%d", myItem.params.width, myItem.params.height).utf8_string());
    comment.append(", Parser: stable-diffusion.cpp");
    comment.append(wxString::Format(", Model: %s", modelPath.GetName()));

    if (!sha256.empty()) {
        comment.append(wxString::Format(", Model hash: %s", sha256).utf8_string());
    }

    comment.append(wxString::Format(", Backend: stable-diffusion.cpp (ver.: %s using %s)", SD_CPP_VERSION, this->usingBackend).utf8_string());
    comment.append(wxString::Format(", App: %s", EXIF_SOFTWARE).utf8_string());
    comment.append(wxString::Format(", Operations: %s", modes_str[(int)myItem.mode]).utf8_string());

    int module_counter = 1;
    if (!myItem.params.vae_path.empty()) {
        auto vae_path = std::filesystem::path(myItem.params.vae_path);
        // set VAE for compatibility
        comment.append(wxString::Format(", VAE: %s", vae_path.filename().replace_extension().string()));
        comment.append(wxString::Format(", Module %d: %s", module_counter, vae_path.filename().replace_extension().string()));
        module_counter++;
    }
    if (!myItem.params.t5xxl_path.empty()) {
        auto t5xxl_path = std::filesystem::path(myItem.params.t5xxl_path);
        comment.append(wxString::Format(", Module %d: %s", module_counter, t5xxl_path.filename().replace_extension().string()));
        module_counter++;
    }

    if (!myItem.params.clip_g_path.empty()) {
        auto clip_g_path = std::filesystem::path(myItem.params.clip_g_path);
        comment.append(wxString::Format(", Module %d: %s", module_counter, clip_g_path.filename().replace_extension().string()));
        module_counter++;
    }

    if (!myItem.params.clip_l_path.empty()) {
        auto clip_l_path = std::filesystem::path(myItem.params.clip_l_path);
        comment.append(wxString::Format(", Module %d: %s", module_counter, clip_l_path.filename().replace_extension().string()));
        module_counter++;
    }

    return comment;
}
std::string MainWindowUI::paramsToImageCommentJson(QueueItem myItem, sd_gui_utils::ModelFileInfo modelInfo) {
    auto modelPath = std::filesystem::path(modelInfo.path);
    auto modelName = modelPath.filename().replace_extension().string();
    auto sha256    = modelInfo.sha256.substr(0, 10);

    nlohmann::json comment;
    comment["prompt"] = myItem.params.prompt;

    if (!myItem.params.negative_prompt.empty()) {
        comment["negative_prompt"] = myItem.params.negative_prompt;
    }
    comment["steps"]      = myItem.params.sample_steps;
    comment["seed"]       = myItem.params.seed;
    comment["sampler"]    = sd_gui_utils::sample_method_str[(int)myItem.params.sample_method];
    comment["cfg_scale"]  = myItem.params.cfg_scale;
    comment["width"]      = myItem.params.width;
    comment["height"]     = myItem.params.height;
    comment["parser"]     = "sd.cpp";
    comment["model"]      = modelName;
    comment["model_hash"] = sha256;
    comment["backend"]    = "stable-diffusion.cpp (" + this->usingBackend + ")";
    comment["app"]        = EXIF_SOFTWARE;
    comment["operations"] = modes_str[(int)myItem.mode];

    if (myItem.params.vae_path.empty() == false) {
        auto vaePath   = std::filesystem::path(myItem.params.vae_path);
        auto vaeName   = vaePath.filename().replace_extension().string();
        comment["vae"] = vaeName;
    }

    return comment.dump();
}

template <typename T>
inline void MainWindowUI::SendThreadEvent(wxEvtHandler* eventHandler, QueueEvents eventType, const T& payload, std::string text) {
    wxThreadEvent* e = new wxThreadEvent();
    e->SetString(wxString::Format("%d:%d:%s", (int)sd_gui_utils::ThreadEvents::QUEUE, (int)eventType, text));
    e->SetPayload(payload);
    wxQueueEvent(eventHandler, e);
}
template <typename T>
void MainWindowUI::SendThreadEvent(wxEvtHandler* eventHandler, sd_gui_utils::ThreadEvents eventType, const T& payload, std::string text) {
    wxThreadEvent* e = new wxThreadEvent();
    e->SetString(wxString::Format("%d:%s", (int)eventType, text));
    e->SetPayload(payload);
    wxQueueEvent(eventHandler, e);
}

template <typename T>
void MainWindowUI::SendThreadEvent(sd_gui_utils::ThreadEvents eventType, const T& payload, std::string text) {
    wxThreadEvent* e = new wxThreadEvent();
    e->SetString(wxString::Format("%d:%s", (int)eventType, text));
    e->SetPayload(payload);
    wxQueueEvent(this->GetEventHandler(), e);
}

void MainWindowUI::LoadFileList(sd_gui_utils::DirTypes type) {
    wxString basepath;
    switch (type) {
        case sd_gui_utils::DirTypes::VAE: {
            this->m_vae->Clear();
            this->m_vae->Append(_("Select one"));
            this->m_vae->Select(0);
            basepath = this->mapp->cfg->vae;
        } break;
        case sd_gui_utils::DirTypes::LORA: {
            basepath = this->mapp->cfg->lora;
        } break;
        case sd_gui_utils::DirTypes::CHECKPOINT: {
            this->m_model->Clear();
            this->m_model->Append(_("Select one"));
            this->m_model->Select(0);
            basepath = this->mapp->cfg->model;
        } break;
        case sd_gui_utils::DirTypes::LLAMA_MODELS: {
            this->m_languageModel->Clear();
            this->m_languageModel->Append(_("Select one"));
            this->m_languageModel->Select(0);
            basepath = this->mapp->cfg->llamapath;
        } break;
        case sd_gui_utils::DirTypes::PRESETS: {
            this->Presets.clear();
            this->m_preset_list->Clear();
            this->m_preset_list->Append(_("Not selected"));
            this->m_preset_list->Select(0);
            basepath = this->mapp->cfg->presets;
        } break;
        case sd_gui_utils::DirTypes::PROMPT_TEMPLATES: {
            this->PromptTemplates.clear();
            this->m_promptPresets->Clear();
            this->m_promptPresets->Append(_("Not selected"));
            this->m_promptPresets->Select(0);
            basepath = this->mapp->cfg->prompt_templates;
        } break;
        case sd_gui_utils::DirTypes::TAESD: {
            this->m_taesd->Clear();
            this->m_taesd->Append(_("Select one"));
            this->m_taesd->Select(0);
            basepath = this->mapp->cfg->taesd;
        } break;
        case sd_gui_utils::DirTypes::EMBEDDING: {
            basepath = this->mapp->cfg->embedding;
        } break;
        case sd_gui_utils::DirTypes::CONTROLNET: {
            this->m_controlnetModels->Clear();
            this->m_controlnetModels->Append(_("Not selected"));
            this->m_controlnetModels->Select(0);
            basepath = this->mapp->cfg->controlnet;
        } break;
        case sd_gui_utils::DirTypes::ESRGAN: {
            this->m_upscaler_model->Clear();
            this->m_upscaler_model->Append(_("Not selected"));
            this->m_upscaler_model->Select(0);
            basepath = this->mapp->cfg->esrgan;
        } break;
        default:
            return;
    }

    this->ModelManager->resetModels(type);
    wxArrayString entries;
    wxDir::GetAllFiles(basepath, &entries, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);

    for (const auto& entry : entries) {
        wxFileName file(entry);

        if (!file.FileExists()) {
            this->writeLog("File not exists: " + file.GetAbsolutePath().utf8_string());
            continue;
        }

        if (file.IsDir()) {
            continue;
        }

        wxString ext = file.GetExt();

        // File extension filtering
        if ((type == sd_gui_utils::DirTypes::CHECKPOINT &&
             std::find(CHECKPOINT_FILE_EXTENSIONS.begin(), CHECKPOINT_FILE_EXTENSIONS.end(), ext.ToStdString()) == CHECKPOINT_FILE_EXTENSIONS.end()) ||
            (type == sd_gui_utils::DirTypes::LLAMA_MODELS &&
             std::find(LLAMA_MODEL_FILES_EXTENSION.begin(), LLAMA_MODEL_FILES_EXTENSION.end(), ext.ToStdString()) == LLAMA_MODEL_FILES_EXTENSION.end()) ||
            (type == sd_gui_utils::DirTypes::LORA &&
             std::find(LORA_FILE_EXTENSIONS.begin(), LORA_FILE_EXTENSIONS.end(), ext.ToStdString()) == LORA_FILE_EXTENSIONS.end()) ||
            (type == sd_gui_utils::DirTypes::EMBEDDING &&
             std::find(EMBEDDING_FILE_EXTENSIONS.begin(), EMBEDDING_FILE_EXTENSIONS.end(), ext.ToStdString()) == EMBEDDING_FILE_EXTENSIONS.end()) ||
            (type == sd_gui_utils::DirTypes::VAE &&
             std::find(VAE_FILE_EXTENSIONS.begin(), VAE_FILE_EXTENSIONS.end(), ext.ToStdString()) == VAE_FILE_EXTENSIONS.end()) ||
            (type == sd_gui_utils::DirTypes::PRESETS && ext != "json") ||
            (type == sd_gui_utils::DirTypes::PROMPT_TEMPLATES && ext != "json") ||
            (type == sd_gui_utils::DirTypes::TAESD &&
             std::find(TAESD_FILE_EXTENSIONS.begin(), TAESD_FILE_EXTENSIONS.end(), ext.ToStdString()) == TAESD_FILE_EXTENSIONS.end()) ||
            (type == sd_gui_utils::DirTypes::ESRGAN &&
             std::find(ESRGAN_FILE_EXTENSIONS.begin(), ESRGAN_FILE_EXTENSIONS.end(), ext.ToStdString()) == ESRGAN_FILE_EXTENSIONS.end()) ||
            (type == sd_gui_utils::DirTypes::CONTROLNET &&
             std::find(CONTROLNET_FILE_EXTENSIONS.begin(), CONTROLNET_FILE_EXTENSIONS.end(), ext.ToStdString()) == CONTROLNET_FILE_EXTENSIONS.end())) {
            continue;
        }

        if (type == sd_gui_utils::DirTypes::CHECKPOINT) {
            auto model_info = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (model_info) {
                this->modelUiManager->AddItem(model_info);
            }

        } else if (type == sd_gui_utils::DirTypes::LLAMA_MODELS) {
            auto lamamodel = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (lamamodel) {
                this->modelUiManager->AddItem(lamamodel);
            }
        } else if (type == sd_gui_utils::DirTypes::LORA) {
            auto lora = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (lora) {
                this->modelUiManager->AddItem(lora);
            }
        } else if (type == sd_gui_utils::DirTypes::EMBEDDING) {
            auto embedding = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (embedding) {
                this->modelUiManager->AddItem(embedding);
            }

        } else if (type == sd_gui_utils::DirTypes::CONTROLNET) {
            auto controlnet = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (controlnet) {
                this->modelUiManager->AddItem(controlnet);
                // this->m_controlnetModels->Append(controlnet->name, controlnet);
            }

        } else if (type == sd_gui_utils::DirTypes::ESRGAN) {
            auto esrgan = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (esrgan) {
                this->modelUiManager->AddItem(esrgan);
                // this->m_upscaler_model->Append(esrgan->name, esrgan);
            }

        } else if (type == sd_gui_utils::DirTypes::VAE) {
            auto vae = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (vae) {
                this->modelUiManager->AddItem(vae);
                // this->m_vae->Append(vae->name, vae);
            }

        } else if (type == sd_gui_utils::DirTypes::PRESETS) {
            std::ifstream f(file.GetFullPath().ToStdString());
            try {
                nlohmann::json data                   = nlohmann::json::parse(f);
                sd_gui_utils::generator_preset preset = data;
                preset.path                           = file.GetFullPath().ToStdString();
                this->m_preset_list->Append(preset.name);
                this->Presets.emplace(preset.name, preset);
            } catch (...) {
                this->writeLog(wxString::Format(_("Failed to parse preset: %s"), file.GetFullPath()));
            }
        } else if (type == sd_gui_utils::DirTypes::PROMPT_TEMPLATES) {
            std::ifstream f(file.GetFullPath().ToStdString());
            try {
                nlohmann::json array = nlohmann::json::parse(f);
                if (array.is_array()) {
                    for (const auto& item : array) {
                        if (item.is_object()) {
                            sd_gui_utils::prompt_templates templateItem = item;
                            templateItem.path                           = file.GetFullPath().ToStdString();
                            this->m_promptPresets->Append(templateItem.name);
                            this->PromptTemplates.emplace(templateItem.name, templateItem);
                        }
                    }
                }
            } catch (...) {
                this->writeLog(wxString::Format(_("Failed to parse prompt template: %s"), file.GetFullPath()));
            }
        } else if (type == sd_gui_utils::DirTypes::TAESD) {
            auto taesd = this->ModelManager->addModel(file.GetAbsolutePath(), type, basepath);
            if (taesd) {
                this->modelUiManager->AddItem(taesd);
                // this->m_taesd->Append(taesd->name, taesd);
            }
        }
    }

    auto counted = this->ModelManager->GetCount(type);
    if (type == sd_gui_utils::DirTypes::PROMPT_TEMPLATES) {
        counted = this->PromptTemplates.size();
    }
    if (type == sd_gui_utils::DirTypes::PRESETS) {
        counted = this->Presets.size();
    }

    if (sd_gui_utils::dirtypes_str.contains(type)) {
        this->writeLog(wxString::Format(_("Loaded %s: %d"), sd_gui_utils::dirtypes_str.at(type), counted));
    } else {
        this->writeLog(wxString::Format(_("Loaded %s: %d"), sd_gui_utils::dirtypes_str.at(sd_gui_utils::DirTypes::UNKNOWN), counted));
    }

    if (type == sd_gui_utils::DirTypes::CHECKPOINT) {
        this->m_model->Enable((counted > 0));
    }

    if (type == sd_gui_utils::DirTypes::TAESD) {
        this->m_taesd->Enable((counted > 0));
    }

    if (type == sd_gui_utils::DirTypes::CONTROLNET) {
        this->m_controlnetModels->Enable((counted > 0));
    }

    if (type == sd_gui_utils::DirTypes::VAE) {
        this->m_vae->Enable((counted > 0));
    }

    if (type == sd_gui_utils::DirTypes::PRESETS) {
        this->m_preset_list->Enable((counted > 0));
    }

    if (type == sd_gui_utils::DirTypes::PROMPT_TEMPLATES) {
        this->m_promptPresets->Enable((counted > 0));
    }
    if (type == sd_gui_utils::DirTypes::LLAMA_MODELS) {
        this->m_languageModel->Enable((counted > 0));
    }
}
void MainWindowUI::loadLoraList() {
    this->LoadFileList(sd_gui_utils::DirTypes::LORA);
    auto lastSelection = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);
    this->modelUiManager->SelectItemByModelPath(lastSelection.utf8_string());
}

void MainWindowUI::OnCloseSettings(wxCloseEvent& event) {
    event.Skip();
    if (this->settingsWindow == nullptr) {
        return;
    }

    this->settingsWindow->Hide();
    if (this->mapp->cfg->enable_civitai == false && this->m_civitai->IsShown()) {
        this->m_civitai->Hide();
        if (this->civitwindow != nullptr) {
            this->civitwindow->Destroy();
            this->civitwindow = nullptr;
        }
    }
    if (this->mapp->cfg->enable_civitai == true && this->m_civitai->IsShown() == false) {
        this->m_civitai->Show();
    }

    // this->mapp->ReloadMainWindow(this->mapp->cfg->language, this->disableExternalProcessHandling);
    this->mapp->ChangeLocale(this->mapp->cfg->language);
    if (this->mapp->cfg->initServerList(this->GetEventHandler())) {
        if (this->mapp->cfg->servers.empty() == false) {
            for (auto& server : this->mapp->cfg->ListRemoteServers()) {
                if (server->IsEnabled() && server->IsConnected() == false) {
                    server->LoadAuthKeyFromSecretStore();
                    if (server->GetAuthKeyState() == true) {
                        server->StartServer();
                    } else {
                        server->SetEnabled(false, true);
                    }
                }
            }
        }
    }
    this->Maximize();
    this->Restore();
    // this->Thaw();
    this->Refresh();
    this->Update();
    this->settingsWindow->Destroy();
    this->settingsWindow = nullptr;
    // this->Show();
}
void MainWindowUI::OnCloseCivitWindow(wxCloseEvent& event) {
    this->civitwindow->Destroy();
    this->civitwindow = nullptr;
    event.Skip();
}

void MainWindowUI::imageCommentToGuiParams(std::unordered_map<wxString, wxString> params, SDMode mode) {
    for (auto item : params) {
        // try to find a sampler
        if (item.first.Lower().Contains("sampler")) {
            const auto fsampler = sd_gui_utils::FindSamplerFromString(item.second);
            this->SetSamplerByType(fsampler);
        }

        // try to find a scheduler
        if (item.first.Lower() == "scheduler" || item.first == "schedule type" || item.first.Lower().Contains("schedule")) {
            const auto fscheduler = sd_gui_utils::FindSchedulerFromString(item.second);
            this->SetSchedulerByType(fscheduler);
        }

        // get the seed, but our seed maximum is smaller than sdgui
        if (item.first.Lower().Contains("seed")) {
            if (this->m_seed->GetMax() >= std::atoi(item.second.c_str())) {
                this->m_seed->SetValue(std::atoi(item.second.c_str()));
            }
        }

        // get the image resolution
        if (item.first.Lower().Contains("size")) {
            size_t pos = item.second.find("x");
            wxString w, h;
            w = item.second.substr(0, pos);
            h = item.second.substr(pos + 1);
            this->m_width->SetValue(w);
            this->m_height->SetValue(h);
        }

        if (item.first == "steps") {
            this->m_steps->SetValue(std::atoi(item.second.c_str()));
        }

        if (item.first == "cfgscale" || item.first == "cfg scale" || item.first == "cfg" || item.first == "cfg_scale") {
            this->m_cfg->SetValue(static_cast<double>(std::atof(item.second.c_str())));
        }

        if (item.first == "negative_prompt") {
            if (mode == SDMode::IMG2IMG) {
                this->m_neg_prompt2->SetValue(item.second);
            } else {
                this->m_neg_prompt->SetValue(item.second);
            }
        }

        if (item.first == "prompt") {
            if (mode == SDMode::IMG2IMG) {
                this->m_prompt2->SetValue(item.second);
            } else {
                this->m_prompt->SetValue(item.second);
            }
        }

        if (item.first.Lower().Contains("vae")) {
            for (auto i = 0; i < this->m_vae->GetCount(); i++) {
                if (this->m_vae->GetString(i).Contains(item.second)) {
                    this->m_vae->Select(i);
                    this->m_vae->SetToolTip(this->m_vae->GetStringSelection());
                    break;
                }
            }
        }
    }

    // get the Module 1,Module 2 ...
    unsigned int module_counter = 1;
    std::string VaeToFind;
    for (auto item : params) {
        wxString searchName = wxString::Format("module %d", module_counter);
        if (item.first.Lower().Contains(searchName.Lower()) && VaeToFind.empty()) {
            // sadly, only one vae loadable once
            VaeToFind = item.second;
        }
        module_counter++;
    }
    if (VaeToFind.empty() == false) {
        for (auto i = 0; i < this->m_vae->GetCount(); i++) {
            if (this->m_vae->GetString(i).Contains(VaeToFind)) {
                this->m_vae->Select(i);
                this->m_vae->SetToolTip(this->m_vae->GetStringSelection());
                break;
            }
        }
    }

    auto model = this->ModelManager->findModelByImageParams(params);
    if (model != nullptr) {
        this->ChangeModelByInfo(model);
    }
}

void MainWindowUI::onimg2ImgImageOpen(const wxString& file, bool forceResolutions) {
    this->m_inpaintCanvasTop->SetValue(wxString("0"));
    this->m_inpaintCanvasRight->SetValue(wxString("0"));
    this->m_inpaintCanvasBottom->SetValue(wxString("0"));
    this->m_inpaintCanvasLeft->SetValue(wxString("0"));

    if (file.length() < 1) {
        std::cerr << __FILE__ << ":" << __LINE__ << " onimg2ImgImageOpen: file is empty" << std::endl;
        return;
    }
    wxImage img;
    if (img.LoadFile(file)) {
        int origWidth = 512, origHeight = 512;
        this->m_width->GetValue().ToInt(&origWidth);
        this->m_height->GetValue().ToInt(&origHeight);

        if (forceResolutions) {
            this->m_width->SetValue(wxString::Format(wxT("%i"), img.GetWidth()));
            this->m_height->SetValue(wxString::Format(wxT("%i"), img.GetHeight()));
        }

        if (img.GetWidth() > origWidth || img.GetHeight() > origHeight) {
            this->m_inpaintResizeToSdSize->Enable();
        } else {
            if (forceResolutions == false) {
                if (img.GetWidth() < origWidth || img.GetHeight() < origHeight) {
                    this->m_inpaintResizeToSdSize->Disable();
                    this->m_inpaintSaveMask->Disable();
                    wxString msg = wxString::Format(_("Image size (%d x %d) is smaller than the original size (%d x %d). Please increase the size to at least (%d x %d)."), img.GetWidth(), img.GetHeight(), origWidth, origHeight, origWidth, origHeight);
                    wxMessageDialog dialog(this, msg, "Error", wxOK | wxICON_ERROR);
                    this->inpaintHelper->Reset();
                    dialog.ShowModal();
                    return;
                }
            }
        }

        this->m_inpaintSaveMask->Enable();

        this->inpaintHelper->SetImage(img);

        this->m_img2im_preview_img->Enable();
        this->m_delete_initial_img->Enable();
        this->m_width->Enable();
        this->m_height->Enable();

        this->m_img2imgOpen->SetPath(file);
        this->mapp->cfg->lastImg2ImgPath = this->m_img2imgOpen->GetPath();
        this->mapp->config->Write("/lastImg2ImgPath", this->m_img2imgOpen->GetPath());
        this->mapp->config->Flush(true);
        this->m_inpaintImageResolution->SetLabel(wxString::Format("%d x %d", img.GetWidth(), img.GetHeight()));

        this->readMetaDataFromImage(wxFileName(file), SDMode::IMG2IMG, true);
        this->CheckQueueButton();
    } else {
        wxMessageBox(_("Can not open image!"));
        this->inpaintHelper->Reset();
    }
}

void MainWindowUI::onimgInfoOpen(const wxString& file) {
    this->cleanUpImageInformations();
    if (file.empty()) {
        this->m_imageInfoOpen->SetPath(this->mapp->cfg->lastImageInfoPath);
        return;
    }

    if (!wxFile::Exists(file)) {
        this->m_imageInfoOpen->SetPath(this->mapp->cfg->lastImageInfoPath);
        return;
    }

    wxFileName imagePath(file);

    wxImage image(imagePath.GetFullPath());
    auto origSize = this->m_imageinfo_preview->GetSize();
    auto preview  = sd_gui_utils::ResizeImageToMaxSize(image, origSize.GetWidth(), origSize.GetHeight());
    this->m_imageinfo_preview->SetBitmap(preview);
    this->m_imageinfo_preview->SetSize(origSize);

    this->bSizer117->Layout();

    if (image.IsOk()) {
        this->m_imageInfoOpen->SetPath(imagePath.GetAbsolutePath());
        this->mapp->cfg->lastImageInfoPath = imagePath.GetAbsolutePath().utf8_string();
        this->mapp->config->Write("/lastImageInfoPath", imagePath.GetPath());
        this->mapp->config->Flush(true);

        std::unordered_map<wxString, wxString> metadata;

        if (image.GetType() == wxBITMAP_TYPE_PNG) {
            try {
                const auto meta = sd_gui_utils::ReadMetadata(imagePath.GetAbsolutePath().utf8_string());
                if (meta.contains("Parameters")) {
                    metadata = sd_gui_utils::parseExifPrompts(meta.at("Parameters").ToStdString());
                }
                if (meta.contains("parameters")) {
                    metadata = sd_gui_utils::parseExifPrompts(meta.at("parameters").ToStdString());
                }
            } catch (const std::exception& e) {
                this->writeLog(wxString::Format("Error reading metadata: %s File: %s", e.what(), imagePath.GetAbsolutePath().utf8_string()));
            }
        }

        if (image.GetType() == wxBITMAP_TYPE_JPEG) {
            metadata = this->getMetaDataFromImage(imagePath);
        }

        if (metadata.empty()) {
            this->writeLog(wxString::Format(_("No metadata found in image: %s"), imagePath.GetAbsolutePath().utf8_string()), true);
            return;
        }

        if (metadata.contains("prompt")) {
            this->m_imageInfoPrompt->SetValue(metadata.at("prompt"));
        }

        if (metadata.contains("negative_prompt")) {
            this->m_imageInfoNegPrompt->SetValue(metadata.at("negative_prompt"));
        }
        wxString meta;
        for (const auto& [key, value] : metadata) {
            if (key == "prompt" || key == "negative_prompt") {
                continue;
            }
            meta += key + ": " + value + "\n";
        }
        if (meta.empty()) {
            return;
        }
        this->m_imageInfoList->SetValue(meta);
        this->m_imageInfoLoadTotxt->Enable();
        this->m_imageInfoLoadToimg2img->Enable();
        this->lastImageInfoParams = metadata;
        if (this->m_notebook1302->GetSelection() != sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO) {
            this->m_notebook1302->SetSelection(+sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO);
        }
    }
}

void MainWindowUI::onUpscaleImageOpen(const wxString& file) {
    if (file.empty()) {
        this->writeLog(_("Upscaler image open: file name is empty"), true);
        return;
    }

    wxFileName fn(file);
    if (fn.FileExists() == false) {
        this->writeLog(wxString::Format(_("Upscaler image open: file not found: %s"), file), true);
        return;
    }

    if (fn.IsDir()) {
        return;
    }
    wxImage img;
    if (img.LoadFile(file)) {
        this->mapp->cfg->lastUpscalerPath = fn.GetPath();
        this->mapp->config->Write("lastUpscalerPath", fn.GetPath());

        this->m_upscaler_filepicker->SetPath(file);
        auto origSize = this->m_upscaler_source_image->GetSize();
        auto preview  = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(), origSize.GetHeight());

        this->m_upscaler_source_image->SetScaleMode(wxStaticBitmap::Scale_AspectFill);
        this->m_upscaler_source_image->SetBitmap(preview);
        this->m_upscaler_source_image->SetSize(origSize);

        this->m_static_upscaler_height->SetLabel(wxString::Format("%dpx", img.GetHeight()));
        this->m_static_upscaler_width->SetLabel(wxString::Format("%dpx", img.GetWidth()));

        int factor        = this->m_upscaler_factor->GetValue();
        int target_width  = factor * img.GetWidth();
        int target_height = factor * img.GetHeight();

        this->m_static_upscaler_target_height->SetLabel(wxString::Format("%dpx", target_height));
        this->m_static_upscaler_target_width->SetLabel(wxString::Format("%dpx", target_width));
        this->CheckQueueButton();
    }
}

void MainWindowUI::StartGeneration(std::shared_ptr<QueueItem> myJob) {
    if (this->disableExternalProcessHandling == false) {
        for (auto& p : this->processHelpers) {
            if (p && p->GetProcessType() == ExternalProcessHelper::ProcessType::diffuser) {
                if (p->IsAlive() == false) {
                    wxMessageDialog errorDialog(NULL, wxT("An error occurred while starting the generation process"), wxT("Error"), wxOK | wxICON_ERROR);
                    myJob->status_message = _("Error accessing to the background process. Please try again");
                    myJob->status         = QueueStatus::FAILED;
                    this->qmanager->SetStatus(QueueStatus::FAILED, myJob);
                    errorDialog.ShowModal();
                    continue;
                }

                try {
                    myJob->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    this->qmanager->SetStatus(QueueStatus::PENDING, myJob);
                    nlohmann::json j = myJob->ConvertToSharedMemory();
                    std::string msg  = j.dump();
                    p->write(msg);
                } catch (const std::exception& e) {
                    std::cerr << __FILE__ << ":" << __LINE__ << e.what() << std::endl;
                }

                break;
            }
        }
    } else {
        myJob->status_message = _("Local diffusion is disabled");
        myJob->status         = QueueStatus::FAILED;
        this->qmanager->SetStatus(QueueStatus::FAILED, myJob);
    }
}

void MainWindowUI::loadSamplerList() {
    this->m_sampler->Clear();

    for (auto sampler : sd_gui_utils::samplerUiName) {
        auto _u = this->m_sampler->Append(sampler.second);
        this->m_sampler->SetClientObject(_u, new sd_gui_utils::IntClientData(sampler.first));
        if (sampler.first == sample_method_t::EULER) {
            this->m_sampler->Select(_u);
        }
    }
}

void MainWindowUI::loadSchedulerList() {
    this->m_scheduler->Clear();

    for (auto type : sd_gui_utils::sd_scheduler_gui_names) {
        auto _z = this->m_scheduler->Append(type.second);

        this->m_scheduler->SetClientObject(_z, new sd_gui_utils::IntClientData(type.first));

        if (type.first == schedule_t::DEFAULT) {
            this->m_scheduler->Select(_z);
        }
    }
}
void MainWindowUI::loadTypeList() {
    this->m_type->Clear();
    unsigned int selected = 0;
    for (auto type : sd_gui_utils::sd_type_gui_names) {
        auto _z = this->m_type->Append(type.second);
        this->m_type->SetClientObject(_z, new sd_gui_utils::IntClientData(type.first));
    }

    std::string selectByBackend = "";

    this->m_type->Select(selected);

    if (this->usingBackend == "cuda" || this->usingBackend == "hipblas") {
        selectByBackend = "F16";
    }

    if (this->usingBackend == "avx512" || this->usingBackend == "avx2" || this->usingBackend == "avx") {
        selectByBackend = "F32";
    }

    if (!selectByBackend.empty()) {
        for (const auto& item : sd_gui_utils::sd_type_gui_names) {
            if (item.second == selectByBackend) {
                this->m_type->Select(item.first);
                break;
            }
        }
    }
}

void MainWindowUI::ModelStandaloneHashingCallback(size_t readed_size, std::string sha256, void* custom_pointer) {
    sd_gui_utils::VoidHolder* holder = static_cast<sd_gui_utils::VoidHolder*>(custom_pointer);

    wxEvtHandler* eventHandler             = (wxEvtHandler*)holder->p1;
    sd_gui_utils::ModelFileInfo* modelinfo = (sd_gui_utils::ModelFileInfo*)holder->p2;
    modelinfo->hash_progress_size          = readed_size;
    if (readed_size > 0) {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
    }
}

void MainWindowUI::OnThreadMessage(wxThreadEvent& e) {
    e.Skip();

    // this is a taskbar event
    if (e.GetId() == 9999) {
        this->m_statusBar166->SetStatusText(e.GetString(), 1);
        return;
    }

    if (e.GetId() == 10000) {
        this->m_statusBar166->SetStatusText(e.GetString(), 2);
        return;
    }
    // chat updated
    if (e.GetId() == 10001) {
        {
            std::lock_guard<std::mutex> lock(this->chat_mutex);
            this->llamaGuiHelper->ProcessEvents();
        }
        return;
    }

    auto msg = e.GetString().utf8_string();

    std::string token                      = msg.substr(0, msg.find(":"));
    std::string content                    = msg.substr(msg.find(":") + 1);
    sd_gui_utils::ThreadEvents threadEvent = (sd_gui_utils::ThreadEvents)std::stoi(token);

    if (threadEvent == sd_gui_utils::ThreadEvents::QUEUE) {
        // only numbers here...
        QueueEvents event = (QueueEvents)std::stoi(content);

        std::shared_ptr<QueueItem> item = e.GetPayload<std::shared_ptr<QueueItem>>();

        if (QueueEvents::ITEM_ADDED != event) {
            this->qmanager->UpdateItem(*item);
        }

        wxString title;
        wxString message;

        switch (event) {
            case QueueEvents::ITEM_MODEL_HASH_START: {
            } break;
            case QueueEvents::ITEM_MODEL_HASH_UPDATE: {
                MainWindowUI::SendThreadEvent(sd_gui_utils::ThreadEvents::HASHING_PROGRESS, item);  // this will call the STANDALONE_HASHING_PROGRESS event too
                this->UpdateCurrentProgress(item, event);
            } break;
            case QueueEvents::ITEM_MODEL_HASH_DONE: {
                MainWindowUI::SendThreadEvent(sd_gui_utils::ThreadEvents::HASHING_DONE, item);  // this will call the STANDALONE_HASHING_DONE event too
                this->UpdateCurrentProgress(item, event);
            } break;
                // new item added
            case QueueEvents::ITEM_ADDED: {
                if (item->server.empty()) {
                    this->UpdateJobInfoDetailsFromJobQueueList(item);
                    this->dataViewListManager->AddItem(item);
                    this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
                } else {
                    auto srv = this->mapp->cfg->GetTcpServer(item->server);
                    if (srv) {
                        srv->SendQueueJob(item);
                        this->writeLog(wxString::Format(_("Job added to %s"), srv->GetName()), true);
                    }
                }
            } break;
                // item status changed
            case QueueEvents::ITEM_STATUS_CHANGED: {
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->dataViewListManager->UpdateColumns(DataViewListManager::queueJobColumns::STATUS | DataViewListManager::queueJobColumns::STATUS_MESSAGE, item);
                this->UpdateCurrentProgress(item, event);
                if (isDEBUG) {
                    auto status_str = QueueStatus_GUI_str.at(item->status);
                    this->writeLog(wxString::Format(_("Job status changed: %s job id: %" PRIu64 " status: %s"), item->status_message, item->id, status_str), true);
                }
            } break;
                // item updated... -> set the progress bar in the queue
            case QueueEvents::ITEM_UPDATED: {
                this->dataViewListManager->UpdateColumns(DataViewListManager::queueJobColumns::PROGRESS | DataViewListManager::queueJobColumns::SPEED, item);
                this->UpdateCurrentProgress(item, event);
                this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
            } break;
                // this is just the item start, if no mode
                // loaded, then will trigger model load
                // event
            case QueueEvents::ITEM_START: {
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->StartGeneration(item);
                this->UpdateCurrentProgress(item, event);
            } break;
            case QueueEvents::ITEM_FINISHED: {
                // update again
                this->SendThreadEvent(this->GetEventHandler(), QueueEvents::ITEM_UPDATED, item);
                this->m_stop_background_process->Enable();
                this->jobsCountSinceSegfault++;
                this->stepsCountSinceSegfault += item->steps;
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->writeLog(message);
                {
                    if (this->jobsCountSinceSegfault.load() > 0) {
                        wxString msg;
                        if (this->jobsCountSinceSegfault.load() > 1) {
                            msg = _("%d jobs and %d steps without a segfault");
                        } else {
                            msg = _("%d job and %d step without a segfault");
                        }

                        wxThreadEvent* event = new wxThreadEvent();
                        event->SetString(wxString::Format(msg, this->jobsCountSinceSegfault.load(), this->stepsCountSinceSegfault.load()));
                        event->SetId(9999);
                        wxQueueEvent(this, event);
                    }
                }
                // update global status info
                this->UpdateCurrentProgress(item, event);
            } break;
            case QueueEvents::ITEM_MODEL_LOADED: {  // MODEL_LOAD_DONE
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->writeLog(wxString::Format(_("Model loaded: %s\n"), item->model));
                this->UpdateCurrentProgress(item, event);
            } break;
            case QueueEvents::ITEM_MODEL_LOAD_START: {  // MODEL_LOAD_START
                this->writeLog(wxString::Format(_("Model load started: %s\n"), item->model));
                this->UpdateCurrentProgress(item, event);
            } break;
            case QueueEvents::ITEM_MODEL_FAILED: {  // MODEL_LOAD_ERROR
                this->writeLog(wxString::Format(_("Model load failed: %s\n"), item->model));
                title   = _("Model load failed");
                message = wxString::Format(_("The '%s' just failed to load... for more details please see the logs!"), item->model);
                this->UpdateCurrentProgress(item, event);
            } break;
            case QueueEvents::ITEM_GENERATION_STARTED:  // GENERATION_START
                if (item->mode == SDMode::IMG2IMG ||
                    item->mode == SDMode::TXT2IMG) {
                    this->writeLog(wxString::Format(
                        _("Diffusion started. Seed: %" PRId64 " Batch: %d %dx%dpx Cfg: %.1f Steps: %d"),
                        item->params.seed, item->params.batch_count, item->params.width,
                        item->params.height, item->params.cfg_scale,
                        item->params.sample_steps));
                }
                if (item->mode == SDMode::UPSCALE) {
                    this->writeLog(wxString::Format(_("Upscale start, factor: %d image: %s\n"), item->upscale_factor, item->initial_image));
                }
                this->UpdateCurrentProgress(item, event);
                break;
            case QueueEvents::ITEM_FAILED:  // GENERATION_ERROR
                this->writeLog(wxString::Format(_("Generation error: %s\n"), wxString(item->status_message.data(), item->status_message.size())));
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->UpdateCurrentProgress(item, event);
                break;
            default:
                break;
        }
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_CONNECTED) {
        sd_gui_utils::sdServer* server = e.GetPayload<sd_gui_utils::sdServer*>();
        this->writeLog(wxString::Format(_("Connected to server: %s"), server->GetName()));
        // add to the server selector if not exists
        bool found = false;
        for (size_t i = 1; i < this->m_server->GetCount(); i++) {
            auto _srv = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(i));
            if (_srv) {
                if (_srv->IsConnected() == false) {
                    this->m_server->Delete(i);
                }
                if (_srv == server) {
                    found = true;
                }
            }
        }
        if (found == false && server->IsConnected()) {
            auto added_id = this->m_server->Append(server->GetName());
            this->m_server->SetClientData(added_id, server);
        }
        if (this->m_server->GetCount() > 1) {
            this->m_server->Show();
            this->m_server->GetParent()->Layout();
        } else {
            this->m_server->Show(false);
            this->m_server->GetParent()->Layout();
        }
        this->m_server->SetSelection(0);

        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_DISCONNECTED) {
        sd_gui_utils::sdServer* server = e.GetPayload<sd_gui_utils::sdServer*>();
        this->writeLog(wxString::Format(_("Server disconnected: %s %s"), server->GetName(), content));
        // remove from m_server too
        for (size_t i = this->m_server->GetCount() - 1; i > 0; i--) {
            auto _srv = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(i));
            if (_srv && _srv == server) {
                this->m_server->Delete(i);
                break;
            }
        }
        if (this->m_server->GetCount() > 1) {
            this->m_server->Show();
            this->m_server->GetParent()->Layout();
        } else {
            this->m_server->Show(false);
            this->m_server->GetParent()->Layout();
        }
        this->modelUiManager->DeleteByServerId(server->GetId());
        this->dataViewListManager->RemoveRemoteItems(server->GetId());
        this->qmanager->RemoveRemoteItems(server->GetId());
        this->ModelManager->UnloadModelsByServer(server);
        this->UpdateJobImagePreviews();
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_ERROR) {
        sd_gui_utils::sdServer* server = e.GetPayload<sd_gui_utils::sdServer*>();
        this->writeLog(wxString::Format(_("Server error: %s - %s - %s"), server->GetName(), server->GetDisconnectReason(), content));
        this->mapp->cfg->ServerEnable(server->GetInternalId(), false, true);
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_JOBLIST_UPDATE_START) {
        this->writeLog(_("Server job list update started"), true);
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_JOBLIST_UPDATE_FINISHED) {
        this->writeLog(_("Server job list update finished"), true);
        if (this->m_joblist->GetItemCount() > 0) {
            this->m_joblist->EnsureVisible(this->m_joblist->GetTopItem());
        }
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_JOB_DELETE) {
        auto resp = e.GetPayload<sd_gui_utils::DeleteResponse>();
        if (resp.state == true) {
            this->dataViewListManager->DeleteItem(resp.job_id);
            this->writeLog(wxString::Format(_("Server job delete: %" PRIu64), resp.job_id), true);
        } else {
            this->writeLog(wxString::Format(_("Server job delete error: %" PRIu64 " - %s"), resp.job_id, resp.error), true);
        }
        this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_JOB_UPDATE) {
        auto payload   = e.GetPayload<QueueItem>();
        auto addedItem = this->qmanager->UpdateItem(payload);

        if (addedItem) {
            if (!this->dataViewListManager->ItemExists(addedItem->id)) {
                this->dataViewListManager->AddItem(addedItem);
                if (this->m_joblist->GetItemCount() > 0 && addedItem->finished_at == 0 && (addedItem->status == QueueStatus::RUNNING || addedItem->status == QueueStatus::PENDING)) {
                    this->m_joblist->EnsureVisible(this->dataViewListManager->RowToDataViewItem(0));
                }
            }
            this->handleSdImages(addedItem);
            this->qmanager->SendEventToMainWindow(QueueEvents::ITEM_UPDATED, addedItem);
            this->UpdateJobImagePreviews(addedItem);
        }
        return;
    }

    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_JOB_IMAGE_UPDATE) {
        auto imageInfo = e.GetPayload<sd_gui_utils::networks::ImageInfo>();
        if (imageInfo.server_id.empty()) {
            return;
        }
        auto jobitem = this->qmanager->GetItemPtr(imageInfo.jobid);
        if (jobitem) {  // trigger the same events like a job update to handle the nem image
            jobitem->SetOrReplaceImageInfo(imageInfo);
            this->handleSdImages(jobitem);
            this->qmanager->SendEventToMainWindow(QueueEvents::ITEM_UPDATED, jobitem);
            this->UpdateJobImagePreviews(jobitem);
        }

        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_AUTH_REQUEST) {
        sd_gui_utils::sdServer* server = e.GetPayload<sd_gui_utils::sdServer*>();
        this->writeLog(wxString::Format(_("Server auth request: %s"), server->GetName()));
        if (server->GetAuthKeyState() == true) {
            server->SendAuthToken();
        } else {
            this->writeLog(wxString::Format(_("Server auth request failed: %s"), server->GetName()));
            this->mapp->cfg->ServerEnable(server->GetInternalId(), false);
        }
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_AUTH_RESPONSE) {
        sd_gui_utils::sdServer* server = e.GetPayload<sd_gui_utils::sdServer*>();
        if (!server) {
            return;
        }

        auto packet_id     = e.GetInt();
        uint64_t client_id = server->GetClientIdFromPacket(packet_id);

        if (client_id > 0) {
            this->mapp->cfg->ServerUpdateClientId(server->GetInternalId(), client_id);
        }
        const std::string server_name = server->GetServerNameFromPacket(packet_id);
        if (!server_name.empty()) {
            this->mapp->cfg->ServerChangeName(server->GetInternalId(), server_name);
            // update the server name in the server list
            for (int i = this->m_server->GetCount() - 1; i > 0; i--) {
                auto srv = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(i));
                if (srv && srv->GetId() == server->GetId()) {
                    this->m_server->SetString(i, srv->GetName());
                    break;
                }
            }
        }
        const std::string server_id = server->GetServerIdFromPacket(packet_id);
        if (server_id.empty() == false) {
            this->mapp->cfg->ServerUpdateId(server->GetInternalId(), server_id);
        }

        this->writeLog(wxString::Format(_("Server auth response: %s"), server->GetName()));
        server->RequestModelList();
        server->RequestJobList();
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SERVER_MODEL_LIST_UPDATE) {
        auto packetId                  = e.GetInt();
        sd_gui_utils::sdServer* server = e.GetPayload<sd_gui_utils::sdServer*>();
        if (server == nullptr) {
            return;
        }
        auto packet = server->GetPacket(packetId);
        if (packet.isValid() == false) {
            this->writeLog("Got invalid packet from server: " + server->GetName());
            return;
        }
        auto last_selected_model = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);

        try {
            auto list = packet.GetData<std::vector<sd_gui_utils::networks::RemoteModelInfo>>();
            for (const auto& item : list) {
                if (item.path.empty() || item.name.empty()) {
                    continue;
                }
                auto newItem = this->ModelManager->addRemoteModel(item, server);
                if (newItem == nullptr) {
                    continue;
                }

                this->modelUiManager->AddItem(newItem, last_selected_model == wxString(newItem->path), server);
            }

            this->writeLog(wxString::Format(_("Server's model list updated: %s Models: %lu"), server->GetName(), list.size()), true);
        } catch (std::exception& e) {
            this->writeLog(wxString::Format(_("Error parsing JSON: %s"), e.what()));
        }

        return;
    }

    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_DONE) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->ModelManager->UpdateInfo(modelinfo);
        this->writeLog(wxString::Format(_("Model civitai image downloaded for model: %s\n"), modelinfo->name));
        // update if the current selected item is the updated item info
        wxTreeListItems selections;
        if (this->m_modelTreeList->GetSelections(selections) > 0) {
            wxTreeListItem item = selections.front();  // get the first
            auto selectedModel  = this->modelUiManager->FindItem(item);
            if (!selectedModel) {
                return;
            }
            if (selectedModel->path == modelinfo->path) {
                this->UpdateModelInfoDetailsFromModelList(modelinfo);
            }
        }
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_MOVE_START || threadEvent == sd_gui_utils::ThreadEvents::MODEL_MOVE_UPDATE) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->modelUiManager->ChangeText(modelinfo->path, wxString::Format(_("Moving: %s%%"), modelinfo->move_progress), 1);
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_MOVE_UPDATE) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->modelUiManager->ChangeText(modelinfo->path, wxString::Format(_("Moving: %s%%"), modelinfo->move_progress), 1);
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_MOVE_FAILED) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->modelUiManager->ChangeText(modelinfo->path, modelinfo->size_f, 1);
        this->writeLog(wxString::Format(_("Model move error: %s\n"), modelinfo->name));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_START) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->ModelManager->UpdateInfo(modelinfo);
        this->writeLog(wxString::Format(_("Model civitai %" PRIuMAX " image(s) download started for model: %s\n"), modelinfo->CivitAiInfo.images.size(), modelinfo->name));
        return;
    }

    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FAILED) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->writeLog(wxString::Format(_("Model civitai info download error: %s\n"), modelinfo->name));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_START) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->writeLog(wxString::Format(_("Model civitai info download start: %s\n"), modelinfo->name));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FINISHED) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->ModelManager->updateCivitAiInfo(modelinfo);

        if (modelinfo->state == sd_gui_utils::CivitAiState::OK) {
            this->writeLog(wxString::Format(_("Model civitai info download finished: %s\n"), modelinfo->CivitAiInfo.name));
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoImageDownload, this, this->GetEventHandler(), modelinfo));
        }
        if (modelinfo->state == sd_gui_utils::CivitAiState::NOT_FOUND) {
            this->writeLog(wxString::Format(_("Model civitai info not found: %s Hash: %s\n"), modelinfo->name, modelinfo->sha256.substr(0, 10)));
        }
        if (modelinfo->state == sd_gui_utils::CivitAiState::ERR) {
            this->writeLog(wxString::Format(_("Model civitai info unkown parsing error happened: %s Hash: %s\n"), modelinfo->name, modelinfo->sha256.substr(0, 10)));
        }
        // update anyway
        this->UpdateModelInfoDetailsFromModelList(modelinfo);
        // update table

        // TODO: add a civitai icon to the model item
        // update table

        return;
    }

    /// status of hashing from modellist
    if (threadEvent == sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS) {
        // modelinfo
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();

        size_t _x            = modelinfo->hash_progress_size;
        size_t _m            = modelinfo->hash_fullsize;
        int current_progress = 0;
        auto _hr1            = sd_gui_utils::formatbytes(_x);
        auto _hr2            = sd_gui_utils::formatbytes(_m);

        if (_m != 0) {
            current_progress = static_cast<int>((_x * 100) / _m);
        }
        this->modelUiManager->UpdateItem(modelinfo);
        this->modelUiManager->ChangeText(modelinfo->path, wxString::Format("%u%% (%.2f %s/%.2f %s)", current_progress, _hr1.first, _hr1.second.c_str(), _hr2.first, _hr2.second.c_str()), 3);
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::STANDALONE_HASHING_DONE) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        if (modelinfo->civitaiPlainJson.empty() && this->mapp->cfg->enable_civitai) {
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
        }
        try {
            nlohmann::json j(*modelinfo);
            std::ofstream file(modelinfo->meta_file);
            file << j;
            file.close();
            modelinfo->hash_progress_size = 0;
            modelinfo->hash_fullsize      = 0;
            this->modelUiManager->ChangeText(modelinfo->path, modelinfo->sha256.substr(0, 10), 3);
        } catch (const std::exception& e) {
            this->writeLog(wxString::Format(_("Error writing model meta file: %s\n"), modelinfo->name));
        }
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::HASHING_PROGRESS) {
        std::shared_ptr<QueueItem> myjob = e.GetPayload<std::shared_ptr<QueueItem>>();

        auto modelinfo = this->ModelManager->getIntoPtr(myjob->params.model_path);
        if (modelinfo != nullptr) {
            modelinfo->hash_progress_size = myjob->hash_progress_size;
            modelinfo->hash_fullsize      = myjob->hash_fullsize;
            this->SendThreadEvent(this->GetEventHandler(), sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
        }

        this->dataViewListManager->UpdateColumn(DataViewListManager::queueJobColumns::PROGRESS, myjob, [myjob]() {
            size_t _x            = myjob->hash_progress_size;
            size_t _m            = myjob->hash_fullsize;
            int current_progress = 0;
            auto _hr1            = sd_gui_utils::formatbytes(_x);
            auto _hr2            = sd_gui_utils::formatbytes(_m);

            if (_m != 0) {
                current_progress = static_cast<int>((_x * 100) / _m);
            }

            return wxVariant(current_progress);
        });
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::HASHING_DONE) {
        std::shared_ptr<QueueItem> myjob = e.GetPayload<std::shared_ptr<QueueItem>>();

        auto modelinfo = this->ModelManager->getIntoPtr(myjob->params.model_path);
        if (modelinfo != nullptr) {
            this->ModelManager->setHash(myjob->params.model_path, myjob->generated_sha256);
            // resend a progress update, because the gui will not receive the event
            this->SendThreadEvent(this->GetEventHandler(), sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
            this->SendThreadEvent(this->GetEventHandler(), sd_gui_utils::ThreadEvents::STANDALONE_HASHING_DONE, modelinfo);
        }

        this->dataViewListManager->UpdateColumn(DataViewListManager::queueJobColumns::PROGRESS, myjob, []() {
            return wxVariant(0);
        });

        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::GENERATION_PROGRESS) {
        std::shared_ptr<QueueItem> myjob = e.GetPayload<std::shared_ptr<QueueItem>>();

        // update column
        auto store      = this->m_joblist->GetStore();
        int progressCol = this->m_joblist->GetColumnCount() - 4;
        int speedCol    = this->m_joblist->GetColumnCount() - 3;

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto currentItem                 = store->GetItem(i);
            auto id                          = store->GetItemData(currentItem);
            std::shared_ptr<QueueItem> qitem = this->qmanager->GetItemPtr(id);
            if (qitem->id == myjob->id) {
                store->SetValueByRow(wxAny(qitem->GetActualProgress()), i, progressCol);
                store->SetValueByRow(qitem->GetActualSpeed(), i, speedCol);
                store->RowValueChanged(i, progressCol);
                store->RowValueChanged(i, speedCol);
                this->m_joblist->Refresh();
                this->m_joblist->Update();
                break;
            }
        }

        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::SD_MESSAGE) {
        if (content.length() < 1) {
            return;
        }
        this->writeLog(wxString::Format("%s", content));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MESSAGE) {
        this->writeLog(wxString::Format("%s\n", content));
        return;
    }
}

void MainWindowUI::OnCivitAiThreadMessage(wxThreadEvent& e) {
    e.Skip();
    auto msg            = e.GetString().utf8_string();
    std::string token   = msg.substr(0, msg.find(":"));
    std::string content = msg.substr(msg.find(":") + 1);

    if (token == "DOWNLOAD_FINISH") {
        auto payload = e.GetPayload<CivitAi::DownloadItem*>();
        if (!payload) {
            return;
        }

        wxString name     = wxFileName(payload->local_file).GetAbsolutePath();
        wxString basepath = this->mapp->cfg->getPathByDirType(payload->dirType);

        auto newinfo = this->ModelManager->addModel(name, payload->dirType, basepath);
        this->modelUiManager->AddItem(newinfo, true);

        auto title   = _("Model download finished");
        auto message = wxString::Format(_("The model download is finished: %s"), name);
        this->ShowNotification(title, message);

        return;
    }
    if (token == "DOWNLOAD_ERROR") {
        auto payload = e.GetPayload<CivitAi::DownloadItem*>();
        auto title   = _("Model download failed");
        auto message = wxString::Format(_("The model download is failed: %s"), std::filesystem::path(payload->local_file).filename().string());
        this->ShowNotification(title, message);
        return;
    }
}

void MainWindowUI::UpdateJobInfoDetailsFromJobQueueList(std::shared_ptr<QueueItem> item) {
    this->m_joblist_item_details->DeleteAllItems();
    if (item == nullptr) {
        for (auto _t : this->jobImagePreviews) {
            _t.second.bitmap->Destroy();
        }
        this->jobImagePreviews.clear();
        return;
    }

    // check if the current selected item is equal with the item from the
    // parameter, so only refresh if it bisible
    auto store = this->m_joblist->GetStore();
    auto row   = this->m_joblist->GetSelectedRow();
    if (row == wxNOT_FOUND) {
        return;
    }
    auto current_item                     = store->GetItem(row);
    auto id                               = store->GetItemData(current_item);
    std::shared_ptr<QueueItem> store_item = this->qmanager->GetItemPtr(id);
    if (store_item != item) {
        return;
    }

    wxVector<wxVariant> data;

    data.push_back(wxVariant(_("ID")));
    data.push_back(wxVariant(wxString::Format("%" PRIu64, item->id)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Created at")));
    data.push_back(wxVariant(sd_gui_utils::formatUnixTimestampToDate(item->created_at)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Started at")));
    if (item->started_at == 0) {
        data.push_back(wxVariant("--"));
    } else {
        data.push_back(wxVariant(sd_gui_utils::formatUnixTimestampToDate(item->started_at)));
    }
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Finished at")));
    if (item->finished_at == 0) {
        data.push_back(wxVariant("--"));
    } else {
        data.push_back(wxVariant(sd_gui_utils::formatUnixTimestampToDate(item->finished_at)));
    }
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Updated at")));
    if (item->updated_at == 0) {
        data.push_back(wxVariant("--"));
    } else {
        data.push_back(wxVariant(sd_gui_utils::formatUnixTimestampToDate(item->updated_at)));
    }
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Server")));
    if (item->server.empty() == false) {
        auto srv = this->mapp->cfg->GetTcpServer(item->server);
        data.push_back(wxVariant(wxString::Format("%s", srv == nullptr ? _("deleted server") : srv->GetName())));
    } else {
        data.push_back(wxVariant("local"));
    }
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    if (item->status_message.empty() == false) {
        data.push_back(wxVariant(_("Status")));
        data.push_back(wxVariant(wxString(item->status_message)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    data.push_back(wxVariant(_("Mode")));
    data.push_back(wxVariant(wxString(modes_str[item->mode])));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Model")));
    data.push_back(wxVariant(wxString(item->model)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    if (item->mode == SDMode::CONVERT) {
        data.push_back(wxVariant(_("Model")));
        data.push_back(wxVariant(wxString(item->params.output_path)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    data.push_back(wxVariant(_("Threads")));
    data.push_back(wxVariant(wxString::Format("%d", item->params.n_threads)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    if (item->mode == SDMode::UPSCALE) {
        data.push_back(wxVariant(_("Factor")));
        data.push_back(
            wxVariant(wxString::Format("%" PRId32, item->upscale_factor)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    if (item->mode == SDMode::IMG2IMG ||
        item->mode == SDMode::TXT2IMG ||
        item->mode == SDMode::CONVERT) {
        data.push_back(wxVariant(_("Type")));
        data.push_back(wxVariant(wxString::Format("%s", sd_gui_utils::sd_type_gui_names[item->params.wtype])));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
        if (item->mode != SDMode::CONVERT) {
            data.push_back(wxVariant(_("Sampler")));
            data.push_back(wxVariant(wxString::Format("%s", sd_gui_utils::samplerUiName.at(item->params.sample_method))));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("Scheduler")));
            data.push_back(wxVariant(sd_gui_utils::sd_scheduler_gui_names[item->params.schedule]));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
    }

    if (item->mode == SDMode::IMG2IMG) {
        data.push_back(wxVariant(_("Init image")));
        data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->initial_image.data(), item->initial_image.size())));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        if (item->mask_image.empty() == false) {
            data.push_back(wxVariant(_("Mask image")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->mask_image.data(), item->mask_image.size())));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }

        data.push_back(wxVariant(_("Strength")));
        data.push_back(wxVariant(wxString::Format("%.2f", item->params.strength)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    if (item->mode == SDMode::TXT2IMG ||
        item->mode == SDMode::IMG2IMG) {
        data.push_back(wxVariant(_("Prompt")));
        data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.prompt)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Neg. prompt")));
        data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.negative_prompt)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Seed")));
        data.push_back(wxVariant(wxString::Format("%" PRId64, item->params.seed)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Clip skip")));
        data.push_back(wxVariant(wxString::Format("%d", item->params.clip_skip)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Clip on CPU")));
        data.push_back(wxVariant(wxString::Format("%s", item->params.clip_on_cpu == true ? _("yes") : _("no"))));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Cfg scale")));
        data.push_back(wxVariant(wxString::Format("%.1f", item->params.cfg_scale)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Steps")));
        data.push_back(wxVariant(wxString::Format("%d", item->params.sample_steps)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        if (item->stats.time_total > 0) {
            data.push_back(wxVariant(_("Time min.")));
            data.push_back(wxVariant(wxString::Format("%.2fs", item->stats.time_min)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("Time max.")));
            data.push_back(wxVariant(wxString::Format("%.2fs", item->stats.time_max)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("Time avg.")));
            data.push_back(wxVariant(wxString::Format("%.2fs", item->stats.time_avg)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("Time total")));
            data.push_back(wxVariant(wxString::Format("%.2fs", item->stats.time_total)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }

        if (item->params.vae_path.empty() == false) {
            wxString vae_path = wxString::FromUTF8Unchecked(item->params.vae_path);
            if (item->server.empty() == false) {
                vae_path = vae_path.SubString(65, -1);
            }
            data.push_back(wxVariant(_("VAE")));
            data.push_back(wxVariant(wxFileName(vae_path).GetFullName()));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }

        data.push_back(wxVariant(_("VAE on CPU")));
        data.push_back(wxVariant(wxString(item->params.vae_on_cpu == true ? _("yes") : _("no"))));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("VAE tiling")));
        data.push_back(wxVariant(wxString(item->params.vae_tiling == true ? _("yes") : _("no"))));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        if (item->params.clip_l_path.empty() == false) {
            data.push_back(wxVariant(_("CLIP L")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.clip_l_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
        if (item->params.clip_g_path.empty() == false) {
            data.push_back(wxVariant(_("CLIP G")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.clip_g_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }

        if (item->params.t5xxl_path.empty() == false) {
            data.push_back(wxVariant(_("T5xxl")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.t5xxl_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }

        if (item->params.taesd_path.empty() == false) {
            wxString taesd_path = wxString::FromUTF8Unchecked(item->params.taesd_path);
            if (item->server.empty() == false) {
                taesd_path = taesd_path.SubString(65, -1);
            }
            data.push_back(wxVariant(_("TAESD")));
            data.push_back(wxVariant(wxFileName(taesd_path).GetFullName()));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
    }

    data.push_back(wxVariant(_("Batch count")));
    data.push_back(wxVariant(wxString::Format("%d", item->params.batch_count)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    if (item->mode == SDMode::TXT2IMG) {
        data.push_back(wxVariant(_("Width")));
        data.push_back(wxVariant(wxString::Format("%dpx", item->params.width)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Height")));
        data.push_back(wxVariant(wxString::Format("%dpx", item->params.height)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        if (!item->params.controlnet_path.empty()) {
            data.push_back(wxVariant(_("CN model")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(wxFileName(item->params.controlnet_path).GetFullName())));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("CN img")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(wxFileName(item->params.control_image_path).GetFullName())));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("CN strength")));
            data.push_back(wxVariant(wxString::Format("%.2f", item->params.control_strength)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("CN on CPU")));
            data.push_back(wxVariant(wxString::Format("%s", item->params.control_net_cpu ? _("yes") : _("no"))));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }

        if (item->params.diffusion_model_path.empty() == false) {
            data.push_back(wxVariant(_("Flash Attention")));
            data.push_back(wxVariant(wxString::Format("%s", item->params.diffusion_flash_attn ? _("yes") : _("no"))));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("SLG scale")));
            data.push_back(wxVariant(wxString::Format("%.2f", item->params.slg_scale)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("Skip layers")));
            wxString skipLayers = "[";
            for (const auto i : item->params.skip_layers) {
                skipLayers += wxString::Format(wxT("%d,"), (int)i);
            }
            skipLayers = skipLayers.SubString(0, skipLayers.Length() - 2);
            skipLayers += "]";
            data.push_back(wxVariant(skipLayers));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("Skip Layer Start")));
            data.push_back(wxVariant(wxString::Format("%.2f", item->params.skip_layer_start)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("Skip Layer End")));
            data.push_back(wxVariant(wxString::Format("%.2f", item->params.skip_layer_end)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
    }
    this->UpdateJobImagePreviews(item);
}

void MainWindowUI::UpdateJobImagePreviews(std::shared_ptr<QueueItem> item) {
    if (item == nullptr) {
        for (auto& [id, preview] : this->jobImagePreviews) {
            if (preview.bitmap) {
                preview.bitmap->Destroy();
            }
        }
        this->jobImagePreviews.clear();
        return;
    }
    if (this->m_joblist->GetSelectedItemsCount() == 0) {
        return;
    }
    auto selected_row = this->m_joblist->GetSelectedRow();
    if (selected_row != wxNOT_FOUND) {
        auto selected_item = this->dataViewListManager->GetQueueItem(selected_row);
        if (item && item->id != selected_item->id) {
            return;
        }
    }

    if (item->image_info.empty()) {
        for (auto& [id, preview] : this->jobImagePreviews) {
            if (preview.bitmap) {
                preview.bitmap->Destroy();
            }
        }
        this->jobImagePreviews.clear();
        return;
    }

    int index = 0;
    for (const auto& img : item->image_info) {
        wxString tooltip = wxString::Format(_("Image width: %dpx, height: %dpx"), img.width, img.height);

        if (isDEBUG) {
            tooltip.Append("\n");
            tooltip.Append(wxString::Format("ID: %s", img.GetId()));
            tooltip.Append("\n");
            tooltip.Append(wxString::Format("Path: %s", img.target_filename));
            tooltip.Append("\n");
            tooltip.Append(wxString::Format("MD5: %s", img.md5_hash));
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::CONTROLNET)) {
            tooltip.Append("\n");
            tooltip.Append(_("Controlnet image"));
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::INITIAL)) {
            tooltip.Append("\n");
            tooltip.Append(_("Initial image"));
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::MASK_INPAINT)) {
            tooltip.Append("\n");
            tooltip.Append(_("Inpaint mask image"));
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::MASK_OUTPAINT)) {
            tooltip.Append("\n");
            tooltip.Append(_("Outpaint mask image"));
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::MASK)) {
            tooltip.Append("\n");
            tooltip.Append(_("Mask image"));
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::ORIGINAL)) {
            tooltip.Append("\n");
            tooltip.Append(_("Original image"));
        }
        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::GENERATED)) {
            tooltip.Append("\n");
            tooltip.Append(_("Generated image"));
        }
        tooltip.Append("\n");
        tooltip.Append(wxString::Format(_("Filename: %s"), wxFileName(img.target_filename).GetFullName()));

        // generate a local unique identifier for the image
        const std::string image_luid = img.GetId();

        if (this->jobImagePreviews.contains(image_luid)) {
            auto& preview = this->jobImagePreviews[image_luid];
            if (preview.is_ok) {
                continue;
            }

            if (preview.need_to_download && wxFileExists(img.target_filename)) {
                auto resized = sd_gui_utils::cropResizeImage(
                    wxString::FromUTF8Unchecked(img.target_filename), 256, 256,
                    wxColour(51, 51, 51, wxALPHA_TRANSPARENT),
                    wxString::FromUTF8Unchecked(this->mapp->cfg->thumbs_path));
                preview.bitmap->SetBitmap(resized);
                preview.bitmap->SetToolTip(tooltip);
                preview.is_ok              = true;
                preview.need_to_download   = false;
                preview.download_requested = false;
                continue;
            }

            if (preview.need_to_download && !preview.download_requested) {
                preview.bitmap->SetBitmap(cloud_download_png_to_wx_bitmap());
                preview.bitmap->SetToolTip(tooltip);
                preview.download_requested = true;

                if (!this->mapp->cfg->remote_download_images_immediately) {
                    auto srv = this->mapp->cfg->GetTcpServer(item->server);
                    if (srv) {
                        srv->RequestImages(item->id, img.GetId());
                        item->image_download_requested = true;
                    }
                }
                continue;
            }
        } else {
            wxStaticBitmap* bitmap = nullptr;
            if (!wxFileExists(img.target_filename)) {
                bitmap = new wxStaticBitmap(this->m_scrolledWindow41, wxID_ANY, !item->server.empty() ? cloud_download_png_to_wx_bitmap() : deleted_png_to_wx_bitmap(), wxDefaultPosition, wxDefaultSize, 0);

                this->jobImagePreviews[image_luid] = {bitmap,
                                                      (item->server.empty() == false),
                                                      true,
                                                      false,
                                                      false,
                                                      img.GetId()};

                if (!this->mapp->cfg->remote_download_images_immediately && !img.server_id.empty()) {
                    auto srv = this->mapp->cfg->GetTcpServer(img.server_id);
                    if (srv) {
                        srv->RequestImages(item->id, img.GetId());
                        this->jobImagePreviews[image_luid].download_requested = true;
                        item->image_download_requested                        = true;
                    }
                }
            } else {
                auto resized = sd_gui_utils::cropResizeImage(
                    wxString::FromUTF8Unchecked(img.target_filename), 256, 256,
                    wxColour(51, 51, 51, wxALPHA_TRANSPARENT),
                    wxString::FromUTF8Unchecked(this->mapp->cfg->thumbs_path));
                bitmap                             = new wxStaticBitmap(this->m_scrolledWindow41, wxID_ANY, resized, wxDefaultPosition, wxDefaultSize, 0);
                this->jobImagePreviews[image_luid] = {bitmap,
                                                      false,
                                                      false,
                                                      true,
                                                      false,
                                                      img.GetId()};
            }

            bitmap->SetToolTip(tooltip);
            this->bSizer8911->Add(bitmap, 0, wxALL, 2);
            this->AddContextMenuToJobImagePreview(bitmap, item, img, index);
        }

        if (sd_gui_utils::hasImageType(img.type, sd_gui_utils::ImageType::GENERATED)) {
            index++;
        }
    }

    for (auto it = this->jobImagePreviews.begin(); it != this->jobImagePreviews.end();) {
        if (std::none_of(item->image_info.begin(), item->image_info.end(),
                         [&it](const auto& img) { return img.GetId() == it->second.imgid; })) {
            if (it->second.bitmap) {
                it->second.bitmap->Destroy();
            }
            it = this->jobImagePreviews.erase(it);
        } else {
            ++it;
        }
    }

    this->m_scrolledWindow41->FitInside();
}

void MainWindowUI::AddContextMenuToJobImagePreview(wxStaticBitmap* bitmap, std::shared_ptr<QueueItem> item, const sd_gui_utils::ImageInfo img, int gen_index) {
    bitmap->Bind(wxEVT_LEFT_DCLICK, [img](wxMouseEvent& event) {
        wxLaunchDefaultApplication(wxString::FromUTF8Unchecked(img.target_filename));
        event.Skip();
    });
    // rightclick
    bitmap->Bind(wxEVT_RIGHT_UP, [img, bitmap, item, this, gen_index](wxMouseEvent& event) {
        event.Skip();
        wxMenu* menu = new wxMenu();
        // menu->Append(99,tooltip);
        // menu->Enable(99,false);
        menu->Append(6, _("Open image"));
        menu->Append(7, _("Open parent folder"));
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::GENERATED)) {
            menu->AppendSeparator();
            menu->Append(0, wxString::Format(_("Copy seed ") + "%" PRId64, item->params.seed + gen_index));
            menu->Append(1, _("Copy prompts to text2img"));
            menu->Append(2, _("Copy prompts to img2img"));
            menu->Append(3, _("Send the image to img2img"));
            menu->Append(5, _("Upscale"));
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::CONTROLNET)) {
            menu->AppendSeparator();
            menu->Append(8, _("Send image to the controlnet image"));
        }

        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::INITIAL) ||
            sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::ORIGINAL)) {
            menu->Append(3, _("Send the image to img2img"));
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::MASK)) {
            menu->AppendSeparator();
            menu->Append(9, _("Send image to img2img mask"));
        }

        menu->AppendSeparator();
        menu->Append(10, _("Copy image to clipboard"));
        menu->Append(11, _("Copy image path to clipboard"));

        menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, img, item, gen_index](wxCommandEvent& evt) {
            evt.Skip();
            auto id = evt.GetId();
            switch (id) {
                case 0: {
                    this->m_seed->SetValue(item->params.seed + gen_index);
                } break;
                case 1: {
                    this->m_prompt->SetValue(wxString::FromUTF8Unchecked(item->params.prompt));
                    this->m_neg_prompt->SetValue(wxString::FromUTF8Unchecked(item->params.negative_prompt));
                } break;
                case 2: {
                    this->m_prompt2->SetValue(wxString::FromUTF8Unchecked(item->params.prompt));
                    this->m_neg_prompt2->SetValue(wxString::FromUTF8Unchecked(item->params.negative_prompt));
                } break;
                case 3: {
                    this->onimg2ImgImageOpen(wxString::FromUTF8Unchecked(img.target_filename), true);
                    this->m_notebook1302->SetSelection(+sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG);
                } break;
                case 5: {
                    this->onUpscaleImageOpen(wxString::FromUTF8Unchecked(img.target_filename));
                    this->m_notebook1302->SetSelection(+sd_gui_utils::GuiMainPanels::PANEL_UPSCALER);
                } break;
                case 6: {
                    wxLaunchDefaultApplication(wxString::FromUTF8Unchecked(img.target_filename));
                } break;
                case 7: {
                    wxLaunchDefaultApplication(wxString::FromUTF8Unchecked(std::filesystem::path(img.target_filename).parent_path().string()));
                } break;
                case 8: {
                    this->onControlnetImageOpen(wxString::FromUTF8Unchecked(img.target_filename));
                } break;
                case 9: {
                    this->m_inpaintOpenMask->SetPath(wxString::FromUTF8Unchecked(img.target_filename));
                    // i never used like this, but trigger the event:
                    wxFileDirPickerEvent event(wxEVT_FILEPICKER_CHANGED, this->m_inpaintOpenMask, this->m_inpaintOpenMask->GetId(), this->m_inpaintOpenMask->GetPath());
                    event.SetEventObject(this->m_inpaintOpenMask);
                    this->m_inpaintOpenMask->GetEventHandler()->ProcessEvent(event);
                } break;
                case 10: {
                    if (wxTheClipboard->Open()) {
                        wxTheClipboard->SetData(new wxBitmapDataObject(wxBitmap(img.target_filename, wxBITMAP_TYPE_ANY)));
                        wxTheClipboard->Close();
                    }
                } break;
                case 11: {
                    if (wxTheClipboard->Open()) {
                        wxTheClipboard->SetData(new wxTextDataObject(wxString::FromUTF8Unchecked(img.target_filename)));
                        wxTheClipboard->Close();
                    }
                } break;

                default: {
                    return;
                } break;
            }
        });
        bitmap->PopupMenu(menu);
        delete menu;
    });
}
std::shared_ptr<QueueItem> MainWindowUI::handleSdImages(std::shared_ptr<QueueItem> item) {
    std::lock_guard<std::mutex> lock(this->qmanager->GetMutex());
    wxString extension = ".jpg";
    auto imgHandler    = wxBITMAP_TYPE_JPEG;

    if (this->mapp->cfg->image_type == sd_gui_utils::imageTypes::PNG) {
        extension  = ".png";
        imgHandler = wxBITMAP_TYPE_PNG;
    }
    if (this->mapp->cfg->image_type == sd_gui_utils::imageTypes::JPG) {
        extension  = ".jpg";
        imgHandler = wxBITMAP_TYPE_JPEG;
    }
    if (isDEBUG) {
        std::cout << "handleSdImages: image format: " << extension << std::endl;
    }

    wxString server_name = wxEmptyString;
    if (!item->server.empty()) {
        auto srv = this->mapp->cfg->GetTcpServer(item->server);
        if (srv) {
            server_name = srv->GetName();
        }
    }
    if (isDEBUG) {
        std::cout << "handleSdImages: server_name: " << server_name << std::endl;
    }

    const auto baseFileName = sd_gui_utils::formatFileName(*item, this->mapp->cfg->output_filename_format, server_name);

    if (isDEBUG) {
        std::cout << "handleSdImages: baseFileName: " << baseFileName << std::endl;
    }
    wxString baseFullName = sd_gui_utils::CreateFilePath(baseFileName, extension, wxString::FromUTF8Unchecked(this->mapp->cfg->output));
    if (isDEBUG) {
        std::cout << "handleSdImages: baseFullName: " << baseFullName << std::endl;
    }
    std::vector<sd_gui_utils::networks::ImageInfo> needExif;
    // the raw images are the generated images. Only presents when the generation is done without errors
    for (const auto& rimage : item->rawImages) {
        sd_gui_utils::networks::ImageInfo rawImage;
        rawImage.target_filename = rimage;
        rawImage.md5_hash        = sd_gui_utils::calculateMD5(rimage);
        rawImage.width           = 0;
        rawImage.height          = 0;
        rawImage.type            = sd_gui_utils::networks::ImageType::GENERATED | sd_gui_utils::networks::ImageType::MOVEABLE;
        item->image_info.push_back(rawImage);
        needExif.push_back(rawImage);
        if (isDEBUG) {
            std::cout << "handleSdImages: rawImage: " << rawImage.target_filename << std::endl;
        }
    }
    // count generated images
    int generated_images = 0;
    for (const auto& img : item->image_info) {
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::GENERATED)) {
            generated_images++;
        }
    }
    // handle other images, this images come from the img2img or upscale
    int index = 0;
    for (auto& img : item->image_info) {
        auto currentimgHandler = imgHandler;
        wxString ext           = extension;
        ext.Replace(".", "");
        wxString nameSuffix = "";
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::MASK_INPAINT)) {
            nameSuffix        = "_inpaint_mask";
            currentimgHandler = wxBITMAP_TYPE_PNG;
            ext               = "png";
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::MASK_OUTPAINT)) {
            nameSuffix        = "_outpaint_mask";
            currentimgHandler = wxBITMAP_TYPE_PNG;
            ext               = "png";
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::INITIAL)) {
            nameSuffix        = "_initial";
            currentimgHandler = imgHandler;
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::ORIGINAL)) {
            nameSuffix        = "_original";
            currentimgHandler = imgHandler;
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::CONTROLNET)) {
            nameSuffix        = "_control";
            currentimgHandler = wxBITMAP_TYPE_PNG;
            ext               = "png";
        }
        if (nameSuffix.empty()) {
            currentimgHandler = imgHandler;
        }
        if (isDEBUG) {
            std::cout << "handleSdImages: image info img.target_filename: " << img.target_filename << std::endl;
        }
        // wxString fullName = sd_gui_utils::CreateFilePath(baseFileName, extension, wxString::FromUTF8Unchecked(this->mapp->cfg->output));
        wxFileName newTargetName(baseFullName);
        newTargetName.SetExt(ext);
        newTargetName.SetName(newTargetName.GetName() + nameSuffix);
        if (generated_images > 1) {
            wxString _name = wxString::Format("%s_%d", newTargetName.GetName(), index);
            newTargetName.SetName(_name);
        }
        wxFileName origName(img.target_filename);

        if (sd_gui_utils::networks::hasImageType(sd_gui_utils::networks::ImageHandleFlags::MOVEABLE_FLAG, img.type)) {
            if (wxFileExists(img.target_filename) && !wxFileExists(newTargetName.GetAbsolutePath())) {
                if (isDEBUG) {
                    std::cout << "handleSdImages: moving image: " << img.target_filename << " to " << newTargetName.GetAbsolutePath() << std::endl;
                }
                wxImage tmp;
                tmp.LoadFile(origName.GetAbsolutePath());
                tmp.SetOption(wxIMAGE_OPTION_FILENAME, newTargetName.GetName());
                if (currentimgHandler == wxBITMAP_TYPE_PNG) {
                    tmp.SetOption(wxIMAGE_OPTION_COMPRESSION, this->mapp->cfg->png_compression_level);  // set the compression from the settings
                }
                if (currentimgHandler == wxBITMAP_TYPE_JPEG) {
                    tmp.SetOption(wxIMAGE_OPTION_QUALITY, this->mapp->cfg->image_quality);
                }
                if (tmp.SaveFile(newTargetName.GetAbsolutePath(), currentimgHandler)) {
                    wxRemove(origName.GetAbsolutePath());
                    auto ts = static_cast<time_t>(item->finished_at == 0 ? item->created_at : item->finished_at);
                    wxDateTime timestamp;
                    timestamp.Set(ts);
                    newTargetName.SetTimes(&timestamp, &timestamp, &timestamp);
                    img.width  = tmp.GetWidth();
                    img.height = tmp.GetHeight();
                }
            }
        }

        if (sd_gui_utils::networks::hasImageType(sd_gui_utils::networks::ImageHandleFlags::COPYABLE_FLAG, img.type)) {
            if (wxFileExists(img.target_filename) && !wxFileExists(newTargetName.GetAbsolutePath())) {
                if (isDEBUG) {
                    std::cout << "handleSdImages: copying image: " << img.target_filename << " to " << newTargetName.GetAbsolutePath() << std::endl;
                }
                wxImage tmp;
                tmp.LoadFile(origName.GetAbsolutePath());
                tmp.SetOption(wxIMAGE_OPTION_FILENAME, newTargetName.GetName());
                if (currentimgHandler == wxBITMAP_TYPE_PNG) {
                    tmp.SetOption(wxIMAGE_OPTION_COMPRESSION, this->mapp->cfg->png_compression_level);  // set the compression from the settings
                }
                if (currentimgHandler == wxBITMAP_TYPE_JPEG) {
                    tmp.SetOption(wxIMAGE_OPTION_QUALITY, this->mapp->cfg->image_quality);
                }
                if (tmp.SaveFile(newTargetName.GetAbsolutePath(), currentimgHandler)) {
                    auto ts = static_cast<time_t>(item->finished_at == 0 ? item->created_at : item->finished_at);
                    wxDateTime timestamp;
                    timestamp.Set(ts);
                    newTargetName.SetTimes(&timestamp, &timestamp, &timestamp);
                    img.width  = tmp.GetWidth();
                    img.height = tmp.GetHeight();
                }
                // wxCopyFile(img.target_filename, newImage.GetAbsolutePath());
            }
        }

        img.target_filename = newTargetName.GetAbsolutePath();
        // update the item's paths too
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::MASK_USED)) {
            item->mask_image = newTargetName.GetAbsolutePath();
            if (isDEBUG) {
                std::cout << "handleSdImages: updating mask_image: " << item->mask_image << std::endl;
            }
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::INITIAL)) {
            item->initial_image = newTargetName.GetAbsolutePath();
            if (isDEBUG) {
                std::cout << "handleSdImages: updating initial_image: " << item->initial_image << std::endl;
            }
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::CONTROLNET)) {
            item->params.control_image_path = newTargetName.GetAbsolutePath();
            if (isDEBUG) {
                std::cout << "handleSdImages: updating control_image_path: " << item->params.control_image_path << std::endl;
            }
        }
        if (sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::GENERATED)) {
            needExif.emplace_back(img);
            index++;
        }
    }

    for (const auto& img : needExif) {
        if (wxFileExists(img.target_filename) && sd_gui_utils::networks::hasImageType(img.type, sd_gui_utils::networks::ImageType::GENERATED)) {
            if (isDEBUG) {
                std::cout << "handleSdImages: updating exif: " << img.target_filename << std::endl;
            }

            if (this->mapp->cfg->image_type == sd_gui_utils::imageTypes::JPG || this->mapp->cfg->image_type == sd_gui_utils::imageTypes::PNG) {
                std::string comment = this->paramsToImageComment(*item).utf8_string();

                if (this->mapp->cfg->image_type == sd_gui_utils::imageTypes::PNG) {
                    std::unordered_map<wxString, wxString> _pngData = {
                        {"parameters", comment},
                        {"Software", EXIF_SOFTWARE}};
                    sd_gui_utils::WriteMetadata(img.target_filename, _pngData, true);
                    return item;
                }
                try {
                    auto image = Exiv2::ImageFactory::open(img.target_filename);
                    image->readMetadata();
                    Exiv2::ExifData& exifData          = image->exifData();
                    exifData["Exif.Photo.UserComment"] = comment.c_str();
                    exifData["Exif.Image.Software"]    = EXIF_SOFTWARE;
                    exifData["Exif.Image.ImageWidth"]  = image->pixelWidth();
                    exifData["Exif.Image.ImageLength"] = image->pixelHeight();

                    if (item->finished_at > 0) {
                        time_t finishedAt = item->finished_at;
                        std::tm* timeinfo = std::localtime(&finishedAt);
                        char dtimeBuffer[20];
                        std::strftime(dtimeBuffer, sizeof(dtimeBuffer), "%Y:%m:%d %H:%M:%S", timeinfo);
                        exifData["Exif.Image.DateTime"] = dtimeBuffer;
                    }
                    Exiv2::XmpData& xmpData       = image->xmpData();
                    xmpData["Xmp.dc.description"] = comment.c_str();

                    image->setExifData(exifData);
                    image->setXmpData(xmpData);
                    image->writeMetadata();
                } catch (Exiv2::Error& e) {
                    std::cerr << "Err: " << e.what() << std::endl;
                }
            }
            //
        }
    }
    return item;
}

void MainWindowUI::ModelHashingCallback(size_t readed_size, std::string sha256, void* custom_pointer) {
    sd_gui_utils::VoidHolder* holder       = static_cast<sd_gui_utils::VoidHolder*>(custom_pointer);
    wxEvtHandler* eventHandler             = (wxEvtHandler*)holder->p1;
    std::shared_ptr<QueueItem> myItem      = std::shared_ptr<QueueItem>(static_cast<QueueItem*>(holder->p2));
    sd_gui_utils::ModelFileInfo* modelinfo = (sd_gui_utils::ModelFileInfo*)holder->p3;
    modelinfo->hash_progress_size          = readed_size;
    modelinfo->hash_fullsize               = myItem->hash_fullsize;
    myItem->hash_progress_size             = readed_size;

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::HASHING_PROGRESS, myItem);

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
}

void MainWindowUI::ShowNotification(const wxString& title, const wxString& message) {
    if (this->mapp->cfg->show_notifications) {
        wxNotificationMessage notification(title, message, this);
        notification.SetTitle(title);
#if defined(_WIN64) || defined(_WIN32) || defined(WIN32)
        notification.UseTaskBarIcon(this->TaskBar);
#endif

        notification.Show(this->mapp->cfg->notification_timeout);
    }

    this->TaskBar->SetIcon(this->TaskBarIcon, wxString::Format("%s - %s", this->GetTitle(), title));
}
void MainWindowUI::ShowNotification(std::shared_ptr<QueueItem> jobItem) {
    if (!this->lastItemNotification.changed(jobItem->status, jobItem->id)) {
        return;
    }

    wxString status_str = wxGetTranslation(QueueStatus_GUI_str.at(jobItem->status));
    wxString message    = _("Job ");
    message.Append(wxString::Format("%" PRIu64, jobItem->id));

    wxString mode_str = sd_gui_utils::SDModeGUINames.at(jobItem->mode);
    if (jobItem->mask_image.empty() == false) {
        mode_str += wxString::Format(_(" (INPAINTING)"));
    }
    wxString title = wxString::Format("%s %s", mode_str, status_str);

    if (jobItem->params.batch_count > 1) {
        message.Append(wxString::Format("\nBatch: %d", jobItem->params.batch_count));
    }

    if (jobItem->model.empty() == false) {
        message.Append("\n");
        message.Append(wxString::Format(_("Model: %s"), jobItem->model));
    }
    if (jobItem->params.vae_path.empty() == false) {
        message.Append("\n");
        message.Append(wxString::Format(_("VAE: %s"), wxFileName(jobItem->params.vae_path).GetFullName()));
    }
    if (jobItem->params.taesd_path.empty() == false) {
        message.Append("\n");
        message.Append(wxString::Format(_("TAESD: %s"), wxFileName(jobItem->params.taesd_path).GetFullName()));
    }

    if (jobItem->status == QueueStatus::HASHING) {
    }

    if (jobItem->status == QueueStatus::FAILED && !jobItem->status_message.empty()) {
        message.Append(wxString::Format("\n%s", wxString::FromUTF8Unchecked(jobItem->status_message)));
    }

    if (!jobItem->server.empty()) {
        auto srv = this->mapp->cfg->GetTcpServer(jobItem->server);
        if (srv) {
            message.Append("\n");
            message.Append(wxString::Format(_("Server: %s"), srv->GetName()));
        }
    }

    this->ShowNotification(title, message);
    this->lastItemNotification.update(jobItem->status, jobItem->id);
}

void MainWindowUI::onControlnetImageOpen(const wxString& file) {
    if (file.empty()) {
        std::cerr << "Empty file path on controlnet image load... " << std::endl;
    }
    wxImage img;
    if (img.LoadFile(file)) {
        this->m_controlnetImageOpen->SetPath(file);
        auto origSize = this->m_controlnetImagePreview->GetSize();
        auto preview  = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(), origSize.GetHeight());

        this->m_controlnetImagePreview->SetScaleMode(wxStaticBitmap::Scale_AspectFill);
        this->m_controlnetImagePreview->SetBitmap(preview);
        this->m_controlnetImagePreview->SetSize(origSize);
        this->m_controlnetImagePreviewButton->Enable();
        this->m_controlnetImageDelete->Enable();
        this->m_width->SetValue(wxString::Format(wxT("%i"), img.GetWidth()));
        this->m_height->SetValue(wxString::Format(wxT("%i"), img.GetHeight()));
        // can not change the outpt images resolution
        this->m_width->Disable();
        this->m_height->Disable();
        this->m_button7->Disable();  // swap resolution
    }
}

void MainWindowUI::LoadPresets() {
    this->LoadFileList(sd_gui_utils::DirTypes::PRESETS);
}
void MainWindowUI::LoadPromptTemplates() {
    this->LoadFileList(sd_gui_utils::DirTypes::PROMPT_TEMPLATES);
}

void MainWindowUI::ChangeModelByInfo(sd_gui_utils::ModelFileInfo* info) {
    if (info == nullptr) {
        return;
    }
    for (unsigned int _z = 1; _z < this->m_model->GetCount(); _z++) {
        sd_gui_utils::ModelFileInfo* m = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_model->GetClientData(_z));
        if (m == nullptr) {
            continue;
        }
        if (info->path == m->path && info->server_id == m->server_id) {
            this->m_model->SetSelection(_z);
            if (m->server_id.empty()) {
                this->m_server->SetSelection(0);
            } else {
                for (int i = this->m_server->GetCount() - 1; i > 0; i--) {
                    auto srv = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(i));
                    if (srv && srv->GetId() == m->server_id) {
                        this->m_server->SetSelection(i);
                        break;
                    }
                }
            }
            return;
        }
    }
    if (this->mapp->cfg->favorite_models_only == true) {
        wxString name = info->name;
        if (!info->server_id.empty()) {
            auto srv = this->mapp->cfg->GetTcpServer(info->server_id);
            if (srv != nullptr) {
                name.Prepend(srv->GetName() + ": ");
            }

            for (int i = this->m_server->GetCount() - 1; i > 0; i--) {
                auto srv = static_cast<sd_gui_utils::sdServer*>(this->m_server->GetClientData(i));
                if (srv && srv->GetId() == info->server_id) {
                    this->m_server->SetSelection(i);
                    break;
                }
            }
        } else {
            this->m_server->SetSelection(0);
        }
        this->m_model->Append(name, info);
        this->m_model->SetSelection(this->m_model->GetCount() - 1);
    }
}

void MainWindowUI::loadModelList() {
    this->LoadFileList(sd_gui_utils::DirTypes::CHECKPOINT);
    this->LoadFileList(sd_gui_utils::DirTypes::LLAMA_MODELS);

    auto lastSelection = this->mapp->config->Read("/model_list/last_selected_model", wxEmptyString);
    this->modelUiManager->SelectItemByModelPath(lastSelection.utf8_string());
}

void MainWindowUI::OnExit(wxEvent& event) {
    this->Close();
}

void MainWindowUI::threadedModelHashCalc(wxEvtHandler* eventHandler, sd_gui_utils::ModelFileInfo* modelinfo) {
    modelinfo->hash_fullsize = modelinfo->size;

    sd_gui_utils::VoidHolder* holder = new sd_gui_utils::VoidHolder;
    holder->p1                       = (void*)eventHandler;
    holder->p2                       = (void*)modelinfo;

    modelinfo->sha256 = sd_gui_utils::sha256_file_openssl(modelinfo->path.c_str(), (void*)holder, &MainWindowUI::ModelStandaloneHashingCallback);
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_DONE, modelinfo);
}

void MainWindowUI::threadedModelInfoDownload(wxEvtHandler* eventHandler, sd_gui_utils::ModelFileInfo* modelinfo) {
    // there is no hash, start hashing
    if (modelinfo->sha256.empty()) {
        modelinfo->hash_fullsize = modelinfo->size;

        sd_gui_utils::VoidHolder* holder = new sd_gui_utils::VoidHolder;
        holder->p1                       = (void*)eventHandler;
        holder->p2                       = (void*)modelinfo;

        modelinfo->sha256 = sd_gui_utils::sha256_file_openssl(modelinfo->path.c_str(), (void*)holder, &MainWindowUI::ModelStandaloneHashingCallback);
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_DONE, modelinfo);
        // the info download will start automatically if hash is done
        return;
    }
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_START, modelinfo);

    // get apikey from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.Load(wxString::Format(wxT("%s/CivitAiApiKey"), PROJECT_NAME), username, password)) {
        apikey = password.GetAsString();
    }

    std::string url      = "https://civitai.com/api/v1/model-versions/by-hash/" + modelinfo->sha256.substr(0, 10);
    std::string response = "";

    sd_gui_utils::SimpleCurl request;

    std::vector<std::string> headers = {
        "Content-Type: application/json",
        "User-Agent: " + std::string(SD_CURL_USER_AGENT)};

    if (!apikey.empty()) {
        headers.emplace_back("Authorization: Bearer " + apikey.utf8_string());
    }

    try {
        request.get(url, headers, response);

        modelinfo->civitaiPlainJson = response;
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FINISHED, modelinfo);
        return;
    } catch (const std::exception& e) {
        std::cerr << __FILE__ << ":" << __LINE__ << " " << e.what() << std::endl;
        wxMessageDialog dialog(this, e.what());
        modelinfo->civitaiPlainJson = response;
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FAILED, modelinfo);
        return;
    }
    modelinfo->civitaiPlainJson = response;
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FAILED, modelinfo);
}

void MainWindowUI::threadedModelInfoImageDownload(
    wxEvtHandler* eventHandler,
    sd_gui_utils::ModelFileInfo* modelinfo) {
    if (modelinfo->civitaiPlainJson.empty()) {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, std::string("No model info found."));
        return;
    }

    if (modelinfo->CivitAiInfo.images.empty()) {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, "No images found.");
        return;
    }

    // Remove old files
    for (const std::string& old_img : modelinfo->preview_images) {
        if (std::filesystem::exists(old_img)) {
            std::filesystem::remove(old_img);
        }
    }
    modelinfo->preview_images.clear();
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_START, modelinfo);

    // Get API key from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;

    wxSecretStore store = wxSecretStore::GetDefault();
    if (store.Load(wxString::Format(wxT("%s/CivitAiApiKey"), PROJECT_NAME), username, password)) {
        apikey = password.GetAsString();
    }

    // Set up headers
    std::vector<std::string> headers;
    headers.push_back("Content-Type: application/json");
    headers.push_back("User-Agent: " + std::string(SD_CURL_USER_AGENT));

    if (!apikey.empty()) {
        headers.push_back("Authorization: Bearer " + apikey.utf8_string());
    }

    // Loop through each image and download
    wxFileName base_path(wxString::Format("%s/CivitAiPreviews/%s/",
                                          this->mapp->cfg->datapath.utf8_string(),
                                          modelinfo->sha256.substr(0, 10)));

    if (wxDirExists(base_path.GetAbsolutePath()) == false) {
        base_path.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    int index = 0;
    for (CivitAi::image& img : modelinfo->CivitAiInfo.images) {
        std::ostringstream response;
        try {
            sd_gui_utils::SimpleCurl curl;
            // oh, because the thumbs, need to add the model's hash to the filename
            wxFileName target_path(wxString::Format("%s/%s%d.tmp", base_path.GetAbsolutePath(), modelinfo->sha256, index));
            curl.getFile(img.url, headers, target_path.GetAbsolutePath().utf8_string());

            // Process downloaded image
            wxImage _tmpImg;
            _tmpImg.SetLoadFlags(_tmpImg.GetLoadFlags() & ~wxImage::Load_Verbose);

            if (_tmpImg.LoadFile(target_path.GetAbsolutePath(), wxBITMAP_TYPE_ANY)) {
                wxFileName new_path = target_path;
                if (_tmpImg.GetType() == wxBITMAP_TYPE_JPEG) {
                    new_path.SetExt("jpg");
                } else if (_tmpImg.GetType() == wxBITMAP_TYPE_PNG) {
                    new_path.SetExt("png");
                }

                wxRenameFile(target_path.GetAbsolutePath(), new_path.GetAbsolutePath());
                modelinfo->preview_images.emplace_back(new_path.GetAbsolutePath().utf8_string());
                img.local_path = new_path.GetAbsolutePath().utf8_string();

                MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_PROGRESS, modelinfo, img.url);
                index++;
            }
            _tmpImg.Destroy();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, img.url);
        }
    }

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_DONE, modelinfo);
}

void MainWindowUI::PrepareModelConvert(sd_gui_utils::ModelFileInfo* modelInfo) {
    std::string usingVae;
    const wxFileName modelIn(modelInfo->path);
    wxFileName modelOut(modelInfo->path);
    modelOut.SetExt("gguf");

    std::string newType = this->m_type->GetStringSelection().utf8_string();

    modelOut.SetName(modelOut.GetName() + "_" + newType);
    this->writeLog(wxString::Format(_("Converting model %s to %s"), modelIn.GetFullName(), modelOut.GetFullName()));

    std::shared_ptr<QueueItem> item = std::make_shared<QueueItem>();
    item->model                     = modelInfo->name;
    item->mode                      = SDMode::CONVERT;
    item->params.mode               = SDMode::CONVERT;
    item->params.n_threads          = this->mapp->cfg->n_threads;
    item->params.output_path        = modelOut.GetAbsolutePath().utf8_string();
    item->params.model_path         = modelIn.GetAbsolutePath().utf8_string();

    auto selectedwType = this->m_type->GetStringSelection();
    for (auto types : sd_gui_utils::sd_type_gui_names) {
        if (types.second == selectedwType) {
            item->params.wtype = (sd_type_t)types.first;
            break;
        }
    }

    auto selectedScheduler = this->m_scheduler->GetStringSelection();
    for (auto schedulers : sd_gui_utils::sd_scheduler_gui_names) {
        if (schedulers.second == selectedScheduler) {
            item->params.schedule = schedulers.first;
            break;
        }
    }

    if (this->m_vae->GetCurrentSelection() > 0) {
        auto vaeClientData = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_vae->GetClientData(this->m_vae->GetCurrentSelection()));
        if (vaeClientData != nullptr) {
            usingVae              = vaeClientData->name;
            item->params.vae_path = vaeClientData->path;
        }
    }

    wxString question = wxString::Format(_("Do you want to convert model %s with quantation %s to gguf format?"), modelIn.GetFullName(), newType);

    if (usingVae.empty() == false) {
        question = wxString::Format(_("Do you want to convert model %s with quantation %s and vae %s to gguf format?"), modelIn.GetFullName(), newType, usingVae);
    }

    wxMessageDialog dialog(this, question, wxString::Format(_("Convert model %s?"), modelIn.GetFullName()), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
    if (dialog.ShowModal() == wxID_YES) {
        if (modelOut.FileExists()) {
            wxString overwriteQuestion = wxString::Format(_("The file %s already exists. Do you want to overwrite it?"), modelOut.GetFullName());
            wxMessageDialog overwriteDialog(this, overwriteQuestion, _("Overwrite File?"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
            if (overwriteDialog.ShowModal() != wxID_YES) {
                return;
            }
        }
        this->qmanager->AddItem(item);
    }
}

void MainWindowUI::OnHtmlLinkClicked(wxHtmlLinkEvent& event) {
    // show a dialog with warning
    wxMessageDialog dialog(this, event.GetLinkInfo().GetHref(), _("Open Link?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
    if (dialog.ShowModal() == wxID_YES) {
        wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
    }
}
void MainWindowUI::onWhatIsThis(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("https://github.com/fszontagh/sd.cpp.gui.wx/wiki/LCM-&-SD3.5-&-FLUX-howtos");
}

void MainWindowUI::OnLoadPrevPrompt(wxCommandEvent& event) {
    auto obj = static_cast<wxButton*>(event.GetEventObject());
    switch (static_cast<sd_gui_utils::GuiMainPanels>(this->m_notebook1302->GetSelection())) {
        case sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG: {
            if (obj == this->m_prompt_load_prev) {
                auto last_prompt = this->mapp->config->Read("/last_prompt");
                this->m_prompt->SetValue(last_prompt);
            }
            if (obj == this->m_nprompt_load_prev) {
                auto last_neg_prompt = this->mapp->config->Read("/last_neg_prompt");
                this->m_neg_prompt->SetValue(last_neg_prompt);
            }
        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG: {
            if (obj == this->m_prompt_load_prev) {
                auto last_prompt = this->mapp->config->Read("/last_prompt");
                this->m_prompt2->SetValue(last_prompt);
            }
            if (obj == this->m_nprompt_load_prev) {
                auto last_neg_prompt = this->mapp->config->Read("/last_neg_prompt");
                this->m_neg_prompt2->SetValue(last_neg_prompt);
            }
        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_QUEUE:
        case sd_gui_utils::GuiMainPanels::PANEL_UPSCALER:
        case sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO:
        case sd_gui_utils::GuiMainPanels::PANEL_MODELS:
            break;
    }
}
void MainWindowUI::OnClearPrompt(wxCommandEvent& event) {
    auto obj = static_cast<wxButton*>(event.GetEventObject());
    switch (static_cast<sd_gui_utils::GuiMainPanels>(this->m_notebook1302->GetSelection())) {
        case sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG: {
            if (obj == this->m_prompt_clear) {
                this->m_prompt->SetValue("");
            }
            if (obj == this->m_nprompt_clear) {
                this->m_neg_prompt->SetValue("");
            }
        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG: {
            if (obj == this->m_prompt_clear) {
                this->m_prompt2->SetValue("");
            }
            if (obj == this->m_nprompt_clear) {
                this->m_neg_prompt2->SetValue("");
            }
        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_QUEUE:
        case sd_gui_utils::GuiMainPanels::PANEL_UPSCALER:
        case sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO:
        case sd_gui_utils::GuiMainPanels::PANEL_MODELS:
            break;
    }
}
void MainWindowUI::OnNormalizePrompt(wxCommandEvent& event) {
    auto obj = static_cast<wxButton*>(event.GetEventObject());
    switch (static_cast<sd_gui_utils::GuiMainPanels>(this->m_notebook1302->GetSelection())) {
        case sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG: {
            if (obj == this->m_prompt_normalize) {
                wxString currentValue = this->m_prompt->GetValue();
                NormalizeTextPrompt(currentValue);
                this->m_prompt->SetValue(currentValue);
            }
            if (obj == this->m_nprompt_normalize) {
                wxString currentValue = this->m_neg_prompt->GetValue();
                NormalizeTextPrompt(currentValue);
                this->m_neg_prompt->SetValue(currentValue);
            }
        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG: {
            if (obj == this->m_prompt_normalize) {
                wxString currentValue = this->m_prompt2->GetValue();
                NormalizeTextPrompt(currentValue);
                this->m_prompt2->SetValue(currentValue);
            }
            if (obj == this->m_nprompt_normalize) {
                wxString currentValue = this->m_neg_prompt2->GetValue();
                NormalizeTextPrompt(currentValue);
                this->m_neg_prompt2->SetValue(currentValue);
            }
        } break;
        case sd_gui_utils::GuiMainPanels::PANEL_QUEUE:
        case sd_gui_utils::GuiMainPanels::PANEL_UPSCALER:
        case sd_gui_utils::GuiMainPanels::PANEL_IMAGEINFO:
        case sd_gui_utils::GuiMainPanels::PANEL_MODELS:
            break;
    }
}

bool MainWindowUI::ProcessEventHandler(const std::string& message, const std::string& error) {
    try {
        nlohmann::json msg    = nlohmann::json::parse(message);
        QueueItem updatedItem = msg.get<QueueItem>();

        auto originalItem = this->qmanager->GetItemPtr(updatedItem.id);

        if (originalItem == nullptr) {
            std::cerr << "[GUI] Could not find item " << updatedItem.id << std::endl;
            return true;
        }

        if (originalItem->status == QueueStatus::FAILED) {
            std::cerr << "[GUI] Item " << updatedItem.id << " QueueStatus::FAILED, skipping" << std::endl;
            return true;
        }
        // mutex lock
        std::lock_guard<std::mutex> lock(this->mutex);

        auto oldStatus = originalItem->status;
        if (originalItem->update_index != updatedItem.update_index) {
            //*originalItem = updatedItem;
            originalItem->CopyFromProcess(updatedItem);

            if (!originalItem->rawImages.empty()) {
                this->handleSdImages(originalItem);
            }
            if (originalItem->status == QueueStatus::FAILED) {
                if (error.empty() == false) {
                    originalItem->status_message = error;
                }
            }

            if (oldStatus != originalItem->status) {
                if (originalItem->status == QueueStatus::DONE) {
                    this->qmanager->SendEventToMainWindow(QueueEvents::ITEM_FINISHED, originalItem);
                }
                if (originalItem->status == QueueStatus::FAILED) {
                    this->qmanager->SendEventToMainWindow(QueueEvents::ITEM_FAILED, originalItem);
                }
                if (originalItem->status == QueueStatus::MODEL_FAILED) {
                    this->qmanager->SendEventToMainWindow(QueueEvents::ITEM_MODEL_FAILED, originalItem);
                }
                this->qmanager->SendEventToMainWindow(QueueEvents::ITEM_STATUS_CHANGED, originalItem);
            } else {
                this->qmanager->SendEventToMainWindow(QueueEvents::ITEM_UPDATED, originalItem);
            }
            return true;
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return false;
}

void MainWindowUI::ProcessStdOutEvent(const char* bytes, size_t n) {
    if (bytes == nullptr) {
        return;
    }

    wxString msg = wxString(bytes, n);
    if (msg.empty()) {
        return;
    }

    if (msg.find("PING") != std::string::npos) {
        return;
    }
    msg.Replace("\r\n", "\n");
    msg.Replace("\n\n", "\n");

    wxStringTokenizer tokenizer(msg, "\n");
    while (tokenizer.HasMoreTokens()) {
        wxString line = tokenizer.GetNextToken();
        line.Trim();
        line.Replace("[EXTPROCESS]", "");
        line.Prepend("[EXTPROCESS]: ");
        this->writeLog(line, (line.Find("ERROR") > 0));
    }
}

void MainWindowUI::ProcessStdErrEvent(const char* bytes, size_t n) {
    if (bytes == nullptr) {
        return;
    }

    wxString msg = wxString(bytes, n);
    if (msg.empty()) {
        return;
    }

    if (msg.find("PING") != std::string::npos) {
        return;
    }

    wxStringTokenizer tokenizer(msg, "\n");
    while (tokenizer.HasMoreTokens()) {
        wxString line = tokenizer.GetNextToken();
        line.Trim();
        line.Replace("[EXTPROCESS]", "");
        line.Prepend("[EXTPROCESS]: ");
        this->writeLog(line, (line.Find("ERROR") > 0));
    }
}

void MainWindowUI::initLog() {
    if (logfile.IsOpened() == false) {
        wxFileName fn(this->mapp->cfg->datapath + wxFileName::GetPathSeparator() + "app.log");
        // add another log file for the external process
        wxFileName fn2(fn);
        fn2.SetName("diffuser");
        fn2.SetExt("log");
        this->extProcessLogFile = fn2.GetFullPath();

        wxFileName fn3(fn);
        fn3.SetName("llama");
        fn3.SetExt("log");
        this->llamaLogFile = fn3.GetFullPath();

        if (logfile.Open(fn.GetAbsolutePath(), wxFile::write_append)) {
            logfile.SeekEnd();
            this->writeLog(wxString::Format(_("%s started"), EXIF_SOFTWARE));
        } else {
            std::cerr << "Can not open the logfile: " << this->mapp->cfg->datapath + "/app.log" << std::endl;
        }
    }
}

void MainWindowUI::deInitLog() {
    if (logfile.IsOpened()) {
        this->writeLog(wxString::Format(_("%s exited"), EXIF_SOFTWARE));
        logfile.Flush();
        logfile.Close();
    }
}
void MainWindowUI::writeLog(const wxString& msg, bool writeIntoGui, bool debug) {
    std::time_t now   = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    char timestamp[30];
    wxString message = msg;
    std::strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", timeinfo);

    message.Replace("\n", " ", true);
    message.Replace("\r\n", " ", true);
    message.Replace("  ", " ", true);

    message.Trim();

    if (message.IsEmpty()) {
        return;
    }
    message.Append("\n");
    if (debug) {
        message.Prepend("[DEBUG] ");
    }
    std::lock_guard<std::mutex> lock(this->logMutex);
    wxString logline = wxString::Format("%s: %s", timestamp, message);
    if (logfile.IsOpened()) {
        logfile.Write(logline);
        logfile.Flush();
    }

    if (writeIntoGui) {
        this->logs->AppendText(logline);
    }
}
void MainWindowUI::writeLog(const std::string& message) {
    this->writeLog(wxString::FromUTF8Unchecked(message));
}
void MainWindowUI::UpdateCurrentProgress(std::shared_ptr<QueueItem> item, const QueueEvents& event) {
    // TODO: handle multiple server's multiple status
    this->m_currentStatus->SetLabel(wxString::Format(_("Current job: %s %s"), modes_str[item->mode], wxGetTranslation(QueueStatus_GUI_str.at(item->status))));

    if (event == QueueEvents::ITEM_STATUS_CHANGED) {
        this->ShowNotification(item);
        return;
    }
    // skip update if localqueue running and we got remote update
    if (this->qmanager->IsRunning() && item->server.empty() == false) {
        return;
    }

    if (event == QueueEvents::ITEM_MODEL_HASH_UPDATE || event == QueueEvents::ITEM_MODEL_HASH_START) {
        auto current = item->hash_progress_size;
        auto total   = item->hash_fullsize;
        if (current > total) {
            current = total;
        }
        this->m_currentProgress->SetRange(total);
        this->m_currentProgress->SetValue(current);
        return;
    }

    if (event == QueueEvents::ITEM_UPDATED ||
        event == QueueEvents::ITEM_START ||
        event == QueueEvents::ITEM_FINISHED) {
        unsigned int stepsSum  = item->params.sample_steps * item->params.batch_count;
        unsigned int stepsDone = item->stats.time_per_step.size();

        // upscaler and the tieled vae will generate more steps after diffusion
        if (stepsSum < stepsDone) {
            stepsSum  = item->steps;
            stepsDone = item->step;
        }
        this->m_currentProgress->SetRange(stepsSum);
        this->m_currentProgress->SetValue(stepsDone > stepsSum ? stepsSum : stepsDone);
    }
}

void MainWindowUI::SetSchedulerByType(schedule_t schedule) {
    for (auto i = 0; i < this->m_scheduler->GetCount(); i++) {
        auto clientData = dynamic_cast<sd_gui_utils::IntClientData*>(this->m_scheduler->GetClientObject(i));
        if (clientData == nullptr) {
            continue;
        }

        if (schedule == clientData->getId()) {
            this->m_scheduler->Select(i);
            break;
        }
    }
}
void MainWindowUI::SetSamplerByType(sample_method_t sampler) {
    for (auto i = 0; i < this->m_sampler->GetCount(); i++) {
        auto clientData = dynamic_cast<sd_gui_utils::IntClientData*>(this->m_sampler->GetClientObject(i));
        if (clientData == nullptr) {
            continue;
        }
        if (sampler == clientData->getId()) {
            this->m_sampler->Select(i);
            break;
        }
    }
}

void MainWindowUI::SetTypeByType(sd_type_t type) {
    for (auto i = 0; i < this->m_type->GetCount(); i++) {
        auto clientData = dynamic_cast<sd_gui_utils::IntClientData*>(this->m_type->GetClientObject(i));
        if (clientData == nullptr) {
            continue;
        }
        if (type == clientData->getId()) {
            this->m_type->Select(i);
            break;
        }
    }
}

void MainWindowUI::OnImg2ImgMouseLeave(wxMouseEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    this->inpaintHelper->OnMouseLeave(event);
}

void MainWindowUI::OnInpaintSaveMask(wxCommandEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    wxFileDialog saveFileDialog(this, _("Save mask image file"), "", "",
                                _("PNG files (*.png)|*.png|All files (*.*)|*.*"),
                                wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    wxString filename = saveFileDialog.GetPath();
    if (filename.empty()) {
        return;
    }
    wxFileName file(filename);
    if (file.GetExt().empty()) {
        file.SetExt("png");
    }

    wxImage image = this->inpaintHelper->OnSaveMask(event);

    if (image.HasAlpha()) {
        image.ClearAlpha();
    }
    if (!image.SaveFile(filename, wxBITMAP_TYPE_PNG)) {
        wxLogError(_("Failed to save image into %s"), filename);
        return;
    }
    this->m_inpaintOpenMask->SetPath(file.GetAbsolutePath());
}
void MainWindowUI::OnImg2ImgSize(wxSizeEvent& event) {
    if (this->inpaintHelper != nullptr) {
        this->inpaintHelper->OnSize(event);
    }
    event.Skip();
}
void MainWindowUI::OnInpaintResizeImage(wxCommandEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }

    int targetWidth, targetHeight;

    this->m_width->GetValue().ToInt(&targetWidth);
    this->m_height->GetValue().ToInt(&targetHeight);
    auto resized = this->inpaintHelper->OnResizeOriginalImage(targetWidth, targetHeight);
    this->m_inpaintImageResolution->SetLabel(wxString::Format("%d x %d", resized.GetWidth(), resized.GetHeight()));
    this->m_width->SetValue(wxString::Format(wxT("%i"), resized.GetWidth()));
    this->m_height->SetValue(wxString::Format(wxT("%i"), resized.GetHeight()));
    this->m_inpaintBrushSizeSlider->SetValue(this->inpaintHelper->GetCurrentBrushSize());
    this->m_inpaintZoomSlider->SetValue(this->inpaintHelper->GetZoomFactor() * 100);

    if (this->inpaintHelper->inPaintCanvasEmpty()) {
        this->m_inpaintClearMask->Disable();
    } else {
        this->m_inpaintClearMask->Enable();
    }
}

void MainWindowUI::OnInpaintInvertMask(wxCommandEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    this->inpaintHelper->OnInvertMask(event);
    if (this->inpaintHelper->inPaintCanvasEmpty()) {
        this->m_inpaintClearMask->Disable();
    } else {
        this->m_inpaintClearMask->Enable();
    }
}
void MainWindowUI::OnImg2ImgMouseDown(wxMouseEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    this->inpaintHelper->OnMouseLeftDown(event);
}

void MainWindowUI::OnImg2ImgMouseUp(wxMouseEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    this->inpaintHelper->OnMouseLeftUp(event);
}
void MainWindowUI::OnImg2ImgRMouseDown(wxMouseEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    this->inpaintHelper->OnMouseRightDown(event);
}
void MainWindowUI::OnImg2ImgRMouseUp(wxMouseEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    this->inpaintHelper->OnMouseRightUp(event);
}
void MainWindowUI::OnInpaintCanvasResizeApply(wxCommandEvent& event) {
    if (this->inpaintHelper->inPaintImageLoaded() == false) {
        this->m_inpaintCanvasTop->SetValue("0");
        this->m_inpaintCanvasLeft->SetValue("0");
        this->m_inpaintCanvasRight->SetValue("0");
        this->m_inpaintCanvasBottom->SetValue("0");
        event.Skip();
        return;
    }
    int canvasTop    = wxAtoi(this->m_inpaintCanvasTop->GetValue());
    int canvasLeft   = wxAtoi(this->m_inpaintCanvasLeft->GetValue());
    int canvasRight  = wxAtoi(this->m_inpaintCanvasRight->GetValue());
    int canvasBottom = wxAtoi(this->m_inpaintCanvasBottom->GetValue());
    sd_gui_utils::wxEnlargeImageSizes enlargedSizes{canvasTop, canvasRight, canvasBottom, canvasLeft};
    this->inpaintHelper->OnOutPaintResize(enlargedSizes);

    this->m_inpaintCanvasTop->SetValue(wxString::FromDouble(enlargedSizes.top));
    this->m_inpaintCanvasRight->SetValue(wxString::FromDouble(enlargedSizes.right));
    this->m_inpaintCanvasBottom->SetValue(wxString::FromDouble(enlargedSizes.bottom));
    this->m_inpaintCanvasLeft->SetValue(wxString::FromDouble(enlargedSizes.left));

    this->m_inpaintZoomSlider->SetValue(this->inpaintHelper->GetZoomFactor() * 100);
    this->m_inpaintBrushSizeSlider->SetValue(this->inpaintHelper->GetCurrentBrushSize());
    this->m_width->SetValue(wxString::Format(wxT("%i"), this->inpaintHelper->GetOutPaintedSize().GetWidth()));
    this->m_height->SetValue(wxString::Format(wxT("%i"), this->inpaintHelper->GetOutPaintedSize().GetHeight()));
}
void MainWindowUI::OnInpaintBrushSizeSliderScroll(wxScrollEvent& event) {
    this->inpaintHelper->SetCurrentBrushSize(event.GetPosition());
}
void MainWindowUI::OnInpaintZoomSliderScroll(wxScrollEvent& event) {
    if (this->inpaintHelper->inPaintImageLoaded() == false) {
        event.Skip();
        return;
    }
    // get the enlarge values
    int canvasTop    = wxAtoi(this->m_inpaintCanvasTop->GetValue());
    int canvasLeft   = wxAtoi(this->m_inpaintCanvasLeft->GetValue());
    int canvasRight  = wxAtoi(this->m_inpaintCanvasRight->GetValue());
    int canvasBottom = wxAtoi(this->m_inpaintCanvasBottom->GetValue());

    this->inpaintHelper->SetZoomFactor(this->m_inpaintZoomSlider->GetValue() / 100.0);

    int rotation = this->inpaintHelper->GetZoomFactor() > (event.GetPosition() / 100.0) ? 1 : -1;

    this->inpaintHelper->OnMouseWheel(rotation, true, sd_gui_utils::wxEnlargeImageSizes{canvasTop, canvasRight, canvasBottom, canvasLeft});

    this->m_inpaintZoomSlider->SetValue(this->inpaintHelper->GetZoomFactor() * 100);
    this->m_inpaintBrushSizeSlider->SetValue(this->inpaintHelper->GetCurrentBrushSize());
    if (this->inpaintHelper->GetZoomFactor() == 1.0) {
        this->m_inpaintImageResolution->SetLabel(wxString::Format(_("%i x %i"), this->inpaintHelper->GetOriginalSize().GetWidth(), this->inpaintHelper->GetOriginalSize().GetHeight()));
    } else {
        this->m_inpaintImageResolution->SetLabel(wxString::Format(_("%i x %i (%i x %i)"),
                                                                  this->inpaintHelper->GetOriginalSize().GetWidth(),
                                                                  this->inpaintHelper->GetOriginalSize().GetHeight(),
                                                                  static_cast<int>(this->inpaintHelper->GetOriginalSize().GetWidth() * this->inpaintHelper->GetZoomFactor()),
                                                                  static_cast<int>(this->inpaintHelper->GetOriginalSize().GetHeight() * this->inpaintHelper->GetZoomFactor())));
    }
}
void MainWindowUI::OnInpaintCleanMask(wxCommandEvent& event) {
    this->m_inpaintClearMask->Disable();
    if (this->inpaintHelper->inPaintImageLoaded() == false) {
        return;
    }

    this->inpaintHelper->CleanMask();
}
void MainWindowUI::OnInPaintBrushStyleToggle(wxCommandEvent& event) {
    auto object = dynamic_cast<wxToggleButton*>(event.GetEventObject());
    if (object == nullptr) {
        return;
    }
    if (event.IsChecked() == false) {
        object->SetValue(true);
        return;
    }

    if (object == this->m_inPaintBrushStyleCircle) {
        this->m_inPaintBrushStyleSquare->SetValue(false);
        this->m_inPaintBrushStyleTriangle->SetValue(false);
        this->inpaintHelper->SetBrushStyle(sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_CIRCLE);
    } else if (object == this->m_inPaintBrushStyleSquare) {
        this->m_inPaintBrushStyleCircle->SetValue(false);
        this->m_inPaintBrushStyleTriangle->SetValue(false);
        this->inpaintHelper->SetBrushStyle(sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_SQUARE);
    } else if (object == this->m_inPaintBrushStyleTriangle) {
        this->m_inPaintBrushStyleCircle->SetValue(false);
        this->m_inPaintBrushStyleSquare->SetValue(false);
        this->inpaintHelper->SetBrushStyle(sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_TRIANGLE);
    }
}

void MainWindowUI::OnInpaintMaskOpen(wxFileDirPickerEvent& event) {
    if (this->inpaintHelper->inPaintImageLoaded() == false) {
        event.Skip();
        return;
    }
    wxFileName fn(event.GetPath());
    if (fn.FileExists() == false) {
        this->writeLog(wxString::Format(_("Inpaint mask open: file not found: %s"), event.GetPath()), true);
        return;
    }

    this->m_inpaintCanvasTop->SetValue(wxString("0"));
    this->m_inpaintCanvasRight->SetValue(wxString("0"));
    this->m_inpaintCanvasBottom->SetValue(wxString("0"));
    this->m_inpaintCanvasLeft->SetValue(wxString("0"));
    if (this->inpaintHelper->inPaintImageLoaded() == false) {
        wxMessageBox(_("No image loaded into the img2img, please load a base image before open the mask!"), _("Error"), wxOK | wxICON_ERROR);
        event.Skip();
        return;
    }
    // convert black to transparent pixels
    wxImage image = wxImage(event.GetPath(), wxBITMAP_TYPE_ANY);
    if (image.GetType() != wxBITMAP_TYPE_PNG) {
        wxMessageBox(_("The mask file must be a PNG file"), _("Error"), wxOK | wxICON_ERROR);
        return;
    }

    if (this->inpaintHelper->GetOriginalImage().GetWidth() < image.GetWidth() || this->inpaintHelper->GetOriginalImage().GetHeight() < image.GetHeight()) {
        this->writeLog(wxString::Format(_("Inpaint mask open: the resolution of the mask does not match the resolution of the image: %s"), event.GetPath()), true);
        // show an error message
        wxMessageBox(_("The resolution of the mask does not match the resolution of the image"), _("Error"), wxOK | wxICON_ERROR);
        return;
    }

    sd_gui_utils::convertMaskImageToTransparent(image);
    this->inpaintHelper->OnMaskFileOpen(image);
}

void MainWindowUI::OnImg2ImgMouseMotion(wxMouseEvent& event) {
    if (this->inpaintHelper->inPaintImageLoaded() == false) {
        event.Skip();
        return;
    }
    if (this->inpaintHelper->inPaintCanvasEmpty() == false) {
        this->m_inpaintClearMask->Enable();
    } else {
        this->m_inpaintClearMask->Disable();
    }

    this->inpaintHelper->OnMouseMotion(event);
}

void MainWindowUI::OnImg2ImgPaint(wxPaintEvent& event) {
    this->inpaintHelper->OnDcPaint(event);
}
void MainWindowUI::OnImg2ImgEraseBackground(wxEraseEvent& event) {
    // do nothing, see https://wiki.wxwidgets.org/Flicker-Free_Drawing
    this->inpaintHelper->OnEraseBackground(event);
}

void MainWindowUI::OnImg2ImgMouseWheel(wxMouseEvent& event) {
    if (!this->inpaintHelper->inPaintImageLoaded()) {
        event.Skip();
        return;
    }
    if (event.ControlDown() == false && event.ShiftDown() == false) {
        event.Skip();
        return;
    }

    // get the enlarge values
    int canvasTop    = wxAtoi(this->m_inpaintCanvasTop->GetValue());
    int canvasLeft   = wxAtoi(this->m_inpaintCanvasLeft->GetValue());
    int canvasRight  = wxAtoi(this->m_inpaintCanvasRight->GetValue());
    int canvasBottom = wxAtoi(this->m_inpaintCanvasBottom->GetValue());

    int rotation = event.GetWheelRotation();

    if (event.ControlDown()) {
        this->inpaintHelper->OnMouseWheel(rotation, true, sd_gui_utils::wxEnlargeImageSizes{canvasTop, canvasRight, canvasBottom, canvasLeft});
    }

    if (event.ShiftDown()) {
        this->inpaintHelper->OnMouseWheel(rotation, false, sd_gui_utils::wxEnlargeImageSizes{canvasTop, canvasRight, canvasBottom, canvasLeft});
    }

    // this->m_inpaintZoom->SetLabel(wxString::Format(_("Zoom: %.0f%%"), this->inpaintHelper->GetZoomFactor() * 100));
    this->m_inpaintZoomSlider->SetValue(this->inpaintHelper->GetZoomFactor() * 100);
    this->m_inpaintBrushSizeSlider->SetValue(this->inpaintHelper->GetCurrentBrushSize());
    if (this->inpaintHelper->GetZoomFactor() == 1.0) {
        this->m_inpaintImageResolution->SetLabel(wxString::Format(_("%i x %i"), this->inpaintHelper->GetOriginalSize().GetWidth(), this->inpaintHelper->GetOriginalSize().GetHeight()));
    } else {
        this->m_inpaintImageResolution->SetLabel(wxString::Format(_("%i x %i (%i x %i)"),
                                                                  this->inpaintHelper->GetOriginalSize().GetWidth(),
                                                                  this->inpaintHelper->GetOriginalSize().GetHeight(),
                                                                  static_cast<int>(this->inpaintHelper->GetOriginalSize().GetWidth() * this->inpaintHelper->GetZoomFactor()),
                                                                  static_cast<int>(this->inpaintHelper->GetOriginalSize().GetHeight() * this->inpaintHelper->GetZoomFactor())));
    }
}
void MainWindowUI::OnSendChat(wxCommandEvent& event) {
    // model

    auto mindex    = this->m_languageModel->GetCurrentSelection();
    auto data      = this->m_languageModel->GetClientData(mindex);
    auto modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(data);
    if (!modelinfo) {
        return;
    }
    std::lock_guard<std::mutex> lock(this->chat_mutex);
    if (this->chat_currentMessage == nullptr) {
        this->chat_currentMessage = std::make_shared<sd_gui_utils::llvmMessage>();
        this->chat_currentMessage->SetId();
    }
    this->chat_currentMessage->SetStatus(sd_gui_utils::llvmstatus::LLVM_STATUS_PENDING);
    this->chat_currentMessage->AppendUserPrompt(this->m_chatInput->GetValue().ToStdString());

    this->chat_currentMessage->SetCommandType(sd_gui_utils::llvmCommand::LLVM_COMMAND_GENERATE_TEXT);
    this->chat_currentMessage->SetModelPath(modelinfo->path);

    if (this->chat_currentMessage->Status(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_UNLOADED)) {
        this->chat_currentMessage->SetNThreads(this->mapp->cfg->n_threads);
        this->chat_currentMessage->SetNCtx(this->m_chat_n_ctx->GetValue());
        this->chat_currentMessage->SetNBatch(this->m_chat_n_batch->GetValue());
    }

    const wxString prompt_template = this->m_chat_prompt_template->GetValue();
    if (!prompt_template.empty() && prompt_template.Length() > 2) {
        this->chat_currentMessage->SetPromptTemplate(prompt_template.ToStdString());
    }

    for (auto helper : this->processHelpers) {
        if (helper->IsAlive() && helper->GetProcessType() == ExternalProcessHelper::ProcessType::llama) {
            helper->write(this->chat_currentMessage->toString());
            break;
        }
    }
    this->m_chatInput->SetValue(wxEmptyString);
    this->m_chatInput->SetFocus();
};
void MainWindowUI::OnChatInputTextEnter(wxCommandEvent& event) {
    if (wxGetKeyState(WXK_SHIFT) || wxGetKeyState(WXK_CONTROL)) {
        event.Skip();
        return;
    }

    const wxString text = this->m_chatInput->GetValue();
    if (!text.empty()) {
        std::cout << "Sending message: " << text.ToStdString() << std::endl;
        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, this->m_sendChat->GetId());
        this->m_sendChat->GetEventHandler()->ProcessEvent(event);
        return;
    }
    event.Skip();
};
void MainWindowUI::OnChatInputKeyDown(wxKeyEvent& event) {
    // check if ctrl + P pressed
    if (event.ControlDown() && event.GetKeyCode() == 'P') {
        this->llamaGuiHelper->PrintCurrentwebView();
        event.Skip();
    }

    // this is not work's, just shows the non modified source
    if (event.ControlDown() && event.GetKeyCode() == 'S') {
        const auto content = this->llamaGuiHelper->GetCurrentWebviewSource();
        if (content.empty()) {
            event.Skip();
            return;
        }

        // show a save file as.. dialog in the user's Documents directory and save the content as html file
        wxString filename = wxFileSelector(
            _("Save chat as HTML"),
            wxStandardPaths::Get().GetDocumentsDir(),
            wxString::Format("%s_chat_%" PRId64, wxString(PROJECT_NAME), wxDateTime::Now().GetTicks()),
            wxT("html"),
            "HTML files (*.html)|*.html",
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (filename.empty()) {
            event.Skip();
            return;
        }
        wxFile file;
        if (file.Create(filename, true)) {
            file.Write(content);
            file.Close();
        }
        event.Skip();
        return;
    }
    event.Skip();
}
void MainWindowUI::OnLanguageModelSelect(wxCommandEvent& event) {
    event.Skip();
    const auto selection = this->m_languageModel->GetSelection();

    if (selection == 0) {  // unload model
        std::lock_guard<std::mutex> lock(this->chat_mutex);
        this->m_languageModel->Disable();  // disable while extprocess unloads the model
        if (this->chat_currentMessage == nullptr) {
            this->chat_currentMessage = std::make_shared<sd_gui_utils::llvmMessage>();
            this->chat_currentMessage->SetId();
        }
        this->chat_currentMessage->SetStatus(sd_gui_utils::llvmstatus::LLVM_STATUS_PENDING);

        this->chat_currentMessage->SetCommandType(sd_gui_utils::llvmCommand::LLVM_COMMAND_MODEL_UNLOAD);
        this->chat_currentMessage->SetModelPath("");
        this->chat_currentMessage->SetNThreads(this->mapp->cfg->n_threads);

        for (auto helper : this->processHelpers) {
            if (helper->IsAlive() && helper->GetProcessType() == ExternalProcessHelper::ProcessType::llama) {
                helper->write(this->chat_currentMessage->toString());
                break;
            }
        }

        return;
    }  // unload model

    // load the model... there is not need to disable controls, because the external process only read the new command when model load is finished...
    auto data      = this->m_languageModel->GetClientData(selection);
    auto modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(data);
    if (!modelinfo) {
        return;
    }
    this->m_languageModel->Disable();  // disable while extprocess loads the model
    std::lock_guard<std::mutex> lock(this->chat_mutex);
    if (this->chat_currentMessage == nullptr) {
        this->chat_currentMessage = std::make_shared<sd_gui_utils::llvmMessage>();
        this->chat_currentMessage->SetId();
    }
    this->chat_currentMessage->SetStatus(sd_gui_utils::llvmstatus::LLVM_STATUS_PENDING);

    this->chat_currentMessage->SetCommandType(sd_gui_utils::llvmCommand::LLVM_COMMAND_MODEL_LOAD);
    this->chat_currentMessage->SetModelPath(modelinfo->path);
    this->chat_currentMessage->SetNThreads(this->mapp->cfg->n_threads);

    if (this->chat_currentMessage->Status(sd_gui_utils::llvmContextStatus::LLVM_CONTEXT_STATUS_UNLOADED)) {
        this->chat_currentMessage->SetNBatch(this->m_chat_n_batch->GetValue());
        this->chat_currentMessage->SetNCtx(this->m_chat_n_ctx->GetValue());
    }

    const wxString user_template = this->m_chat_prompt_template->GetValue();

    if (!user_template.empty()) {
        this->chat_currentMessage->SetPromptTemplate(user_template.ToStdString());
    }

    this->m_chatStatus->SetLabel(wxString::Format(_("Loading model: %s"), modelinfo->name));

    for (auto helper : this->processHelpers) {
        if (helper->IsAlive() && helper->GetProcessType() == ExternalProcessHelper::ProcessType::llama) {
            helper->write(this->chat_currentMessage->toString());
            break;
        }
    }
};
