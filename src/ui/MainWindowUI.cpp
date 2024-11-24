#include "MainWindowUI.h"
#include <chrono>
#include <cinttypes>
#include <cstring>
#include <filesystem>
#include <memory>
#include <thread>
#include "../helpers/simplecurl.h"
#include "../helpers/sslUtils.hpp"
#include "MainWindowAboutDialog.h"
#include "MainWindowImageDialog.h"
#include "QueueManager.h"
#include "embedded_files/app_icon.h"
#include "extprocess/config.hpp"
#include "utils.hpp"
#include "ver.hpp"
#include "wx/colour.h"
#include "wx/fileconf.h"
#include "wx/filename.h"
#include "wx/image.h"
#include "wx/string.h"
#include "wx/translation.h"
#include "wx/uilocale.h"
#include "wx/utils.h"
#include "wx/variant.h"
#include "wx/window.h"

MainWindowUI::MainWindowUI(wxWindow* parent, const std::string dllName, const std::string& usingBackend, bool disableExternalProcessHandling, MainApp* mapp)
    : mainUI(parent), usingBackend(usingBackend), disableExternalProcessHandling(disableExternalProcessHandling), mapp(mapp) {
    this->ControlnetOrigPreviewBitmap = this->m_controlnetImagePreview->GetBitmap();
    this->AppOrigPlaceHolderBitmap    = this->m_img2img_preview->GetBitmap();

    this->SetTitle(wxString::Format("%s - %s (%s)", PROJECT_DISPLAY_NAME, SD_GUI_VERSION, GIT_HASH));
    this->TaskBar = new wxTaskBarIcon();

    this->TaskBarIcon = app_png_to_wx_bitmap();
    this->TaskBar->SetIcon(this->TaskBarIcon, wxString::Format("%s - %s (%s)", PROJECT_DISPLAY_NAME, SD_GUI_VERSION, GIT_HASH));

    wxIcon icon;
    icon.CopyFromBitmap(this->TaskBarIcon);
    this->SetIcon(icon);

    this->cfg = new sd_gui_utils::config;

    this->initConfig();
    if (this->cfg->enable_civitai == false) {
        this->m_civitai->Hide();
    } else {
        this->m_civitai->Show();
    }

    wxPersistentRegisterAndRestore(this, this->GetName());

    this->qmanager     = std::make_shared<QM::QueueManager>(this->GetEventHandler(), this->cfg->jobs);
    this->ModelManager = std::make_shared<ModelInfo::Manager>(this->cfg->datapath);

    this->initLog();

    // load
    this->LoadPresets();
    this->loadModelList();
    this->loadLoraList();
    this->loadVaeList();
    this->loadTaesdList();
    this->loadControlnetList();
    this->loadEsrganList();
    this->loadEmbeddingList();
    this->refreshModelTable();
    // reload config, set up checkboxes from it
    const auto diffusion_flash_attn = this->mapp->config->ReadBool("/diffusion_flash_attn", this->diffusionFlashAttn->GetValue());
    const auto clip_on_cpu          = this->mapp->config->ReadBool("/clip_on_cpu", this->clipOnCpu->GetValue());
    const auto controlnet_on_cpu    = this->mapp->config->ReadBool("/controlnet_on_cpu", this->cnOnCpu->GetValue());
    const auto vae_on_cpu           = this->mapp->config->ReadBool("/vae_on_cpu", this->vaeOnCpu->GetValue());

    this->diffusionFlashAttn->SetValue(diffusion_flash_attn);
    this->clipOnCpu->SetValue(clip_on_cpu);
    this->cnOnCpu->SetValue(controlnet_on_cpu);
    this->vaeOnCpu->SetValue(vae_on_cpu);

    Bind(wxEVT_THREAD, &MainWindowUI::OnThreadMessage, this);

    this->m_upscalerHelp->SetPage(wxString::Format((_("Officially from sd.cpp, the following upscaler model is supported: <br/><a href=\"%s\">RealESRGAN_x4Plus Anime 6B</a><br/>This is working sometimes too: <a href=\"%s\">RealESRGAN_x4Plus</a>")), wxString("https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.2.4/RealESRGAN_x4plus_anime_6B.pth"), wxString("https://civitai.com/models/147817/realesrganx4plus")));

    // setup shared memory
    this->sharedMemory = std::make_shared<SharedMemoryManager>(SHARED_MEMORY_PATH, SHARED_MEMORY_SIZE, true);

    if (this->disableExternalProcessHandling == false) {
        this->m_stop_background_process->Show();
        if (BUILD_TYPE == "Release") {  // run it from PATH on unix & win
            this->extprocessCommand = std::string(EPROCESS_BINARY_NAME);
        } else {
            this->extprocessCommand = "./extprocess/" + std::string(EPROCESS_BINARY_NAME);
        }

        wxFileName f(wxStandardPaths::Get().GetExecutablePath());
        wxString appPath(f.GetPathWithSep());

        wxString commandFullPath = appPath + wxString::FromUTF8Unchecked(this->extprocessCommand.c_str());

        this->extprocessCommand = commandFullPath.utf8_string();

        if (std::filesystem::exists(this->extprocessCommand) == false) {
            wxMessageDialog errorDialog(this, wxString::Format(_("An error occurred when trying to start external process: %s.\n Please try again."), this->extprocessCommand), _("Error"), wxOK | wxICON_ERROR);
            this->writeLog(wxString::Format(_("An error occurred when trying to start external process: %s.\n Please try again."), this->extprocessCommand));
            errorDialog.ShowModal();
            this->Destroy();
            return;
        }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        wxString dllFullPath  = appPath + wxString::FromUTF8Unchecked(dllName.c_str());
        this->extProcessParam = dllFullPath.utf8_string() + ".dll";

        if (std::filesystem::exists(this->extProcessParam) == false) {
            wxMessageDialog errorDialog(this, wxString::Format(_("An error occurred when trying to start external process. Shared lib not found: %s.\n Please try again."), this->extProcessParam), _("Error"), wxOK | wxICON_ERROR);
            this->writeLog(wxString::Format(_("An error occurred when trying to start external process. Shared lib not found: %s.\n Please try again."), this->extProcessParam));
            errorDialog.ShowModal();
            this->Destroy();
            return;
        }
#else
        this->extProcessParam = dllName + ".so";
#endif

        this->extProcessRunning = false;

        // start process from the thread

        this->writeLog("Starting external process: " + this->extprocessCommand + " " + this->extProcessParam + "\n");
        const char* command_line[] = {this->extprocessCommand.c_str(), this->extProcessParam.c_str(), nullptr};
        this->subprocess           = new subprocess_s();

        int result = subprocess_create(command_line, subprocess_option_no_window | subprocess_option_combined_stdout_stderr | subprocess_option_enable_async | subprocess_option_search_user_path | subprocess_option_inherit_environment, this->subprocess);
        if (0 != result) {
            wxMessageDialog errorDialog(this, _("An error occurred when trying to start external process. Please try again."), _("Error"), wxOK | wxICON_ERROR);
            errorDialog.ShowModal();
            delete this->subprocess;
            this->subprocess = nullptr;
            this->Destroy();
            return;
        }
        this->extProcessNeedToRun = true;
        this->processCheckThread  = std::make_shared<std::thread>(&MainWindowUI::ProcessCheckThread, this);
        this->processHandleOutput = std::make_shared<std::thread>(&MainWindowUI::ProcessOutputThread, this);
    }
}

void MainWindowUI::onSettings(wxCommandEvent& event) {
    if (this->qmanager->IsRunning()) {
        wxMessageDialog(this, _("Please wait to finish the currently running jobs!")).ShowModal();
        return;
    }
    auto bitmap = app_png_to_wx_bitmap();
    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    this->settingsWindow = new MainWindowSettings(this);
    wxPersistenceManager::Get().RegisterAndRestore(this->settingsWindow);
    this->settingsWindow->SetIcon(icon);
    this->Freeze();
    this->settingsWindow->Bind(wxEVT_CLOSE_WINDOW, &MainWindowUI::OnCloseSettings, this);
    this->Hide();
    this->settingsWindow->Show();
}

void MainWindowUI::onModelsRefresh(wxCommandEvent& event) {
    this->loadModelList();
    this->loadLoraList();
    this->loadVaeList();
    this->loadTaesdList();
    this->loadControlnetList();
    this->loadEsrganList();
}

void MainWindowUI::OnAboutButton(wxCommandEvent& event) {
    // SdGetSystemInfoFunction
    // SdGetSystemInfoFunction sd_get_system_info = (SdGetSystemInfoFunction)this->sd_dll->GetSymbol("sd_get_system_info");
    // auto sysinfo                               = //sd_get_system_info();
    MainWindowAboutDialog* dialog = new MainWindowAboutDialog(this);

    wxString about = wxString(_("<p><strong>Disclaimer</strong></p><p>Use of this application is at your own risk. The developer assumes no responsibility or liability for any potential data loss, damage, or other issues arising from its usage. By using this application, you acknowledge and accept these terms.</p>"));

    // about.Append(wxString::Format(
    //     "<h2>%s</h2><p>Version: %s </p><p>Git version: %s</p><p>Website: <a target='_blank' href='%s'>%s</a></p><hr/>",
    //     PROJECT_NAME, SD_GUI_VERSION, GIT_HASH, SD_GUI_HOMEPAGE, PROJECT_NAME));

    about.Append(wxString::Format("<h2>%s</h2>", PROJECT_NAME));
    about.Append(wxString::Format(_("<p>Version: %s</p>"), SD_GUI_VERSION));
    about.Append(wxString::Format(_("<p>Git version: %s</p>"), GIT_HASH));
    about.Append(wxString::Format(_("<p>Website: <a target='_blank' href='%s'>%s</a></p>"), SD_GUI_HOMEPAGE, SD_GUI_HOMEPAGE));

    about.Append(wxString::Format(_("<p>Loaded backend: %s</p>"), usingBackend.c_str()));
    about.append(wxString::Format(
        "<p>%s %s</p><p>%s %s</p><p>%s %s</p>", _("Configuration folder:"),
        wxStandardPaths::Get().GetUserConfigDir(), _("Config file:"), this->mapp->getIniPath(), _("Data folder:"), this->cfg->datapath));

    wxString format_string = "<p>%s %s</p>";
    about.Append(wxString::Format(format_string, _("Model folder:"), this->cfg->model));
    about.Append(wxString::Format(format_string, _("Embedding folder:"), this->cfg->embedding));
    about.Append(wxString::Format(format_string, _("Lora folder:"), this->cfg->lora));
    about.Append(wxString::Format(format_string, _("Vae folder:"), this->cfg->vae));
    about.Append(wxString::Format(format_string, _("Controlnet folder:"), this->cfg->controlnet));
    about.Append(wxString::Format(format_string, _("ESRGAN folder:"), this->cfg->esrgan));
    about.Append(wxString::Format(format_string, _("TAESD folder:"), this->cfg->taesd));
    about.Append(wxString::Format(format_string, _("Output folder:"), this->cfg->output));
    about.Append(wxString::Format(format_string, _("Jobs folder:"), this->cfg->jobs));
    about.Append(wxString::Format(format_string, _("Presets folder:"), this->cfg->presets));

    about.append(wxString("<pre>"));
    about.append(wxString(_("N/A")));
    about.append(wxString("</pre>"));
    dialog->m_about->SetPage(about);
    dialog->SetIcon(this->GetIcon());
    dialog->ShowModal();
}

void MainWindowUI::OnCivitAitButton(wxCommandEvent& event) {
    if (this->civitwindow == nullptr) {
        this->civitwindow = new MainWindowCivitAiWindow(this);
        auto bitmap       = app_png_to_wx_bitmap();
        wxIcon icon;
        icon.CopyFromBitmap(bitmap);

        this->civitwindow->SetIcon(icon);
        this->civitwindow->SetModelManager(this->ModelManager);
        this->civitwindow->SetCfg(this->cfg);
        this->civitwindow->SetTitle(wxString::Format("%s | %s", this->GetTitle(), this->civitwindow->GetTitle()));
        this->civitwindow->Bind(wxEVT_THREAD, &MainWindowUI::OnCivitAiThreadMessage, this);
        this->civitwindow->Bind(wxEVT_CLOSE_WINDOW, &MainWindowUI::OnCloseCivitWindow, this);
        this->civitwindow->SetName(_("CivitAi.com model downloader"));
        wxPersistenceManager::Get().RegisterAndRestore(this->civitwindow);
        this->civitwindow->Show();
        return;
    }
    this->civitwindow->RequestUserAttention();
}

void MainWindowUI::OnStopBackgroundProcess(wxCommandEvent& event) {
    if (this->subprocess != nullptr &&              // we have subprocess
        subprocess_alive(this->subprocess) != 0 &&  // is running
        this->extProcessNeedToRun &&                // need to run, eg. not in stopping state
        this->qmanager->GetCurrentItem() == nullptr) {
        this->m_stop_background_process->Disable();
        subprocess_terminate(this->subprocess);
    }
}

void MainWindowUI::onModelSelect(wxCommandEvent& event) {
    // check if really selected a model, or just the first element, which is
    // always exists...
    if (this->m_model->GetSelection() == 0) {
        this->m_generate1->Disable();
        this->m_generate2->Disable();
        {
            std::lock_guard<std::mutex> lock(this->taskBarMutex);
            this->m_statusBar166->SetStatusText(_("No model selected"));
        }
        return;
    }
    auto name = this->m_model->GetStringSelection().utf8_string();
    // img2img has an image
    if (!this->m_img2imgOpen->GetPath().empty()) {
        this->m_generate1->Enable();
    }
    this->m_generate2->Enable();
    {
        std::lock_guard<std::mutex> lock(this->taskBarMutex);
        this->m_statusBar166->SetStatusText(wxString::Format(_("Model: %s"), this->ModelFiles.at(name)));
    }
}

void MainWindowUI::onTypeSelect(wxCommandEvent& event) {
    // TODO: Implement onTypeSelect
}

void MainWindowUI::onVaeSelect(wxCommandEvent& event) {
    // TODO: Implement onVaeSelect
}

void MainWindowUI::onRandomGenerateButton(wxCommandEvent& event) {
    this->m_seed->SetValue(sd_gui_utils::generateRandomInt(100000000, 999999999));
}

void MainWindowUI::onSd15ResSelect(wxCommandEvent& event) {
    const auto index = this->m_sd15Res->GetSelection();
    if (index < 1) {
        return;
    }
    const auto text = this->m_sd15Res->GetString(index).utf8_string();
    size_t pos      = text.find('x');
    int w           = std::stoi(text.substr(0, pos));
    int h           = std::stoi(text.substr(pos + 1));
    this->m_width->SetValue(w);
    this->m_height->SetValue(h);
}

void MainWindowUI::onSdXLResSelect(wxCommandEvent& event) {
    const auto index = this->m_sdXlres->GetSelection();
    if (index < 1) {
        return;
    }
    const auto text = this->m_sdXlres->GetString(index).utf8_string();
    size_t pos      = text.find('x');
    int w           = std::stoi(text.substr(0, pos));
    int h           = std::stoi(text.substr(pos + 1));
    this->m_width->SetValue(w);
    this->m_height->SetValue(h);
}

void MainWindowUI::OnWHChange(wxSpinEvent& event) {
    int w = this->m_width->GetValue();
    int h = this->m_height->GetValue();

    if (this->m_model->GetCurrentSelection() > 0) {
        this->m_generate2->Enable(true);
    }

    this->init_width  = w;
    this->init_height = h;
}

void MainWindowUI::onResolutionSwap(wxCommandEvent& event) {
    auto oldW = this->m_width->GetValue();
    auto oldH = this->m_height->GetValue();

    this->m_height->SetValue(oldW);
    this->m_width->SetValue(oldH);
}

void MainWindowUI::m_notebook1302OnNotebookPageChanged(wxNotebookEvent& event) {
    event.Skip();
    int selected = event.GetSelection();
    if (selected == wxNOT_FOUND) {
        return;
    }

    if (selected == sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG)  // on img2img and img2vid the vae_decode_only is false, otherwise true
    {
        this->m_vae_decode_only->SetValue(false);
    } else {
        this->m_vae_decode_only->SetValue(true);
    }

    if (selected == sd_gui_utils::GuiMainPanels::PANEL_QUEUE || selected == sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG || selected == sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG || selected == sd_gui_utils::GuiMainPanels::PANEL_MODELS) {
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

        if (selected == sd_gui_utils::GuiMainPanels::PANEL_IMG2IMG)  // not working on img2img.. this is hardcoded in sd.cpp
        // to only 1 image...
        {
            this->m_batch_count->Disable();
            this->m_batch_count->SetValue(1);
        } else {
            this->m_batch_count->Enable();
            this->m_batch_count->SetToolTip("");
        }
        this->m_sampler->Enable();
        this->m_scheduler->Enable();

        this->m_preset_list->Enable();
        this->m_save_preset->Enable();

        this->m_seed->Enable();
        this->m_steps->Enable();

        if (selected == sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG && this->m_controlnetImageOpen->GetPath().length() > 0) {
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
            this->m_width->Disable();
            this->m_height->Disable();
            this->m_button7->Disable();  // swap button
        }
        if (selected == sd_gui_utils::GuiMainPanels::PANEL_TEXT2IMG) {
            if (this->m_filePickerDiffusionModel->GetPath().empty() == false) {
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
        this->m_type->Enable();
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
}

void MainWindowUI::onJobsStart(wxCommandEvent& event) {
    // TODO: Implement onJobsStart
    this->qmanager->RestartQueue();
}

void MainWindowUI::onJobsPause(wxCommandEvent& event) {
    this->qmanager->PauseAll();
}

void MainWindowUI::onJobsDelete(wxCommandEvent& event) {
    // this->qmanager->RestartQueue();
}

void MainWindowUI::OnJobListItemActivated(wxDataViewEvent& event) {
    // implemented in OnJobListItemSelection
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

        auto item = event.GetItem();

        wxDataViewListStore* store = this->m_joblist->GetStore();

        int id     = store->GetItemData(item);
        auto qitem = this->qmanager->GetItemPtr(id);

        if (qitem->mode != QM::GenerationMode::CONVERT) {
            menu->Append(1, _("Requeue"));
            if (qitem->mode == QM::GenerationMode::IMG2IMG ||
                qitem->mode == QM::GenerationMode::UPSCALE) {
                if (!std::filesystem::exists(qitem->initial_image)) {
                    menu->Enable(1, false);
                }
            }
        }

        if (qitem->params.controlnet_path.length() > 0 &&
            qitem->params.control_image_path.length() > 0 &&
            !std::filesystem::exists(qitem->params.control_image_path)) {
            menu->Enable(1, false);
        }

        if (qitem->mode != QM::GenerationMode::UPSCALE &&
            qitem->mode != QM::GenerationMode::CONVERT) {
            menu->Append(2, _("Load parameters"));
            menu->Append(3, _("Copy prompts to text2img"));
            menu->Append(4, _("Copy prompts to img2img"));
            menu->Append(5, wxString::Format(_("Select model %s"), qitem->model));

            if (qitem->images.size() > 0) {
                menu->Append(6, _("Send the last image to the Upscale tab"));
                menu->Append(7, _("Send the last image to the img2img tab"));
            }
        }
        if (qitem->mode == QM::GenerationMode::UPSCALE) {
            if (qitem->images.size() > 0) {
                menu->Append(6, wxString::Format(_("Upscale again")));
            }
        }

        if (qitem->status == QM::QueueStatus::PAUSED ||
            qitem->status == QM::QueueStatus::PENDING) {
            menu->AppendSeparator();
            menu->Append(8, qitem->status == QM::QueueStatus::PENDING ? _("Pause") : _("Resume"));
        }

        menu->AppendSeparator();
        menu->Append(99, _("Delete"));
        menu->Enable(99, false);

        if (qitem->status == QM::QueueStatus::RUNNING ||
            qitem->status == QM::QueueStatus::HASHING) {
            menu->Enable(1, false);
        }

        if (qitem->status == QM::QueueStatus::PENDING ||
            qitem->status == QM::QueueStatus::PAUSED ||
            qitem->status == QM::QueueStatus::FAILED ||
            qitem->status == QM::QueueStatus::DONE) {
            menu->Enable(99, true);
        }
        menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainWindowUI::OnPopupClick, this);
        PopupMenu(menu);
        delete menu;
    }

    if (source == this->m_data_model_list) {
        if (this->m_data_model_list->GetSelectedItemsCount() == 0) {
            return;
        }
        wxMenu* menu = new wxMenu();
        auto item    = this->m_data_model_list->GetCurrentItem();

        wxDataViewListStore* store             = this->m_data_model_list->GetStore();
        sd_gui_utils::ModelFileInfo* modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(store->GetItemData(item));

        if (!modelinfo->sha256.empty()) {
            menu->Append(100, _("RE-Calculate Hash"));
        } else {
            menu->Append(100, _("Calculate Hash"));
        }
        if (!modelinfo->civitaiPlainJson.empty() && modelinfo->hash_progress_size == 0 && this->cfg->enable_civitai == true) {
            menu->Append(105, _("Force update info from CivitAi"));
        }

        if (modelinfo->model_type == sd_gui_utils::DirTypes::CHECKPOINT) {
            menu->Append(200, wxString::Format(_("Select model %s to the next job"), modelinfo->name));
            if (modelinfo->state == sd_gui_utils::CivitAiState::OK && this->cfg->enable_civitai == true) {
                menu->Append(199, _("Open model on CivitAi.com in default browser"));
            }
            if (modelinfo->name.find(".safetensors")) {
                menu->AppendSeparator();
                menu->Append(201, wxString::Format(_("Convert model to %s gguf format"), this->m_type->GetStringSelection()));
            }
        }
        if (modelinfo->model_type == sd_gui_utils::DirTypes::LORA) {
            auto shortname = std::filesystem::path(modelinfo->name).replace_extension().string();
            menu->Append(101, wxString::Format(_("Append to text2img prompt <lora:%s:0.5>"), shortname));
            menu->Append(102, wxString::Format(_("Append to text2img neg. prompt <lora:%s:0.5>"), shortname));
            menu->Append(103, wxString::Format(_("Append to img2img prompt <lora:%s:0.5>"), shortname));
            menu->Append(104, wxString::Format(_("Append to img2img neg. prompt <lora:%s:0.5>"), shortname));
        }
        if (modelinfo->model_type == sd_gui_utils::DirTypes::EMBEDDING) {
            auto shortname = std::filesystem::path(modelinfo->name).replace_extension().string();
            menu->Append(111, wxString::Format(_("Append to text2img prompt %s"), shortname));
            menu->Append(112, wxString::Format(_("Append to text2img neg. prompt %s"), shortname));
            menu->Append(113, wxString::Format(_("Append to img2img prompt %s"), shortname));
            menu->Append(114, wxString::Format(_("Append to img2img neg. prompt %s"), shortname));
        }
        menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainWindowUI::OnPopupClick, this);
        PopupMenu(menu);
        delete menu;
    }
}

void MainWindowUI::OnJobListItemSelection(wxDataViewEvent& event) {
    auto row = this->m_joblist->GetSelectedRow();
    if (row == wxNOT_FOUND) {
        return;
    }
    auto store = this->m_joblist->GetStore();
    auto ditem = store->GetItem(row);
    int itemid = store->GetItemData(ditem);
    auto item  = this->qmanager->GetItemPtr(itemid);

    this->UpdateJobInfoDetailsFromJobQueueList(item);
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

    // only jpeg supported now
    if (img.GetType() != wxBITMAP_TYPE_JPEG) {
        wxMessageBox(wxString::Format(_("Only jpeg supported: %s"), path.GetExt()));
        return;
    }

    /// get meta
    try {
        auto image = Exiv2::ImageFactory::open(file.utf8_string());
        if (!image->good()) {
            return;
        }
        image->readMetadata();
        Exiv2::ExifData& exifData = image->exifData();
        if (!exifData.empty()) {
            std::string ex;
            Exiv2::ExifData::iterator it;
            std::string usercomment;
            for (it = exifData.begin(); it != exifData.end(); ++it) {
                if (it->key() == "Exif.Photo.UserComment") {
                    usercomment = it->getValue()->toString();
                    if (!usercomment.empty()) {
                        std::map<std::string, std::string> getParams = sd_gui_utils::parseExifPrompts(usercomment);
                        this->imageCommentToGuiParams(getParams, SDMode::TXT2IMG);
                        break;
                    }
                }
            }
        }
    } catch (Exiv2::Error& e) {
        std::cerr << "Err: " << e.what() << std::endl;
    }
}

/// TODO: store embeddings like checkpoints and loras, the finetune this
void MainWindowUI::OnPromptText(wxCommandEvent& event) {
    event.Skip();  // disabled while really slow
    return;
}

void MainWindowUI::OnNegPromptText(wxCommandEvent& event) {
    event.Skip();  // disabled while really slow
    return;
}

void MainWindowUI::onGenerate(wxCommandEvent& event) {
    bool generate1State = this->m_generate1->IsEnabled();
    bool generate2State = this->m_generate2->IsEnabled();
    bool generate3State = this->m_generate_upscaler->IsEnabled();

    this->m_generate1->Enable(false);
    this->m_generate2->Enable(false);
    this->m_generate_upscaler->Enable(false);

    // this->initConfig();
    auto type  = QM::GenerationMode::TXT2IMG;
    int pageId = this->m_notebook1302->GetSelection();
    switch (pageId) {
        case 1:
            type = QM::GenerationMode::TXT2IMG;
            break;
        case 2:
            type = QM::GenerationMode::IMG2IMG;
            break;
        case 3:
            type = QM::GenerationMode::UPSCALE;
            break;
        default:
            return;
            break;
    }

    if (type == QM::GenerationMode::UPSCALE) {
        std::shared_ptr<QM::QueueItem> item = std::make_shared<QM::QueueItem>();
        item->model                         = this->m_upscaler_model->GetStringSelection().utf8_string();
        item->mode                          = type;
        item->params.esrgan_path            = this->EsrganFiles.at(this->m_upscaler_model->GetStringSelection().utf8_string());
        item->initial_image                 = this->m_upscaler_filepicker->GetPath();
        item->params.mode                   = SDMode::MODE_COUNT;
        item->params.n_threads              = this->cfg->n_threads;
        item->keep_checkpoint_in_memory     = this->m_keep_other_models_in_memory->GetValue();
        item->keep_upscaler_in_memory       = this->m_keep_upscaler_in_memory->GetValue();

        auto selectedwType = this->m_type->GetStringSelection();
        for (auto types : sd_gui_utils::sd_type_gui_names) {
            if (types.second == selectedwType) {
                item->params.wtype = (sd_type_t)types.first;
            }
        }

        auto selectedScheduler = this->m_scheduler->GetStringSelection();
        for (auto schedulers : sd_gui_utils::sd_scheduler_gui_names) {
            if (schedulers.second == selectedScheduler) {
                item->params.schedule = (schedule_t)schedulers.first;
                break;
            }
        }

        if (this->cfg->save_all_image) {
            item->images.emplace_back(QM::QueueItemImage({item->initial_image, QM::QueueItemImageType::INITIAL}));
        }
        this->qmanager->AddItem(item, false);
        return;
    }

    std::shared_ptr<QM::QueueItem> item = std::make_shared<QM::QueueItem>();

    auto diffusionModel = this->m_filePickerDiffusionModel->GetPath().utf8_string();

    if (diffusionModel.empty() == false) {
        item->params.diffusion_model_path = diffusionModel;
    } else {
        auto mindex                            = this->m_model->GetCurrentSelection();
        sd_gui_utils::ModelFileInfo* modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_model->GetClientData(mindex));
        item->params.model_path                = modelinfo->path;
    }

    if (this->m_taesd->GetCurrentSelection() > 0) {
        item->params.taesd_path = this->TaesdFiles.at(this->m_taesd->GetStringSelection().utf8_string());
    } else {
        item->params.taesd_path = "";
    }
    if (this->m_vae->GetCurrentSelection() > 0) {
        item->params.vae_path = this->VaeFiles.at(this->m_vae->GetStringSelection().utf8_string());
    } else {
        item->params.vae_path = "";
    }

    item->params.lora_model_dir     = this->cfg->lora;
    item->params.embeddings_path    = this->cfg->embedding;
    item->params.n_threads          = this->cfg->n_threads;
    item->keep_checkpoint_in_memory = this->m_keep_other_models_in_memory->GetValue();
    item->keep_upscaler_in_memory   = this->m_keep_upscaler_in_memory->GetValue();

    item->params.vae_on_cpu           = this->vaeOnCpu->GetValue();
    item->params.clip_on_cpu          = this->clipOnCpu->GetValue();
    item->params.diffusion_flash_attn = this->diffusionFlashAttn->GetValue();

    if (type == QM::GenerationMode::TXT2IMG) {
        item->params.prompt          = this->m_prompt->GetValue().utf8_string();
        item->params.negative_prompt = this->m_neg_prompt->GetValue().utf8_string();

        if (this->m_controlnetModels->GetSelection() > 0) {
            item->params.control_net_cpu = this->cnOnCpu->GetValue();
        }

        auto diffusionModel = this->m_filePickerDiffusionModel->GetPath();

        if (diffusionModel.IsEmpty()) {
            item->model = this->m_model->GetStringSelection().utf8_string();
        } else {
            item->params.diffusion_model_path = diffusionModel.utf8_string();
            item->model                       = this->m_filePickerDiffusionModel->GetFileName().GetName().utf8_string();
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
    if (type == QM::GenerationMode::IMG2IMG) {
        item->params.prompt          = this->m_prompt2->GetValue().utf8_string();
        item->params.negative_prompt = this->m_neg_prompt2->GetValue().utf8_string();
    }

    item->params.cfg_scale    = static_cast<float>(this->m_cfg->GetValue());
    item->params.seed         = this->m_seed->GetValue();
    item->params.clip_skip    = this->m_clip_skip->GetValue();
    item->params.sample_steps = this->m_steps->GetValue();

    if (type == QM::GenerationMode::TXT2IMG) {
        item->params.control_strength = this->m_controlnetStrength->GetValue();
    }

    if (type == QM::GenerationMode::IMG2IMG) {
        item->params.strength = this->m_strength->GetValue();
    }

    if (this->m_controlnetModels->GetCurrentSelection() > 0 && type == QM::GenerationMode::TXT2IMG) {
        item->params.controlnet_path = this->ControlnetModels.at(this->m_controlnetModels->GetStringSelection().utf8_string());
        // do not set the control image if we have no model
        if (this->m_controlnetImageOpen->GetPath().length() > 0) {
            item->params.control_image_path = this->m_controlnetImageOpen->GetPath().utf8_string();
        }
    } else {
        item->params.controlnet_path    = "";
        item->params.control_image_path = "";
        item->params.control_strength   = 0;
    }

    item->params.sample_method = (sample_method_t)this->m_sampler->GetCurrentSelection();

    auto selectedwType = this->m_type->GetStringSelection();
    for (auto types : sd_gui_utils::sd_type_gui_names) {
        if (types.second == selectedwType) {
            item->params.wtype = (sd_type_t)types.first;
        }
    }

    auto selectedScheduler = this->m_scheduler->GetStringSelection();
    for (auto schedulers : sd_gui_utils::sd_scheduler_gui_names) {
        if (schedulers.second == selectedScheduler) {
            item->params.schedule = (schedule_t)schedulers.first;
            break;
        }
    }

    item->params.batch_count = this->m_batch_count->GetValue();

    item->params.width  = this->m_width->GetValue();
    item->params.height = this->m_height->GetValue();

    item->params.vae_tiling = this->m_vae_tiling->GetValue();

    item->mode = type;

    if (type == QM::GenerationMode::IMG2IMG) {
        item->initial_image = this->m_img2imgOpen->GetPath().utf8_string();
        if (this->cfg->save_all_image) {
            item->images.emplace_back(QM::QueueItemImage({item->initial_image, QM::QueueItemImageType::INITIAL}));
        }
    }

    if (item->params.seed == -1) {
        item->params.seed = sd_gui_utils::generateRandomInt(100000000, this->m_seed->GetMax());
        this->m_seed->SetValue(item->params.seed);
    }

    // add the queue item
    this->qmanager->AddItem(item);

    this->m_generate1->Enable(generate1State);
    this->m_generate2->Enable(generate2State);
    this->m_generate_upscaler->Enable(generate3State);

    this->mapp->config->Write("/last_prompt", this->m_prompt->GetValue());
    this->mapp->config->Write("/last_neg_prompt", this->m_neg_prompt->GetValue());
    this->mapp->config->Write("/last_model", this->m_model->GetStringSelection());
    this->mapp->config->Write("/last_diffusion_model", this->m_filePickerDiffusionModel->GetPath());
    this->mapp->config->Write("/last_generation_mode", wxString::FromUTF8Unchecked(QM::GenerationMode_str.at(type)));
    this->mapp->config->Flush(true);
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
    wxPersistenceManager::Get().RegisterAndRestore(dialog);
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

    // auto select the vae  for first mathcing
    for (const auto vae : this->VaeFiles) {
        if (vae.first.ends_with("/ae.safetensors") || vae.first == "ae.safetensors") {
            this->m_vae->SetStringSelection(vae.first);
            break;
        }
    }

    // autoselect clipl for bestmatch
    if (this->m_filePickerClipL->GetPath().empty()) {
        for (const auto vae : this->VaeFiles) {
            if (vae.first.ends_with("/clip_l.safetensors") || vae.first == "clip_l.safetensors" || vae.first.find("clip_l") != std::string::npos) {
                this->m_filePickerClipL->SetPath(vae.second);
                break;
            }
        }
    }

    if (this->m_filePickerT5XXL->GetPath().empty()) {
        for (const auto vae : this->VaeFiles) {
            if (vae.first.ends_with("/t5xxl.safetensors") || vae.first == "t5xxl.safetensors" || vae.first.find("t5xxl") != std::string::npos) {
                this->m_filePickerT5XXL->SetPath(vae.second);
                break;
            }
        }
    }

    if (this->m_filePickerClipL->GetPath().empty() == false && this->m_filePickerT5XXL->GetPath().empty() == false && this->m_filePickerDiffusionModel->GetPath().empty() == false) {
        this->m_generate2->Enable();
    }
}
void MainWindowUI::onFilePickerClipL(wxFileDirPickerEvent& event) {
    if (this->m_filePickerClipL->GetPath().empty() == false && this->m_filePickerT5XXL->GetPath().empty() == false && this->m_filePickerDiffusionModel->GetPath().empty() == false) {
        this->m_generate2->Enable();
        this->DisableControlNet();
    }
}
void MainWindowUI::onFilePickerT5XXL(wxFileDirPickerEvent& event) {
    if (this->m_filePickerClipL->GetPath().empty() == false && this->m_filePickerT5XXL->GetPath().empty() == false && this->m_filePickerDiffusionModel->GetPath().empty() == false) {
        this->m_generate2->Enable();
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
    auto origSize = this->m_img2img_preview->GetSize();
    this->m_img2img_preview->SetBitmap(this->AppOrigPlaceHolderBitmap);
    this->m_img2img_preview->SetSize(origSize);
    this->m_img2im_preview_img->Disable();
    this->m_delete_initial_img->Disable();
    this->m_img2imgOpen->SetPath("");
    this->m_generate2->Disable();
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
    this->m_generate_upscaler->Disable();
    // this->m_upscaler_factor->SetValue(2.0);
    this->Layout();
}

void MainWindowUI::OnUpscalerModelSelection(wxCommandEvent& event) {
    if (this->m_upscaler_model->GetCurrentSelection() != 0) {
        if (!this->m_upscaler_filepicker->GetPath().empty()) {
            this->m_generate_upscaler->Enable();
            return;
        }
    }
    this->m_generate_upscaler->Disable();
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

void MainWindowUI::OnCheckboxLoraFilter(wxCommandEvent& event) {
    auto value = this->m_modellist_filter->GetValue().utf8_string();
    this->refreshModelTable(value);
}

void MainWindowUI::OnCheckboxCheckpointFilter(wxCommandEvent& event) {
    auto value = this->m_modellist_filter->GetValue().utf8_string();
    this->refreshModelTable(value);
}

void MainWindowUI::OnModellistFilterKeyUp(wxKeyEvent& event) {
    auto value = this->m_modellist_filter->GetValue().utf8_string();
    this->refreshModelTable(value);
}

void MainWindowUI::OnDataModelActivated(wxDataViewEvent& event) {}

void MainWindowUI::OnDataModelSelected(wxDataViewEvent& event) {
    auto store = this->m_data_model_list->GetStore();
    auto row   = this->m_data_model_list->GetSelectedRow();
    if (row == wxNOT_FOUND) {
        return;
    }
    auto currentItem                       = store->GetItem(row);
    sd_gui_utils::ModelFileInfo* _item     = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(store->GetItemData(currentItem));
    sd_gui_utils::ModelFileInfo* modelinfo = this->ModelManager->getIntoPtr(_item->path);

    // download infos only when empty and sha256 is present
    if (modelinfo->civitaiPlainJson.empty() && !modelinfo->sha256.empty() && this->cfg->enable_civitai) {
        this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
    }
    this->UpdateModelInfoDetailsFromModelList(modelinfo);
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

void MainWindowUI::onSamplerSelect(wxCommandEvent& event) {
    // this->sd_params->sample_method = (sample_method_t)this->m_sampler->GetSelection();
}

void MainWindowUI::onSavePreset(wxCommandEvent& event) {
    wxTextEntryDialog dlg(this, _("Please specify a name (only alphanumeric)"));
    dlg.SetTextValidator(wxFILTER_ALPHA | wxFILTER_DIGITS);
    if (dlg.ShowModal() == wxID_OK) {
        sd_gui_utils::generator_preset preset;

        wxString preset_name = dlg.GetValue();
        preset.cfg           = this->m_cfg->GetValue();
        // do not save the seed
        // preset.seed = this->m_seed->GetValue();
        preset.clip_skip = this->m_clip_skip->GetValue();
        preset.steps     = this->m_steps->GetValue();
        preset.width     = this->m_width->GetValue();
        preset.height    = this->m_height->GetValue();

        int index = 0;
        for (auto sampler : sd_gui_utils::sample_method_str) {
            if (this->m_sampler->GetStringSelection().utf8_string() == sampler) {
                preset.sampler = (sample_method_t)index;
                break;
            }
            index++;
        }
        preset.batch = this->m_batch_count->GetValue();
        preset.name  = preset_name.utf8_string();
        preset.type  = this->m_type->GetStringSelection().utf8_string();
        if (this->m_notebook1302->GetSelection() == 1) {
            preset.mode = modes_str[QM::GenerationMode::TXT2IMG];
        }
        if (this->m_notebook1302->GetSelection() == 2) {
            preset.mode = modes_str[QM::GenerationMode::IMG2IMG];
        }

        nlohmann::json j(preset);
        std::string presetfile =
            wxString::Format("%s%s%s.json", this->cfg->presets,
                             wxString(wxFileName::GetPathSeparator()), preset.name)
                .utf8_string();

        std::ofstream file(presetfile);
        file << j;
        file.close();
        this->LoadPresets();
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
                this->m_width->SetValue(preset.second.width);
                this->m_height->SetValue(preset.second.height);
            }
            this->m_sampler->SetSelection(preset.second.sampler);
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
void MainWindowUI::ChangeGuiFromQueueItem(QM::QueueItem item) {
    if (item.id == 0) {
        return;
    }
    this->m_seed->SetValue(item.params.seed);
    this->m_width->SetValue(item.params.width);
    this->m_height->SetValue(item.params.height);
    this->m_steps->SetValue(item.params.sample_steps);
    this->m_clip_skip->SetValue(item.params.clip_skip);
    this->m_controlnetStrength->SetValue(item.params.control_strength);
    this->m_cfg->SetValue(item.params.cfg_scale);
    this->m_batch_count->SetValue(item.params.batch_count);

    if (item.mode == QM::GenerationMode::TXT2IMG) {
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

    if (!item.params.model_path.empty() && std::filesystem::exists(item.params.model_path)) {
        sd_gui_utils::ModelFileInfo model = this->ModelManager->getInfo(item.params.model_path);
        this->ChangeModelByInfo(model);
        this->m_generate2->Enable();
        if (!this->m_img2imgOpen->GetPath().empty()) {
            this->m_generate1->Enable();
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
            this->m_generate2->Enable();
        }
    }

    this->m_sampler->SetSelection(item.params.sample_method);

    if (!item.params.taesd_path.empty()) {
        int index = 0;
        for (auto taesd : this->TaesdFiles) {
            if (item.params.taesd_path == taesd.second) {
                for (unsigned int _u = 0; _u < this->m_taesd->GetCount(); ++_u) {
                    if (this->m_taesd->GetString(_u) == taesd.first) {
                        this->m_taesd->Select(_u);
                        break;
                    }
                }
            }
            index++;
        }
    }

    if (!item.params.vae_path.empty() &&
        std::filesystem::exists(item.params.vae_path)) {
        for (auto vae : this->VaeFiles) {
            if (item.params.vae_path == vae.second) {
                for (unsigned int _u = 0; _u < this->m_vae->GetCount(); ++_u) {
                    if (this->m_vae->GetString(_u) == vae.first) {
                        this->m_vae->Select(_u);
                        break;
                    }
                }
            }
        }
    }

    auto type_name = sd_gui_utils::sd_type_gui_names[item.params.wtype];
    for (unsigned int index = 0; index < this->m_type->GetCount(); ++index) {
        if (this->m_type->GetString(index) == wxString(type_name)) {
            this->m_type->Select(index);
            break;
        }
    }

    auto sheduler_name = sd_gui_utils::sd_scheduler_gui_names[item.params.schedule];

    for (unsigned int index = 0; index < this->m_scheduler->GetCount(); ++index) {
        if (this->m_scheduler->GetString(index) == wxString(sheduler_name)) {
            this->m_scheduler->Select(index);
            break;
        }
    }

    this->m_vae_tiling->SetValue(item.params.vae_tiling);

    if (!item.params.control_image_path.empty()) {
        if (std::filesystem::exists(item.params.control_image_path)) {
            this->onControlnetImageOpen(item.params.control_image_path);
        }

        for (auto cnmodel : this->ControlnetModels) {
            if (cnmodel.second == item.params.controlnet_path) {
                for (unsigned int _u = 0; _u < this->m_controlnetModels->GetCount(); ++_u) {
                    if (this->m_controlnetModels->GetString(_u) == cnmodel.first) {
                        this->m_controlnetModels->Select(_u);
                        break;
                    }
                }
            }
        }
    }
}

void MainWindowUI::UpdateModelInfoDetailsFromModelList(
    sd_gui_utils::ModelFileInfo* modelinfo) {
    this->m_model_details->DeleteAllItems();
    this->m_model_details_description->Hide();
    this->m_model_details_description->SetPage("");

    for (auto bm : this->modelImagePreviews) {
        bm->Destroy();
    }
    this->modelImagePreviews.clear();
    this->m_scrolledWindow4->FitInside();

    if (modelinfo->state == sd_gui_utils::CivitAiState::NOT_FOUND ||
        modelinfo->state == sd_gui_utils::CivitAiState::ERR) {
        return;
    }
    if (modelinfo->civitaiPlainJson.empty()) {
        return;
    }

    // this->m_data_model_url->SetLabel(modelinfo->CivitAiInfo.model.name + " " +
    // modelinfo->CivitAiInfo.name); wxString url =
    // wxString::Format("https://civitai.com/models/%d",
    // modelinfo->CivitAiInfo.modelId); this->m_data_model_url->SetURL(url);

    wxVector<wxVariant> data;

    data.push_back(wxVariant(_("Name")));
    data.push_back(wxVariant(wxString::Format("%s %s", modelinfo->CivitAiInfo.model.name, modelinfo->CivitAiInfo.name)));
    this->m_model_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("File name")));
    data.push_back(wxVariant(wxString::Format("%s", modelinfo->path)));
    this->m_model_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Hash")));
    data.push_back(wxVariant(wxString::Format("%s", modelinfo->sha256)));
    this->m_model_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Type")));
    data.push_back(
        wxVariant(wxString::Format("%s", modelinfo->CivitAiInfo.model.type)));
    this->m_model_details->AppendItem(data);
    data.clear();

    if (modelinfo->CivitAiInfo.description.empty()) {
        this->m_model_details_description->Hide();
        this->m_model_details_description->SetPage("");
    } else {
        this->m_model_details_description->Show();
        // this->m_model_details_description->SetLabelMarkup(modelinfo->CivitAiInfo.description);
        this->m_model_details_description->SetPage(
            modelinfo->CivitAiInfo.description);
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
            wxImage resized        = sd_gui_utils::cropResizeImage(wxString::FromUTF8Unchecked(img.local_path), 256, 256, wxColour(0, 0, 0, wxALPHA_TRANSPARENT), wxString::FromUTF8Unchecked(this->cfg->thumbs_path));
            wxStaticBitmap* bitmap = new wxStaticBitmap(this->m_scrolledWindow4, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(resized.GetSize()), 0);
            bitmap->Hide();
            bitmap->SetBitmap(resized);
            auto tooltip = wxString::Format(_("Resolution: %s Seed: %" PRId64 " Steps: %d"), img.meta.Size, img.meta.seed, img.meta.steps);
            this->bSizer891->Add(bitmap, 0, wxALL, 2);
            this->modelImagePreviews.emplace_back(bitmap);
            bitmap->Show();
            bitmap->Bind(wxEVT_RIGHT_UP, [img, bitmap, modelinfo, tooltip, this](wxMouseEvent& event) {
                wxMenu* menu = new wxMenu();
                menu->Append(99, tooltip);
                menu->Enable(99, false);
                if (!img.meta.prompt.empty()) {
                    menu->Append(0, "Copy prompts to text2img");
                    menu->Append(1, "Copy prompts to img2img");
                    menu->Append(2, "Send to img2img");
                    menu->Enable(1, false);
                    menu->Enable(2, false);
                    menu->Append(3, "Load available parameters");
                }
                menu->Append(10, _("Open Image on CivitAi.com"));
                menu->Enable(10, false);

                if (!img.meta.hashes.model.empty()) {
                    auto imgsModel = this->ModelManager->getByHash(img.meta.hashes.model);
                    if (!imgsModel.civitaiPlainJson.empty()) {
                        menu->Append(4, wxString::Format("Select model %s", imgsModel.name));
                    } else {
                        menu->Append(4, wxString::Format("Image's model not found: %s", img.meta.hashes.model));
                        menu->Enable(4, false);
                    }
                }

                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, img, modelinfo](wxCommandEvent& evt) {
                    auto id = evt.GetId();
                    switch (id) {
                        case 0: {
                            this->m_prompt->SetLabel(img.meta.prompt);
                            this->m_neg_prompt->SetLabel(img.meta.negativePrompt);
                        } break;
                        case 1: {
                            this->m_prompt2->SetLabel(img.meta.prompt);
                            this->m_neg_prompt2->SetLabel(img.meta.negativePrompt);
                        } break;
                        case 2: {
                            this->onimg2ImgImageOpen(img.local_path);
                            this->m_prompt2->SetLabel(img.meta.prompt);
                            this->m_neg_prompt2->SetLabel(img.meta.negativePrompt);
                        } break;
                        case 3: {
                            if (img.meta.cfgScale > 0) {
                                this->m_cfg->SetValue(img.meta.cfgScale);
                            }

                            int seed = static_cast<int>(img.meta.seed);
                            if (img.meta.seed > 0 && (seed > INT_MIN && seed < INT_MAX)) {
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

                                    // Az első és a második részt számokká alakítja
                                    int width, height;
                                    std::istringstream(part1) >> width;
                                    std::istringstream(part2) >> height;
                                    this->m_width->SetValue(width);
                                    this->m_height->SetValue(height);
                                }
                            }
                        } break;
                        case 4: {
                            this->ChangeModelByInfo(*modelinfo);
                            this->m_generate2->Enable();
                        } break;
                        case 10: {
                            // wxString url =
                            // wxString::Format("https://civitai.com/images/%d",img.i);
                            //  TODO: missing img ID from the json...
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

    this->m_scrolledWindow4->FitInside();
    this->m_scrolledWindow4->SetScrollbar(wxVERTICAL, 0, 0, 0);
}

void MainWindowUI::OnQueueItemManagerItemAdded(std::shared_ptr<QM::QueueItem> item) {
    wxVector<wxVariant> data;

    auto created_at = sd_gui_utils::formatUnixTimestampToDate(item->created_at);

    data.push_back(wxVariant(std::to_string(item->id)));
    data.push_back(wxVariant(created_at));
    data.push_back(wxVariant(modes_str[item->mode]));
    data.push_back(wxVariant(item->model));

    if (item->mode == QM::GenerationMode::UPSCALE || item->mode == QM::GenerationMode::CONVERT) {
        data.push_back(wxVariant("--"));  // sample method
        data.push_back(wxVariant("--"));  // seed
    } else {
        data.push_back(wxVariant(sd_gui_utils::sample_method_str[(int)item->params.sample_method]));
        data.push_back(wxVariant(std::to_string(item->params.seed)));
    }

    data.push_back(item->status == QM::QueueStatus::DONE ? 100 : 1);  // progressbar
    // calculate the item average speed frrom item->stats in step / seconds or seconds / step

    wxString speed = wxString::Format(item->stats.time_avg > 1.0f ? "%.2fs/it %d/%d" : "%.2fit/s %d/%d", item->stats.time_avg > 1.0f || item->stats.time_avg == 0 ? item->stats.time_avg : (1.0f / item->stats.time_avg), item->step, item->steps);
    data.push_back(wxString(speed));                               // speed
    data.push_back(wxVariant(QM::QueueStatus_str[item->status]));  // status
    data.push_back(wxVariant(item->status_message));

    auto store = this->m_joblist->GetStore();

    // only store the queue item id... not the pointer
    store->PrependItem(data, wxUIntPtr(item->id));
    this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
}

void MainWindowUI::OnQueueItemManagerItemUpdated(std::shared_ptr<QM::QueueItem> item) {
    // update column
    auto store             = this->m_joblist->GetStore();
    wxString speed         = wxString::Format(item->time > 1.0f ? "%.2fs/it %d/%d" : "%.2fit/s %d/%d", item->time > 1.0f || item->time == 0 ? item->time : (1.0f / item->time), item->step, item->steps);
    int progressCol        = this->m_joblist->GetColumnCount() - 4;
    int speedCol           = this->m_joblist->GetColumnCount() - 3;
    float current_progress = 0.f;

    if (item->step > 0 && item->steps > 0) {
        current_progress = 100.f * (static_cast<float>(item->step) /
                                    static_cast<float>(item->steps));
    }
    if (item->step == item->steps) {
        current_progress = 100.f;
    }

    for (unsigned int i = 0; i < store->GetItemCount(); i++) {
        auto currentItem                     = store->GetItem(i);
        int id                               = store->GetItemData(currentItem);
        std::shared_ptr<QM::QueueItem> qitem = this->qmanager->GetItemPtr(id);
        if (qitem->id == item->id) {
            store->SetValueByRow(static_cast<int>(current_progress), i, progressCol);
            store->SetValueByRow(speed, i, speedCol);
            store->RowValueChanged(i, progressCol);
            store->RowValueChanged(i, speedCol);
            this->m_joblist->Refresh();
            this->m_joblist->Update();
            break;
        }
    }

    return;
}

MainWindowUI::~MainWindowUI() {
    if (this->civitwindow != nullptr) {
        this->civitwindow->Destroy();
    }

    this->extProcessNeedToRun = false;

    if (this->processCheckThread != nullptr && this->processCheckThread->joinable()) {
        this->processCheckThread->join();
    }

    if (this->subprocess != nullptr && subprocess_alive(this->subprocess) != 0) {
        std::cout << "Terminating processs" << std::endl;
        int result = 0;
        result     = subprocess_terminate(subprocess);
        if (0 != result) {
            std::cerr << "Can not terminate extprocess" << std::endl;
        }

        std::cout << "Join processs" << std::endl;
        result = subprocess_join(subprocess, NULL);
        if (0 != result) {
            std::cerr << "Can not join extprocess" << std::endl;
        }

        std::cout << "Destroy processs" << std::endl;
        result = subprocess_destroy(subprocess);
        if (0 != result) {
            std::cerr << "Can not destroy extprocess" << std::endl;
        }
    }
    if (this->processHandleOutput != nullptr && this->processHandleOutput->joinable()) {
        this->processHandleOutput->join();
    }

    for (auto& threadPtr : threads) {
        if (threadPtr->joinable()) {
            threadPtr->join();
        }
        delete threadPtr;
    }

    this->JobTableItems.clear();

    for (auto& v : this->voids) {
        delete v;
    }

    this->voids.clear();

    for (auto& b : this->modelImagePreviews) {
        delete b;
    }
    this->modelImagePreviews.clear();

    for (auto& b : this->jobImagePreviews) {
        delete b;
    }
    this->jobImagePreviews.clear();
    delete this->cfg;
    this->TaskBar->Destroy();
    if (logfile.is_open()) {
        logfile.close();
    }
}

void MainWindowUI::loadControlnetList() {
    this->LoadFileList(sd_gui_utils::DirTypes::CONTROLNET);
}

void MainWindowUI::loadEmbeddingList() {
    this->LoadFileList(sd_gui_utils::DirTypes::EMBEDDING);
}

void MainWindowUI::OnPopupClick(wxCommandEvent& evt) {
    auto tu = evt.GetId();

    // 100 for queueitem list table
    if (tu < 100) {
        wxDataViewListStore* store = this->m_joblist->GetStore();
        auto currentItem           = this->m_joblist->GetCurrentItem();
        auto currentRow            = this->m_joblist->GetSelectedRow();
        if (currentRow == wxNOT_FOUND) {
            return;
        }
        int id                               = store->GetItemData(currentItem);
        std::shared_ptr<QM::QueueItem> qitem = this->qmanager->GetItemPtr(id);

        /*
                1 Copy and queue
                2 copy to text2img
                3 copy prompts to text2image
                4 copy prompts to img2img
                5 Details
                99 delete
        */

        switch (tu) {
            case 1:
                this->qmanager->Duplicate(qitem);
            case 2:
                this->ChangeGuiFromQueueItem(*qitem);
                break;
            case 3:
                this->m_prompt->SetValue(wxString::FromUTF8Unchecked(qitem->params.prompt));
                this->m_neg_prompt->SetValue(wxString::FromUTF8Unchecked(qitem->params.negative_prompt));
                break;
            case 4:
                this->m_prompt2->SetValue(wxString::FromUTF8Unchecked(qitem->params.prompt));
                this->m_neg_prompt2->SetValue(wxString::FromUTF8Unchecked(qitem->params.negative_prompt));
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
                    auto model = this->ModelManager->getInfo(qitem->params.model_path);
                    this->ChangeModelByInfo(model);
                }
            } break;
            case 6: {
                this->m_upscaler_filepicker->SetPath(wxString::FromUTF8Unchecked(qitem->images.back().pathname));
                this->onUpscaleImageOpen(wxString::FromUTF8Unchecked(qitem->images.back().pathname));
                this->m_notebook1302->SetSelection(3);  // switch to the upscaler
            } break;
            case 7: {
                this->m_img2imgOpen->SetPath(wxString::FromUTF8Unchecked(qitem->images.back().pathname));
                this->onimg2ImgImageOpen(wxString::FromUTF8Unchecked(qitem->images.back().pathname));
                this->m_notebook1302->SetSelection(2);  // switch to the img2img
            } break;
            case 8: {
                if (qitem->status == QM::QueueStatus::PAUSED) {
                    this->qmanager->UnPauseItem(qitem);
                    return;
                }
                if (qitem->status == QM::QueueStatus::PENDING) {
                    this->qmanager->PauseItem(qitem);
                    return;
                }
            } break;
            case 99: {
                if (this->qmanager->DeleteJob(qitem->id)) {
                    store->DeleteItem(currentRow);
                    this->m_static_number_of_jobs->SetLabel(wxString::Format(_("Number of jobs: %d"), this->m_joblist->GetItemCount()));
                }
            } break;
        }
        return;
    }
    /*
    101   lora txt2img
    102   lora txt2img neg
    103   lora img2img
    104   lora img2img neg
    */
    if (tu >= 100 && tu < 300) {
        wxDataViewListStore* store = this->m_data_model_list->GetStore();
        int currow                 = this->m_data_model_list->GetSelectedRow();
        if (currow == wxNOT_FOUND) {
            return;
        }
        auto currentItem = store->GetItem(currow);

        sd_gui_utils::ModelFileInfo* modelinfo =
            reinterpret_cast<sd_gui_utils::ModelFileInfo*>(store->GetItemData(currentItem));
        auto shortname =
            std::filesystem::path(modelinfo->name).replace_extension().string();
        switch (tu) {
            case 100:
                this->threads.emplace_back(
                    new std::thread(&MainWindowUI::threadedModelHashCalc, this, this->GetEventHandler(), modelinfo));
                break;
            case 101:
                this->m_prompt->SetValue(wxString::Format(
                    "%s <lora:%s:0.5>", this->m_prompt->GetValue(), shortname));
                break;
            case 102:
                this->m_neg_prompt->SetValue(wxString::Format(
                    "%s <lora:%s:0.5>", this->m_neg_prompt->GetValue(), shortname));
                break;
            case 103:
                this->m_prompt2->SetValue(wxString::Format(
                    "%s <lora:%s:0.5>", this->m_prompt2->GetValue(), shortname));
                break;
            case 104:
                this->m_neg_prompt2->SetValue(wxString::Format(
                    "%s <lora:%s:0.5>", this->m_neg_prompt2->GetValue(), shortname));
                break;
            case 105:
                this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
                break;
            case 111:
                this->m_prompt->SetValue(
                    wxString::Format("%s %s", this->m_prompt->GetValue(), shortname));
                break;
            case 112:
                this->m_neg_prompt->SetValue(
                    wxString::Format("%s %s", this->m_neg_prompt->GetValue(), shortname));
                break;
            case 113:
                this->m_prompt2->SetValue(
                    wxString::Format("%s %s", this->m_prompt2->GetValue(), shortname));
                break;
            case 114:
                this->m_neg_prompt2->SetValue(wxString::Format("%s %s", this->m_neg_prompt2->GetValue(), shortname));
                break;
            case 115:
                this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
                break;
            case 199: {
                //
                wxString url = wxString::Format("https://civitai.com/models/%d", modelinfo->CivitAiInfo.modelId);
                wxLaunchDefaultBrowser(url);
            } break;
            case 200: {
                this->ChangeModelByInfo(*modelinfo);
            } break;
            // model converter
            case 201: {
                this->PrepareModelConvert(modelinfo);
            } break;
        }
        return;
    }
}

void MainWindowUI::loadVaeList() {
    this->LoadFileList(sd_gui_utils::DirTypes::VAE);
}

void MainWindowUI::loadEsrganList() {
    this->LoadFileList(sd_gui_utils::DirTypes::ESRGAN);
}

void MainWindowUI::refreshModelTable(const wxString& filter) {
    auto types = sd_gui_utils::DirTypes::LORA | sd_gui_utils::DirTypes::CHECKPOINT | sd_gui_utils::DirTypes::EMBEDDING;

    if (!this->m_checkbox_lora_filter->IsChecked()) {
        types &= ~sd_gui_utils::DirTypes::LORA;
    }

    if (!this->m_checkbox_filter_checkpoints->IsChecked()) {
        types &= ~sd_gui_utils::DirTypes::CHECKPOINT;
    }
    if (!this->m_checkbox_filter_embeddings->IsChecked()) {
        types &= ~sd_gui_utils::DirTypes::EMBEDDING;
    }

    auto store = this->m_data_model_list->GetStore();

    std::vector<sd_gui_utils::ModelFileInfo> list = this->ModelManager->getList();
    std::vector<sd_gui_utils::ModelFileInfo> filtered_list;

    std::copy_if(list.begin(), list.end(), std::back_inserter(filtered_list),
                 [types](const sd_gui_utils::ModelFileInfo& info) {
                     return sd_gui_utils::filterByModelType(info.model_type, types);
                 });

    if (!filter.empty()) {
        std::vector<sd_gui_utils::ModelFileInfo> tmp_list;
        std::copy_if(
            filtered_list.begin(), filtered_list.end(),
            std::back_inserter(tmp_list),
            [types, filter](const sd_gui_utils::ModelFileInfo& model) {
                auto name = model.name;
                auto path = model.path;
                std::transform(name.begin(), name.end(), name.begin(),
                               [](unsigned char c) {
                                   return std::tolower(c);
                               });
                std::transform(path.begin(), path.end(), path.begin(),
                               [](unsigned char c) {
                                   return std::tolower(c);
                               });

                return (path.find(filter) != std::string::npos ||
                        name.find(filter) != std::string::npos) ||
                       ((!model.civitaiPlainJson.empty() &&
                         model.hash_progress_size != 0) &&
                        (model.CivitAiInfo.name.find(filter) != std::string::npos ||
                         model.CivitAiInfo.model.name.find(filter) !=
                             std::string::npos));
            });
        filtered_list = tmp_list;
    }

    store->DeleteAllItems();
    for (auto ele : filtered_list) {
        wxVector<wxVariant> itemData;
        itemData.push_back(ele.name);                                    // name
        itemData.push_back(ele.size_f);                                  // size
        itemData.push_back(sd_gui_utils::dirtypes_str[ele.model_type]);  // type
        itemData.push_back(ele.sha256.substr(0, 10));                    // autov2 hash
        itemData.push_back(
            sd_gui_utils::civitai_state_str[ele.state]);  // civitai state
        if (ele.hash_progress_size > 0) {
            int current_progress = 0;
            if (ele.hash_fullsize != 0) {
                current_progress = static_cast<int>((ele.hash_progress_size * 100) / ele.hash_fullsize);
            }
            itemData.push_back(current_progress);  // progress bar
        } else {
            itemData.push_back(ele.sha256.empty() ? 0 : 100);  // progress bar
        }
        store->AppendItem(itemData, (wxUIntPtr)this->ModelManager->getIntoPtr(
                                        ele.path));  // setClientData
    }
    // remove which not needed
    this->m_data_model_list->Refresh();
}

void MainWindowUI::loadTaesdList() {
    this->LoadFileList(sd_gui_utils::DirTypes::TAESD);
}

std::string MainWindowUI::paramsToImageComment(QM::QueueItem myItem, sd_gui_utils::ModelFileInfo modelInfo) {
    auto modelPath = std::filesystem::path(modelInfo.path);

    std::string comment = "charset=Unicode " + myItem.params.prompt;

    if (!myItem.params.negative_prompt.empty()) {
        comment = comment + wxString::Format("\nNegative prompt: %s", myItem.params.negative_prompt).utf8_string();
    }
    comment =
        comment +
        wxString::Format(
            "\nSteps: %d, Seed: %" PRId64
            ", Sampler: %s, CFG scale: %.1f, Size: %dx%d, Parser: sd.cpp, Model: "
            "%s, Model hash: %s, Backend: sd.cpp, App: %s, Operations: %s",
            myItem.params.sample_steps, myItem.params.seed,
            sd_gui_utils::sample_method_str[(int)myItem.params.sample_method],
            myItem.params.cfg_scale, myItem.params.width, myItem.params.height,
            modelPath.filename().replace_extension().string(),
            modelInfo.sha256.substr(
                0, 10),  // autov2 hash (the first 10 char from sha256 :) )
            PROJECT_NAME, modes_str[(int)myItem.mode]);

    if (!myItem.params.vae_path.empty()) {
        auto vae_path = std::filesystem::path(myItem.params.vae_path);
        comment       = comment + wxString::Format(" VAE: %s", vae_path.filename().replace_extension().string());
    }

    return comment;
}

template <typename T>
inline void MainWindowUI::SendThreadEvent(wxEvtHandler* eventHandler, QM::QueueEvents eventType, const T& payload, std::string text) {
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
void MainWindowUI::LoadFileList(sd_gui_utils::DirTypes type) {
    std::string basepath;

    switch (type) {
        case sd_gui_utils::DirTypes::VAE:
            this->VaeFiles.clear();
            this->m_vae->Clear();
            this->m_vae->Append(_("-none-"));
            this->m_vae->Select(0);
            basepath = this->cfg->vae;
            break;
        case sd_gui_utils::DirTypes::LORA:
            basepath = this->cfg->lora;
            break;
        case sd_gui_utils::DirTypes::CHECKPOINT:
            this->ModelFiles.clear();
            this->ModelFilesIndex.clear();
            this->m_model->Clear();
            this->m_model->Append(_("-none-"));
            this->m_model->Select(0);
            basepath = this->cfg->model;
            break;
        case sd_gui_utils::DirTypes::PRESETS:
            this->Presets.clear();
            this->m_preset_list->Clear();
            this->m_preset_list->Append(_("-none-"));
            this->m_preset_list->Select(0);
            basepath = this->cfg->presets;
            break;
        case sd_gui_utils::DirTypes::TAESD: {
            this->m_taesd->Clear();
            this->m_taesd->Append(_("-none-"));
            this->m_taesd->Select(0);
            basepath = this->cfg->taesd;
        } break;
        case sd_gui_utils::DirTypes::EMBEDDING: {
            basepath = this->cfg->embedding;
        } break;
        case sd_gui_utils::DirTypes::CONTROLNET:
            this->m_controlnetModels->Clear();
            this->m_controlnetModels->Append(_("-none-"));
            this->m_controlnetModels->Select(0);
            basepath = this->cfg->controlnet;
            break;
        case sd_gui_utils::DirTypes::ESRGAN:
            this->m_upscaler_model->Clear();
            this->m_upscaler_model->Append(_("-none-"));
            this->m_upscaler_model->Select(0);
            basepath = this->cfg->esrgan;
            break;
        default:
            return;
            break;
    }

    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(basepath)) {
        if (!dir_entry.exists() || !dir_entry.is_regular_file() ||
            !dir_entry.path().has_extension()) {
            continue;
        }

        std::filesystem::path path = dir_entry.path();

        std::string ext = path.extension().string();

        if (type == sd_gui_utils::DirTypes::CHECKPOINT) {
            if (std::find(CHECKPOINT_FILE_EXTENSIONS.begin(), CHECKPOINT_FILE_EXTENSIONS.end(), ext) == CHECKPOINT_FILE_EXTENSIONS.end()) {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::LORA) {
            if (std::find(LORA_FILE_EXTENSIONS.begin(), LORA_FILE_EXTENSIONS.end(), ext) == LORA_FILE_EXTENSIONS.end()) {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::EMBEDDING) {
            if (std::find(EMBEDDING_FILE_EXTENSIONS.begin(), EMBEDDING_FILE_EXTENSIONS.end(), ext) == EMBEDDING_FILE_EXTENSIONS.end()) {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::VAE) {
            if (std::find(VAE_FILE_EXTENSIONS.begin(), VAE_FILE_EXTENSIONS.end(), ext) == VAE_FILE_EXTENSIONS.end()) {
                continue;
            }
        }

        if (type == sd_gui_utils::DirTypes::PRESETS) {
            if (ext != ".json") {
                continue;
            }
        }

        if (type == sd_gui_utils::DirTypes::TAESD) {
            if (std::find(TAESD_FILE_EXTENSIONS.begin(), TAESD_FILE_EXTENSIONS.end(), ext) == TAESD_FILE_EXTENSIONS.end()) {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::ESRGAN) {
            if (std::find(ESRGAN_FILE_EXTENSIONS.begin(), ESRGAN_FILE_EXTENSIONS.end(), ext) == ESRGAN_FILE_EXTENSIONS.end()) {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::CONTROLNET) {
            if (std::find(CONTROLNET_FILE_EXTENSIONS.begin(), CONTROLNET_FILE_EXTENSIONS.end(), ext) == CONTROLNET_FILE_EXTENSIONS.end()) {
                continue;
            }
        }
        std::string name = path.filename().string();
        // prepend the subdirectory to the modelname
        // // wxFileName::GetPathSeparator()
        std::string path_name = path.parent_path().string();

        sd_gui_utils::replace(path_name, basepath, "");
        if (!path_name.empty()) {
            name = path_name.substr(1) + "/" + name;
        }

        if (type == sd_gui_utils::DirTypes::CHECKPOINT) {
            this->ModelFiles.emplace(name, dir_entry.path().string());
            this->ModelFilesIndex[name] = this->ModelFiles.size();
            this->ModelManager->addModel(dir_entry.path().string(), type, name);

            sd_gui_utils::ModelFileInfo* _minfo = this->ModelManager->getIntoPtr(dir_entry.path().string());

            this->m_model->Append(name, (void*)_minfo);
        }
        if (type == sd_gui_utils::DirTypes::LORA) {
            this->LoraFiles.emplace(name, dir_entry.path().string());
            this->ModelManager->addModel(dir_entry.path().string(), type, name);
        }
        if (type == sd_gui_utils::DirTypes::EMBEDDING) {
            this->EmbeddingFiles.emplace(name, dir_entry.path().string());
            this->ModelManager->addModel(dir_entry.path().string(), type, name);
        }
        if (type == sd_gui_utils::DirTypes::ESRGAN) {
            this->EsrganFiles.emplace(name, dir_entry.path().string());
            this->m_upscaler_model->Append(name);
        }
        if (type == sd_gui_utils::DirTypes::VAE) {
            this->m_vae->Append(name);
            this->VaeFiles.emplace(name, dir_entry.path().string());
        }
        if (type == sd_gui_utils::DirTypes::PRESETS) {
            sd_gui_utils::generator_preset preset;
            std::ifstream f(path.string());
            try {
                nlohmann::json data = nlohmann::json::parse(f);
                preset              = data;
                preset.path         = path.string();
                this->m_preset_list->Append(preset.name);
                this->Presets.emplace(preset.name, preset);
            } catch (const std::exception& e) {
                std::remove(path.string().c_str());
            }
        }
        if (type == sd_gui_utils::DirTypes::TAESD) {
            this->m_taesd->Append(name);
            this->TaesdFiles.emplace(name, dir_entry.path().string());
        }
        if (type == sd_gui_utils::DirTypes::CONTROLNET) {
            this->m_controlnetModels->Append(name);
            this->ControlnetModels.emplace(name, dir_entry.path().string());
        }
    }

    if (type == sd_gui_utils::DirTypes::CHECKPOINT) {
        this->writeLog(wxString::Format(_("Loaded checkpoints: %" PRIuMAX "\n"), this->ModelFiles.size()));
        if (this->ModelFiles.size() > 0) {
            this->m_model->Enable();
        } else {
            this->m_model->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::LORA) {
        this->writeLog(wxString::Format(_("Loaded Loras: %" PRIuMAX "\n"), this->LoraFiles.size()));
    }
    if (type == sd_gui_utils::DirTypes::EMBEDDING) {
        this->writeLog(wxString::Format(_("Loaded embeddings: %" PRIuMAX "\n"), this->EmbeddingFiles.size()));
    }
    if (type == sd_gui_utils::DirTypes::VAE) {
        this->writeLog(wxString::Format(_("Loaded vaes: %" PRIuMAX "\n"), this->VaeFiles.size()));
        if (this->VaeFiles.size() > 0) {
            this->m_vae->Enable();
        } else {
            this->m_vae->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::PRESETS) {
        this->writeLog(wxString::Format(_("Loaded presets: %" PRIuMAX "\n"), this->Presets.size()));
        if (this->Presets.size() > 0) {
            this->m_preset_list->Enable();
        } else {
            this->m_preset_list->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::TAESD) {
        this->writeLog(wxString::Format(_("Loaded taesd: %" PRIuMAX "\n"), this->TaesdFiles.size()));
        if (this->TaesdFiles.size() > 0) {
            this->m_taesd->Enable();
        } else {
            this->m_taesd->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::ESRGAN) {
        this->writeLog(wxString::Format(_("Loaded esrgan: %" PRIuMAX "\n"), this->EsrganFiles.size()));
        if (this->EsrganFiles.size() > 0) {
            this->m_upscaler_model->Enable();
        } else {
            this->m_upscaler_model->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::CONTROLNET) {
        this->writeLog(wxString::Format(_("Loaded controlnet: %" PRIuMAX "\n"), this->ControlnetModels.size()));
        if (this->ControlnetModels.size() > 0) {
            this->m_controlnetModels->Enable();
        } else {
            this->m_controlnetModels->Disable();
        }
    }
}
void MainWindowUI::loadLoraList() {
    this->LoadFileList(sd_gui_utils::DirTypes::LORA);
}

void MainWindowUI::OnCloseSettings(wxCloseEvent& event) {
    this->mapp->initConfig();
    this->initConfig();
    this->settingsWindow->Destroy();
    if (this->cfg->enable_civitai == false && this->m_civitai->IsShown()) {
        this->m_civitai->Hide();
        if (this->civitwindow != nullptr) {
            this->civitwindow->Destroy();
            this->civitwindow = nullptr;
        }
    }
    if (this->cfg->enable_civitai == true && this->m_civitai->IsShown() == false) {
        this->m_civitai->Show();
    }

    std::cout << "restart window with lang: " << this->cfg->language << std::endl;
    this->mapp->ReloadMainWindow(this->cfg->language);

    //        this->Thaw();
    //        this->Show();
}
void MainWindowUI::OnCloseCivitWindow(wxCloseEvent& event) {
    this->civitwindow->Destroy();
    this->civitwindow = nullptr;
}

void MainWindowUI::imageCommentToGuiParams(std::map<std::string, std::string> params, SDMode mode) {
    bool modelFound = false;
    for (auto item : params) {
        // sampler
        if (item.first == "sampler" || item.first == "Sampler") {
            /// we default sampler is euler_a :)
            /// in automatic, the default is unipc, we dont have it
            if (item.second == "Default") {
                this->m_sampler->Select(0);
                continue;
            }
            unsigned int index = 0;
            for (auto sampler : sd_gui_utils::sample_method_str) {
                if (sampler == item.second) {
                    this->m_sampler->Select(index);
                    break;
                }
                index++;
            }
        }

        if (item.first == "scheduler" || item.first == "Scheduler") {
            if (item.second == "Default") {
                this->m_scheduler->Select(0);
                continue;
            }
            unsigned int index = 0;
            for (auto scheduler : sd_gui_utils::sd_scheduler_gui_names) {
                if (scheduler.second == item.second) {
                    this->m_scheduler->Select(index);
                    break;
                }
                index++;
            }
        }
        // the seed
        if (item.first == "seed" || item.first == "Seed") {
            if (this->m_seed->GetMax() >= std::atoi(item.second.c_str())) {
                this->m_seed->SetValue(std::atoi(item.second.c_str()));
            }
        }
        if (item.first == "size" || item.first == "Size") {
            size_t pos = item.second.find("x");
            std::string w, h;
            w = item.second.substr(0, pos);
            h = item.second.substr(pos + 1);
            this->m_width->SetValue(std::atoi(w.c_str()));
            this->m_height->SetValue(std::atoi(h.c_str()));
        }
        if (item.first == "steps") {
            this->m_steps->SetValue(std::atoi(item.second.c_str()));
        }
        if (item.first == "cfgscale") {
            this->m_cfg->SetValue(
                static_cast<double>(std::atof(item.second.c_str())));
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
        // first check by hash
        if (item.first == "modelhash" && !modelFound) {
            auto check = this->ModelManager->getByHash(item.second);
            if (!check.path.empty()) {
                this->m_model->Select(this->ModelFilesIndex[check.name]);
                modelFound = true;
                continue;
            }
        }
        if (item.first == "model" && !modelFound) {
            // get by name
            auto check = this->ModelManager->getInfoByName(item.second);
            if (!check.path.empty()) {
                this->m_model->Select(this->ModelFilesIndex[check.name]);
                modelFound = true;
                continue;
            }
            // search by name
            auto check2 = this->ModelManager->findInfoByName(item.second);
            if (!check.path.empty()) {
                this->m_model->Select(this->ModelFilesIndex[check2.name]);
                modelFound = true;
                continue;
            }
        }

        if (item.first == "vae") {
            unsigned int index = 0;
            for (auto vae : this->VaeFiles) {
                if (vae.second.find(item.second) != std::string::npos) {
                    this->m_vae->Select(index);
                    continue;
                }
                index++;
            }
        }
    }
}

void MainWindowUI::onimg2ImgImageOpen(const wxString& file) {
    if (file.length() < 1) {
        std::cerr << __FILE__ << ":" << __LINE__ << " onimg2ImgImageOpen: file is empty" << std::endl;
        return;
    }
    wxImage img;
    if (img.LoadFile(file)) {
        auto origSize = this->m_img2img_preview->GetSize();

        auto preview = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(), origSize.GetHeight());

        this->m_img2img_preview->SetScaleMode(wxStaticBitmap::Scale_AspectFill);
        this->m_img2img_preview->SetBitmap(preview);
        this->m_img2img_preview->SetSize(origSize);

        this->m_img2im_preview_img->Enable();
        this->m_delete_initial_img->Enable();
        if (this->m_model->GetSelection() > 0) {
            this->m_generate1->Enable();
        }
        this->m_width->SetValue(img.GetWidth());
        this->m_height->SetValue(img.GetHeight());
        this->m_width->Disable();
        this->m_height->Disable();
        this->m_img2imgOpen->SetPath(file);

        // png not working... yet...
        if (img.GetType() == wxBITMAP_TYPE_PNG) {
            return;
        }

        try {
            auto image = Exiv2::ImageFactory::open(file.utf8_string());
            if (!image->good()) {
                return;
            }
            image->readMetadata();
            Exiv2::ExifData& exifData = image->exifData();
            if (!exifData.empty()) {
                std::string ex;
                Exiv2::ExifData::iterator it;
                std::string usercomment;
                for (it = exifData.begin(); it != exifData.end(); ++it) {
                    if (it->key() == "Exif.Photo.UserComment") {
                        usercomment = it->getValue()->toString();
                        if (!usercomment.empty()) {
                            std::map<std::string, std::string> getParams = sd_gui_utils::parseExifPrompts(usercomment);
                            this->imageCommentToGuiParams(getParams, SDMode::IMG2IMG);
                            break;
                        }
                    }
                }
            }
        } catch (Exiv2::Error& e) {
            std::cerr << "Err: " << e.what() << std::endl;
        }
    } else {
        wxMessageBox(_("Can not open image!"));
    }
}

void MainWindowUI::onUpscaleImageOpen(const wxString& file) {
    if (file.length() < 1) {
        std::cerr << __FILE__ << ":" << __LINE__ << " onUpscaleImageOpen: file is empty" << std::endl;
        return;
    }
    wxImage img;
    if (img.LoadFile(file)) {
        this->m_upscaler_filepicker->SetPath(file);
        auto origSize = this->m_upscaler_source_image->GetSize();
        auto preview  = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(),
                                                           origSize.GetHeight());

        this->m_generate_upscaler->Enable();
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

        if (this->m_upscaler_model->GetCurrentSelection() != 0) {
            this->m_generate_upscaler->Enable();
        } else {
            this->m_generate_upscaler->Disable();
        }
    }
}

void MainWindowUI::StartGeneration(std::shared_ptr<QM::QueueItem> myJob) {
    if (this->disableExternalProcessHandling == false) {
        if (subprocess_alive(this->subprocess) == 0) {
            wxMessageDialog errorDialog(NULL, wxT("An error occurred while starting the generation process."), wxT("Error"), wxOK | wxICON_ERROR);
            myJob->status_message = _("Error accessing to the background process. Please try again.");
            // this->qmanager->SendEventToMainWindow(QM::QueueEvents::ITEM_FAILED, myJob);
            this->qmanager->SetStatus(QM::QueueStatus::FAILED, myJob);
            errorDialog.ShowModal();
            return;
        }
    }

    try {
        myJob->updated_at = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        this->qmanager->SetStatus(QM::QueueStatus::PENDING, myJob);
        nlohmann::json j = *myJob;
        std::string msg  = j.dump();
        this->sharedMemory->write(msg.data(), msg.size());

        //  this->qmanager->SendEventToMainWindow(QM::QueueEvents::ITEM_GENERATION_STARTED, myJob);

    } catch (const std::exception& e) {
        std::cerr << __FILE__ << ":" << __LINE__ << e.what() << std::endl;
    }
}

void MainWindowUI::loadSamplerList() {
    this->m_sampler->Clear();
    for (auto sampler : sd_gui_utils::sample_method_str) {
        int _u = this->m_sampler->Append(sampler);

        if (std::string(sampler) == "euler_a") {
            this->m_sampler->Select(_u);
        }
    }
}
void MainWindowUI::loadTypeList() {
    this->m_type->Clear();
    unsigned int selected = 0;
    for (auto type : sd_gui_utils::sd_type_gui_names) {
        auto _z = this->m_type->Append(type.second);
        if (type.second == "Count") {
            selected = _z;
        }
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

void MainWindowUI::loadSchedulerList() {
    this->m_scheduler->Clear();
    unsigned int selected = 0;

    for (auto type : sd_gui_utils::sd_scheduler_gui_names) {
        auto _z = this->m_scheduler->Append(type.second);
        if (type.second == "Default") {
            selected = _z;
        }
    }
    this->m_scheduler->Select(selected);
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

void MainWindowUI::OnQueueItemManagerItemStatusChanged(std::shared_ptr<QM::QueueItem> item) {
    auto store = this->m_joblist->GetStore();

    int statusCol     = this->m_joblist->GetColumnCount() - 2;
    int statusTextCol = this->m_joblist->GetColumnCount() - 1;

    for (unsigned int i = 0; i < store->GetItemCount(); i++) {
        auto currentItem                     = store->GetItem(i);
        int id                               = store->GetItemData(currentItem);
        std::shared_ptr<QM::QueueItem> qitem = this->qmanager->GetItemPtr(id);
        if (qitem->id == item->id) {
            store->SetValueByRow(wxVariant(QM::QueueStatus_str[item->status]), i, statusCol);
            store->RowValueChanged(i, statusCol);

            store->SetValueByRow(wxVariant(item->status_message), i, statusTextCol);
            store->RowValueChanged(i, statusTextCol);

            break;
        }
    }
    this->m_joblist->Refresh();
    this->m_joblist->Update();
}

void MainWindowUI::OnThreadMessage(wxThreadEvent& e) {
    if (e.GetSkipped() == false) {
        e.Skip();
    }
    auto msg = e.GetString().utf8_string();

    std::string token                      = msg.substr(0, msg.find(":"));
    std::string content                    = msg.substr(msg.find(":") + 1);
    sd_gui_utils::ThreadEvents threadEvent = (sd_gui_utils::ThreadEvents)std::stoi(token);

    if (threadEvent == sd_gui_utils::ThreadEvents::QUEUE) {
        // only numbers here...
        QM::QueueEvents event = (QM::QueueEvents)std::stoi(content);

        std::shared_ptr<QM::QueueItem> item = e.GetPayload<std::shared_ptr<QM::QueueItem>>();

        if (QM::QueueEvents::ITEM_ADDED != event) {
            this->qmanager->UpdateItem(*item);
        }

        wxString title;
        wxString message;

        switch (event) {
                // new item added
            case QM::QueueEvents::ITEM_ADDED:
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->OnQueueItemManagerItemAdded(item);
                break;
                // item status changed
            case QM::QueueEvents::ITEM_STATUS_CHANGED:
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->OnQueueItemManagerItemStatusChanged(item);
                break;
                // item updated... -> set the progress bar in the queue
            case QM::QueueEvents::ITEM_UPDATED:
                this->OnQueueItemManagerItemUpdated(item);
                break;
            case QM::QueueEvents::ITEM_START:  // this is just the item start, if no mode
                // loaded, then wil trigger model load
                // event
                {
                    this->UpdateJobInfoDetailsFromJobQueueList(item);
                    this->StartGeneration(item);
                    message = wxString::Format(_("%s is just stared to generate %d images\nModel: %s"), modes_str[item->mode], item->params.batch_count, item->model);

                    if (item->mode == QM::GenerationMode::UPSCALE) {
                        title   = _("Upscaling started");
                        message = wxString::Format(_("Upscaling the image is started: %s\nModel: %s"), item->initial_image, item->model);
                    } else if (item->mode == QM::GenerationMode::CONVERT) {
                        title   = _("Conversion started");
                        message = wxString::Format(_("Conversion the model is started: %s\nModel: %s"), item->initial_image, item->model);
                    } else {
                        if (item->params.batch_count > 1) {
                            title = wxString::Format(_("%d images generation started"), item->params.batch_count);
                        } else {
                            title   = _("One image generation started!");
                            message = wxString::Format(_("%s is just started to generate the image\nModel: %s"), modes_str[item->mode], item->model);
                        }
                    }
                    this->ShowNotification(title, message);
                }
                break;
            case QM::QueueEvents::ITEM_FINISHED: {
                // update again
                this->SendThreadEvent(this->GetEventHandler(), QM::QueueEvents::ITEM_UPDATED, item);
                this->m_stop_background_process->Enable();
                this->jobsCountSinceSegfault++;
                this->stepsCountSinceSegfault += item->steps;
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                message = wxString::Format(_("%s is just finished to generate %d images\nModel: %s"), modes_str[item->mode], item->params.batch_count, item->model);
                if (item->mode == QM::GenerationMode::UPSCALE) {
                    title   = _("Upscaling done");
                    message = wxString::Format(_("Upscaling the image is done: %s\nModel: %s"), item->initial_image, item->model);
                } else if (item->mode == QM::GenerationMode::CONVERT) {
                    title   = _("Conversion done");
                    message = wxString::Format(_("Conversion the model is done: %s\nModel: %s"), item->model, item->params.output_path);
                    this->loadModelList();
                } else {
                    if (item->params.batch_count > 1) {
                        title = wxString::Format(_("%d images generation done"), item->params.batch_count);
                    } else {
                        title   = _("Image generation done!");
                        message = wxString::Format(_("%s is just finished to generate the image with model: %s"), modes_str[item->mode], item->model);
                    }
                }
                this->ShowNotification(title, message);
                this->writeLog(message);
                {
                    if (this->jobsCountSinceSegfault > 0) {
                        std::lock_guard<std::mutex> lock(this->taskBarMutex);
                        wxString msg;
                        if (this->jobsCountSinceSegfault.load() > 1) {
                            msg = _("%d jobs and %d steps without a segfault");
                        } else {
                            msg = _("%d job and %d step without a segfault");
                        }
                        this->m_statusBar166->SetStatusText(wxString::Format(msg, this->jobsCountSinceSegfault.load(), this->stepsCountSinceSegfault.load()), 1);
                    }
                }
            } break;
            case QM::QueueEvents::ITEM_MODEL_LOADED:  // MODEL_LOAD_DONE
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                this->writeLog(wxString::Format(_("Model loaded: %s\n"), item->params.model_path.empty() ? item->params.diffusion_model_path : item->params.model_path));
                break;
            case QM::QueueEvents::ITEM_MODEL_LOAD_START:  // MODEL_LOAD_START
                this->writeLog(wxString::Format(_("Model load started: %s\n"), item->params.model_path.empty() ? item->params.diffusion_model_path : item->params.model_path));
                break;
            case QM::QueueEvents::ITEM_MODEL_FAILED:  // MODEL_LOAD_ERROR
                this->writeLog(wxString::Format(_("Model load failed: %s\n"), item->params.model_path.empty() ? item->params.diffusion_model_path : item->params.model_path));
                title   = _("Model load failed");
                message = wxString::Format(_("The '%s' just failed to load... for more details please see the logs!"), item->params.model_path.empty() ? item->params.diffusion_model_path : item->params.model_path);
                this->ShowNotification(title, message);
                break;
            case QM::QueueEvents::ITEM_GENERATION_STARTED:  // GENERATION_START
                if (item->mode == QM::GenerationMode::IMG2IMG ||
                    item->mode == QM::GenerationMode::TXT2IMG) {
                    this->writeLog(wxString::Format(
                        "Diffusion started. Seed: %" PRId64 " Batch: %d %dx%dpx Cfg: %.1f Steps: %d\n",
                        item->params.seed, item->params.batch_count, item->params.width,
                        item->params.height, item->params.cfg_scale,
                        item->params.sample_steps));
                }
                if (item->mode == QM::GenerationMode::UPSCALE) {
                    this->writeLog(wxString::Format(_("Upscale start, factor: %d image: %s\n"), item->upscale_factor, item->initial_image));
                }
                break;
            case QM::QueueEvents::ITEM_FAILED:  // GENERATION_ERROR
                this->writeLog(wxString::Format(_("Generation error: %s\n"), item->status_message));
                this->UpdateJobInfoDetailsFromJobQueueList(item);
                break;
            default:
                break;
        }
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_DONE) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->ModelManager->UpdateInfo(modelinfo);
        this->writeLog(wxString::Format(_("Model civitai image downloaded for model: %s\n"), modelinfo->name));
        // update if the current selected item is the updated item info
        if (this->m_data_model_list->GetSelectedItemsCount() > 0) {
            wxDataViewItem item               = this->m_data_model_list->GetCurrentItem();
            sd_gui_utils::ModelFileInfo* info = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_data_model_list->GetItemData(item));
            if (info->path == modelinfo->path) {
                this->UpdateModelInfoDetailsFromModelList(modelinfo);
            }
        }
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
        sd_gui_utils::ModelFileInfo* modelinfo =
            e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->writeLog(wxString::Format(_("Model civitai info download start: %s\n"), modelinfo->name));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FINISHED) {
        sd_gui_utils::ModelFileInfo* modelinfo  = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        sd_gui_utils::ModelFileInfo newInfo     = this->ModelManager->updateCivitAiInfo(modelinfo);
        sd_gui_utils::ModelFileInfo* newInfoptr = this->ModelManager->getIntoPtr(newInfo.path);

        if (newInfo.state == sd_gui_utils::CivitAiState::OK) {
            this->writeLog(wxString::Format(_("Model civitai info download finished: %s\n"), newInfo.CivitAiInfo.name));
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoImageDownload, this, this->GetEventHandler(), newInfoptr));
        }
        if (newInfo.state == sd_gui_utils::CivitAiState::NOT_FOUND) {
            this->writeLog(wxString::Format(_("Model civitai info not found: %s Hash: %s\n"), newInfo.name, newInfo.sha256.substr(0, 10)));
        }
        if (newInfo.state == sd_gui_utils::CivitAiState::ERR) {
            this->writeLog(wxString::Format(_("Model civitai info unkown parsing error happened: %s Hash: %s\n"), newInfo.name, newInfo.sha256.substr(0, 10)));
        }
        // update anyway
        this->UpdateModelInfoDetailsFromModelList(modelinfo);
        // update table
        auto store     = this->m_data_model_list->GetStore();
        int civitAiCol = this->m_data_model_list->GetColumnCount() - 2;  // civitai col

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto _item      = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto* _qitem    = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(_item_data);
            if (_qitem->path == modelinfo->path) {
                store->SetValueByRow(sd_gui_utils::civitai_state_str[modelinfo->state], i, civitAiCol);
                store->RowValueChanged(i, civitAiCol);
                this->m_data_model_list->Refresh();
                this->m_data_model_list->Update();
                break;
            }
        }
        // update table

        return;
    }

    /// status of hashing from modellist
    if (threadEvent == sd_gui_utils::STANDALONE_HASHING_PROGRESS) {
        // modelinfo
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        auto store                             = this->m_data_model_list->GetStore();
        int progressCol                        = this->m_data_model_list->GetColumnCount() - 1;  // progressbar col
        int hashCol                            = this->m_data_model_list->GetColumnCount() - 3;
        size_t _x                              = modelinfo->hash_progress_size;
        size_t _m                              = modelinfo->hash_fullsize;
        int current_progress                   = 0;
        auto _hr1                              = sd_gui_utils::humanReadableFileSize(static_cast<double>(_x));
        auto _hr2                              = sd_gui_utils::humanReadableFileSize(static_cast<double>(_m));

        if (_m != 0) {
            current_progress = static_cast<int>((_x * 100) / _m);
        }

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto _item      = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto* _qitem =
                reinterpret_cast<sd_gui_utils::ModelFileInfo*>(_item_data);
            if (_qitem->path == modelinfo->path) {
                store->SetValueByRow(current_progress, i, progressCol);
                store->SetValueByRow(modelinfo->sha256.substr(0, 10), i, hashCol);
                store->RowValueChanged(i, hashCol);
                this->m_data_model_list->Refresh();
                this->m_data_model_list->Update();
                break;
            }
        }
    }
    if (threadEvent == sd_gui_utils::STANDALONE_HASHING_DONE) {
        sd_gui_utils::ModelFileInfo* modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        if (modelinfo->civitaiPlainJson.empty() && this->cfg->enable_civitai) {
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
        }
        nlohmann::json j(*modelinfo);
        std::ofstream file(modelinfo->meta_file);
        file << j;
        file.close();
        modelinfo->hash_progress_size = 0;
        return;
    }
    if (threadEvent == sd_gui_utils::HASHING_PROGRESS) {
        std::shared_ptr<QM::QueueItem> myjob = e.GetPayload<std::shared_ptr<QM::QueueItem>>();

        // update column
        auto store = this->m_joblist->GetStore();

        int progressCol = this->m_joblist->GetColumnCount() - 4;

        size_t _x            = myjob->hash_progress_size;
        size_t _m            = myjob->hash_fullsize;
        int current_progress = 0;
        auto _hr1            = sd_gui_utils::humanReadableFileSize(static_cast<double>(_x));
        auto _hr2            = sd_gui_utils::humanReadableFileSize(static_cast<double>(_m));

        if (_m != 0) {
            current_progress = static_cast<int>((_x * 100) / _m);
        }

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto currentItem                     = store->GetItem(i);
            int id                               = store->GetItemData(currentItem);
            std::shared_ptr<QM::QueueItem> qitem = this->qmanager->GetItemPtr(id);
            if (qitem->id == myjob->id) {
                store->SetValueByRow(current_progress, i, progressCol);
                store->RowValueChanged(i, progressCol);
                this->m_joblist->Refresh();
                this->m_joblist->Update();
                break;
            }
        }
        return;
    }
    if (threadEvent == sd_gui_utils::HASHING_DONE) {
        std::shared_ptr<QM::QueueItem> myjob = e.GetPayload<std::shared_ptr<QM::QueueItem>>();

        // update column
        auto store = this->m_joblist->GetStore();

        int progressCol = this->m_joblist->GetColumnCount() - 4;

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto currentItem                     = store->GetItem(i);
            int id                               = store->GetItemData(currentItem);
            std::shared_ptr<QM::QueueItem> qitem = this->qmanager->GetItemPtr(id);
            if (qitem->id == myjob->id) {
                store->SetValueByRow(0, i, progressCol);
                store->RowValueChanged(i, progressCol);
                this->m_joblist->Refresh();
                this->m_joblist->Update();
                break;
            }
        }
        this->ModelManager->getIntoPtr(myjob->params.model_path)->hash_progress_size = 0;
        return;
    }
    if (threadEvent == sd_gui_utils::GENERATION_PROGRESS) {
        std::shared_ptr<QM::QueueItem> myjob = e.GetPayload<std::shared_ptr<QM::QueueItem>>();

        // update column
        auto store             = this->m_joblist->GetStore();
        wxString speed         = wxString::Format(myjob->time > 1.0f ? "%.2fs/it" : "%.2fit/s", myjob->time > 1.0f || myjob->time == 0 ? myjob->time : (1.0f / myjob->time));
        int progressCol        = this->m_joblist->GetColumnCount() - 4;
        int speedCol           = this->m_joblist->GetColumnCount() - 3;
        float current_progress = 100.f * (static_cast<float>(myjob->step) /
                                          static_cast<float>(myjob->steps));

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto currentItem                     = store->GetItem(i);
            int id                               = store->GetItemData(currentItem);
            std::shared_ptr<QM::QueueItem> qitem = this->qmanager->GetItemPtr(id);
            if (qitem->id == myjob->id) {
                store->SetValueByRow(static_cast<int>(current_progress), i, progressCol);
                store->SetValueByRow(speed, i, speedCol);
                store->RowValueChanged(i, progressCol);
                store->RowValueChanged(i, speedCol);
                this->m_joblist->Refresh();
                this->m_joblist->Update();
                break;
            }
        }

        return;
    }
    if (threadEvent == sd_gui_utils::SD_MESSAGE) {
        if (content.length() < 1) {
            return;
        }
        this->writeLog(wxString::Format("%s", content));
        return;
    }
    if (threadEvent == sd_gui_utils::MESSAGE) {
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

        std::string name = std::filesystem::path(payload->local_file).filename().string();

        if (payload->type == CivitAi::ModelTypes::Checkpoint) {
            this->loadModelList();
            //    type = sd_gui_utils::DirTypes::CHECKPOINT;
        }

        if (payload->type == CivitAi::ModelTypes::LORA) {
            //  type = sd_gui_utils::DirTypes::LORA;
            this->loadLoraList();
        }
        if (payload->type == CivitAi::ModelTypes::TextualInversion) {
            // type = sd_gui_utils::DirTypes::EMBEDDING;
            this->loadEmbeddingList();
        }
        auto value = this->m_modellist_filter->GetValue();
        this->refreshModelTable(value);

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

void MainWindowUI::UpdateJobInfoDetailsFromJobQueueList(std::shared_ptr<QM::QueueItem> item) {
    if (item == nullptr) {
        this->m_joblist_item_details->DeleteAllItems();
        this->previewImageList->Destroy();
        this->previewImageList->Create(256, 256);
        for (auto _t : this->jobImagePreviews) {
            _t->Destroy();
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
    auto current_item                         = store->GetItem(row);
    int id                                    = store->GetItemData(current_item);
    std::shared_ptr<QM::QueueItem> store_item = this->qmanager->GetItemPtr(id);
    if (store_item != item) {
        return;
    }

    this->m_joblist_item_details->DeleteAllItems();
    for (auto _t : this->jobImagePreviews) {
        _t->Destroy();
    }
    this->jobImagePreviews.clear();

    wxVector<wxVariant> data;

    data.push_back(wxVariant(_("ID")));
    data.push_back(wxVariant(wxString::Format("%d", item->id)));
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

    data.push_back(wxVariant(_("Udated at")));
    if (item->updated_at == 0) {
        data.push_back(wxVariant("--"));
    } else {
        data.push_back(wxVariant(sd_gui_utils::formatUnixTimestampToDate(item->updated_at)));
    }
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Mode")));
    data.push_back(wxVariant(wxString(modes_str[item->mode])));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Model")));
    data.push_back(wxVariant(wxString(item->model)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    if (item->mode == QM::GenerationMode::CONVERT) {
        data.push_back(wxVariant(_("Model")));
        data.push_back(wxVariant(wxString(item->params.output_path)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    data.push_back(wxVariant(_("Threads")));
    data.push_back(wxVariant(wxString::Format("%d", item->params.n_threads)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    if (item->mode == QM::GenerationMode::UPSCALE) {
        data.push_back(wxVariant(_("Factor")));
        data.push_back(
            wxVariant(wxString::Format("%" PRId32, item->upscale_factor)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    if (item->mode == QM::GenerationMode::IMG2IMG ||
        item->mode == QM::GenerationMode::TXT2IMG ||
        item->mode == QM::GenerationMode::CONVERT) {
        data.push_back(wxVariant(_("Type")));
        data.push_back(wxVariant(wxString::Format("%s", sd_gui_utils::sd_type_gui_names[item->params.wtype])));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
        if (item->mode != QM::GenerationMode::CONVERT) {
            data.push_back(wxVariant(_("Scheduler")));
            data.push_back(wxVariant(wxString::Format("%s", sd_gui_utils::sd_scheduler_gui_names[item->params.schedule])));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
    }

    if (item->mode == QM::GenerationMode::IMG2IMG) {
        data.push_back(wxVariant(_("Init image")));
        data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->initial_image.data(), item->initial_image.size())));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Strength")));
        data.push_back(wxVariant(wxString::Format("%.2f", item->params.strength)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    if (item->mode == QM::GenerationMode::TXT2IMG ||
        item->mode == QM::GenerationMode::IMG2IMG) {
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

        data.push_back(wxVariant(_("Sampler")));
        data.push_back(wxVariant(wxString::Format("%s", sd_gui_utils::sample_method_str[item->params.sample_method])));
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
            data.push_back(wxVariant(_("VAE")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.vae_path)));
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
            data.push_back(wxVariant(_("TAESD")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.taesd_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
    }
    if (item->mode == QM::GenerationMode::TXT2IMG) {
        data.push_back(wxVariant(_("Width")));
        data.push_back(wxVariant(wxString::Format("%dpx", item->params.width)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Height")));
        data.push_back(wxVariant(wxString::Format("%dpx", item->params.height)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Batch count")));
        data.push_back(wxVariant(wxString::Format("%d", item->params.batch_count)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        if (!item->params.controlnet_path.empty()) {
            data.push_back(wxVariant(_("CN model")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.controlnet_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("CN img")));
            data.push_back(wxVariant(wxString::FromUTF8Unchecked(item->params.control_image_path)));
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
    int index = 0;
    for (auto img : item->images) {
        if (std::filesystem::exists(img.pathname)) {
            auto resized = sd_gui_utils::cropResizeImage(wxString::FromUTF8Unchecked(img.pathname), 256, 256, wxColour(0, 0, 0, wxALPHA_TRANSPARENT), wxString::FromUTF8Unchecked(this->cfg->thumbs_path));
            img.id       = index;

            wxStaticBitmap* bitmap = new wxStaticBitmap(this->m_scrolledWindow41, wxID_ANY, resized, wxDefaultPosition, wxDefaultSize, 0);
            bitmap->Hide();
            this->bSizer8911->Add(bitmap, 0, wxALL, 2);
            bitmap->Show();
            this->jobImagePreviews.emplace_back(bitmap);
            bitmap->Bind(wxEVT_LEFT_DCLICK, [img](wxMouseEvent& event) {
                wxLaunchDefaultApplication(wxString::FromUTF8Unchecked(img.pathname));
            });
            // rightclick
            bitmap->Bind(wxEVT_RIGHT_UP, [img, bitmap, item, this](wxMouseEvent& event) {
                wxMenu* menu = new wxMenu();
                // menu->Append(99,tooltip);
                // menu->Enable(99,false);
                menu->Append(6, _("Open image"));
                menu->Append(7, _("Open parent folder"));
                menu->AppendSeparator();
                menu->Append(0, wxString::Format(_("Copy seed %" PRId64), item->params.seed + img.id));
                menu->Append(1, _("Copy prompts to text2img"));
                menu->Append(2, _("Copy prompts to img2img"));
                menu->Append(3, _("Send the image to img2img"));
                menu->Append(5, _("Upscale"));

                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, img, item](wxCommandEvent& evt) {
                    auto id = evt.GetId();
                    switch (id) {
                        case 0: {
                            this->m_seed->SetValue(item->params.seed);
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
                            this->onimg2ImgImageOpen(wxString::FromUTF8Unchecked(img.pathname));
                        } break;
                        case 5: {
                            this->onUpscaleImageOpen(wxString::FromUTF8Unchecked(img.pathname));
                            this->m_notebook1302->SetSelection(3);
                        } break;
                        case 6: {
                            wxLaunchDefaultApplication(wxString::FromUTF8Unchecked(img.pathname));
                        } break;
                        case 7: {
                            wxLaunchDefaultApplication(wxString::FromUTF8Unchecked(std::filesystem::path(img.pathname).parent_path().string()));
                        } break;
                        default: {
                            return;
                        } break;
                    }
                });
                bitmap->PopupMenu(menu);
                delete menu;
            });
            // rightclick
            index++;
        }
    }
    this->m_scrolledWindow41->FitInside();
}

std::shared_ptr<QM::QueueItem> MainWindowUI::handleSdImage(const std::string& tmpImagePath, std::shared_ptr<QM::QueueItem> itemPtr, wxEvtHandler* eventHandler) {
    wxImage* img = new wxImage(0, 0);
    img->SetLoadFlags(img->GetLoadFlags() & ~wxImage::Load_Verbose);

    if (!img->LoadFile(wxString::FromUTF8Unchecked(tmpImagePath))) {
        itemPtr->status_message = std::string(_("Invalid image from diffusion..."));
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, itemPtr);
        delete img;
        return itemPtr;
    }
    wxString filename     = wxString::FromUTF8Unchecked(this->cfg->output);
    std::string extension = ".jpg";
    auto imgHandler       = wxBITMAP_TYPE_JPEG;

    if (this->cfg->image_type == sd_gui_utils::imageTypes::PNG) {
        extension  = ".png";
        imgHandler = wxBITMAP_TYPE_PNG;
    }
    if (this->cfg->image_type == sd_gui_utils::imageTypes::JPG) {
        extension  = ".jpg";
        imgHandler = wxBITMAP_TYPE_JPEG;
        img->SetOption("quality", this->cfg->image_quality);
    }
    wxString filename_without_extension;
    filename = filename + wxString(wxFileName::GetPathSeparator()).utf8_string();
    filename = filename + modes_str[itemPtr->mode];
    filename = filename + "_";
    filename = filename + std::to_string(itemPtr->id);
    filename = filename + "_";
    filename = filename + std::to_string(itemPtr->params.seed);
    filename = filename + "_";
    filename = filename + std::to_string(img->GetWidth()) + "x" + std::to_string(img->GetHeight());
    filename = filename + "_";

    filename_without_extension = filename;
    filename                   = filename + extension;

    int _c = 0;
    while (std::filesystem::exists(filename.utf8_string())) {
        filename = filename_without_extension + "_" + std::to_string(_c) + extension;
        _c++;
    }

    if (!img->SaveFile(filename, imgHandler)) {
        itemPtr->status_message = wxString::Format(_("Failed to save image into %s"), filename);
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, itemPtr);
        delete img;
        std::cerr << __FILE__ << " " << __LINE__ << " " << itemPtr->status_message.c_str() << std::endl;
        std::filesystem::remove(tmpImagePath);
        return itemPtr;
    } else {
        std::filesystem::remove(tmpImagePath);

        itemPtr->images.emplace_back(QM::QueueItemImage(filename, QM::QueueItemImageType::GENERATED));

        if (itemPtr->params.control_image_path.length() > 0 && this->cfg->save_all_image) {
            wxString ctrlFilename = this->cfg->output;
            ctrlFilename          = filename_without_extension + "_ctrlimg_" + extension;
            wxImage _ctrlimg(itemPtr->params.control_image_path);
            _ctrlimg.SaveFile(ctrlFilename);
            itemPtr->images.emplace_back(QM::QueueItemImage({ctrlFilename, QM::QueueItemImageType::CONTROLNET}));
        }
        // add generation parameters into the image meta
        if (this->cfg->image_type == sd_gui_utils::imageTypes::JPG) {
            std::string comment = this->paramsToImageComment(*itemPtr, this->ModelManager->getInfo(itemPtr->params.model_path));

            try {
                auto image = Exiv2::ImageFactory::open(filename.utf8_string());
                image->readMetadata();
                Exiv2::ExifData& exifData          = image->exifData();
                exifData["Exif.Photo.UserComment"] = comment;
                exifData["Exif.Image.XPComment"]   = comment;
                exifData["Exif.Image.Software"]    = EXIF_SOFTWARE;
                exifData["Exif.Image.ImageWidth"]  = img->GetWidth();
                exifData["Exif.Image.ImageLength"] = img->GetHeight();

                if (itemPtr->finished_at > 0) {
                    time_t finishedAt = itemPtr->finished_at;
                    std::tm* timeinfo = std::localtime(&finishedAt);
                    char dtimeBuffer[20];
                    std::strftime(dtimeBuffer, sizeof(dtimeBuffer), "%Y:%m:%d %H:%M:%S", timeinfo);
                    exifData["Exif.Image.DateTime"] = dtimeBuffer;
                }

                image->setExifData(exifData);
                image->writeMetadata();
            } catch (Exiv2::Error& e) {
                std::cerr << "Err: " << e.what() << std::endl;
            }
        }
    }
    return itemPtr;
}

void MainWindowUI::ModelHashingCallback(size_t readed_size, std::string sha256, void* custom_pointer) {
    sd_gui_utils::VoidHolder* holder       = static_cast<sd_gui_utils::VoidHolder*>(custom_pointer);
    wxEvtHandler* eventHandler             = (wxEvtHandler*)holder->p1;
    std::shared_ptr<QM::QueueItem> myItem  = std::shared_ptr<QM::QueueItem>(static_cast<QM::QueueItem*>(holder->p2));
    sd_gui_utils::ModelFileInfo* modelinfo = (sd_gui_utils::ModelFileInfo*)holder->p3;
    modelinfo->hash_progress_size          = readed_size;
    modelinfo->hash_fullsize               = myItem->hash_fullsize;
    myItem->hash_progress_size             = readed_size;

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::HASHING_PROGRESS, myItem);

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
}

void MainWindowUI::ShowNotification(const wxString& title, const wxString& message) {
    if (this->cfg->show_notifications) {
        wxNotificationMessage notification(title, message, this);
        notification.SetTitle(title);
#if defined(_WIN64) || defined(_WIN32) || defined(WIN32)
        notification.UseTaskBarIcon(this->TaskBar);
#endif

        notification.Show(this->cfg->notification_timeout);
    }

    this->TaskBar->SetIcon(this->TaskBarIcon, wxString::Format("%s - %s", this->GetTitle(), title));
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
        this->m_width->SetValue(img.GetWidth());
        this->m_height->SetValue(img.GetHeight());
        // can not change the outpt images resolution
        this->m_width->Disable();
        this->m_height->Disable();
        this->m_button7->Disable();  // swap resolution
    }
}

void MainWindowUI::LoadPresets() {
    this->LoadFileList(sd_gui_utils::DirTypes::PRESETS);
}

void MainWindowUI::ChangeModelByName(wxString ModelName) {
    sd_gui_utils::ModelFileInfo minfo = this->ModelManager->findInfoByName(ModelName.utf8_string());

    for (unsigned int _z = 0; _z < this->m_model->GetCount(); _z++) {
        sd_gui_utils::ModelFileInfo* m = reinterpret_cast<sd_gui_utils::ModelFileInfo*>(this->m_model->GetClientData(_z));

        if (m == nullptr) {
            continue;
        }
        if (m->path == minfo.path) {
            this->m_model->SetSelection(_z);
            this->m_generate2->Enable();
            if (this->m_img2imgOpen->GetPath().length() > 0) {
                this->m_generate1->Enable();
            }
            return;
        }
    }
}

void MainWindowUI::ChangeModelByInfo(const sd_gui_utils::ModelFileInfo info) {
    for (unsigned int _z = 0; _z < this->m_model->GetCount(); _z++) {
        sd_gui_utils::ModelFileInfo* m =
            reinterpret_cast<sd_gui_utils::ModelFileInfo*>(
                this->m_model->GetClientData(_z));
        // maybe this is the none
        if (m == nullptr) {
            continue;
        }
        if (info == m) {
            this->m_model->SetSelection(_z);
            return;
        }
    }
}

void MainWindowUI::loadModelList() {
    std::string oldSelection = this->m_model->GetStringSelection().utf8_string();

    this->LoadFileList(sd_gui_utils::DirTypes::CHECKPOINT);

    if (this->ModelFilesIndex.find(oldSelection) != this->ModelFilesIndex.end()) {
        this->m_model->SetSelection(this->ModelFilesIndex[oldSelection]);
    }
}

void MainWindowUI::initConfig() {
    if (this->mapp->config == nullptr) {
        return;
    }

    wxString datapath   = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "sd_ui_data" + wxFileName::GetPathSeparator();
    wxString imagespath = wxStandardPaths::Get().GetDocumentsDir() + wxFileName::GetPathSeparator() + "sd_ui_output" + wxFileName::GetPathSeparator();

    wxString model_path = datapath;
    model_path.append("checkpoints");

    wxString vae_path = datapath;
    vae_path.append("vae");

    wxString lora_path = datapath;
    lora_path.append("lora");

    wxString embedding_path = datapath;
    embedding_path.append("embedding");

    wxString taesd_path = datapath;
    taesd_path.append("taesd");

    wxString presets_path = datapath;
    presets_path.append("presets");

    wxString jobs_path = datapath;
    jobs_path.append("queue_jobs");

    wxString thumbs_path = datapath;
    thumbs_path.append("thumbs");

    wxString tmp_path = datapath;
    tmp_path.append("tmp");

    wxString controlnet_path = datapath;
    controlnet_path.append("controlnet");

    wxString esrgan_path = datapath;
    esrgan_path.append("esrgan");

    this->cfg->datapath = datapath;

    this->cfg->lora                 = this->mapp->config->Read("/paths/lora", lora_path).utf8_string();
    this->cfg->model                = this->mapp->config->Read("/paths/model", model_path).utf8_string();
    this->cfg->vae                  = this->mapp->config->Read("/paths/vae", vae_path).utf8_string();
    this->cfg->embedding            = this->mapp->config->Read("/paths/embedding", embedding_path).utf8_string();
    this->cfg->taesd                = this->mapp->config->Read("/paths/taesd", taesd_path).utf8_string();
    this->cfg->esrgan               = this->mapp->config->Read("/paths/esrgan", esrgan_path).utf8_string();
    this->cfg->controlnet           = this->mapp->config->Read("/paths/controlnet", controlnet_path).utf8_string();
    this->cfg->presets              = this->mapp->config->Read("/paths/presets", presets_path).utf8_string();
    this->cfg->jobs                 = this->mapp->config->Read("/paths/jobs", jobs_path).utf8_string();
    this->cfg->thumbs_path          = thumbs_path.utf8_string();
    this->cfg->tmppath              = tmp_path.utf8_string();
    this->cfg->output               = this->mapp->config->Read("/paths/output", imagespath).utf8_string();
    this->cfg->keep_model_in_memory = this->mapp->config->Read("/keep_model_in_memory", this->cfg->keep_model_in_memory);
    this->cfg->save_all_image       = this->mapp->config->Read("/save_all_image", this->cfg->save_all_image);
    this->cfg->n_threads            = this->mapp->config->Read("/n_threads", cores());
    this->cfg->show_notifications   = this->mapp->config->ReadBool("/show_notification", this->cfg->show_notifications);
    this->cfg->notification_timeout = this->mapp->config->Read("/notification_timeout", this->cfg->notification_timeout);
    this->cfg->image_quality        = this->mapp->config->Read("/image_quality", this->cfg->image_quality);
    this->cfg->enable_civitai       = this->mapp->config->ReadBool("/enable_civitai", this->cfg->enable_civitai);
    this->cfg->language             = this->mapp->config->Read("/language", wxUILocale::GetLanguageInfo(wxUILocale::GetSystemLocale())->CanonicalName.utf8_string());

    int idx               = 0;
    auto saved_image_type = this->mapp->config->Read("/image_type", "JPG");

    for (auto type : sd_gui_utils::image_types_str) {
        if (saved_image_type == type) {
            this->cfg->image_type = (sd_gui_utils::imageTypes)idx;
            break;
        }
        idx++;
    }
    // populate data from sd_params as default...

    if (this->firstCfgInit) {
        // check if directories exists...
        if (!std::filesystem::exists(model_path.utf8_string())) {
            std::filesystem::create_directories(model_path.utf8_string());
        }
        if (!std::filesystem::exists(lora_path.utf8_string())) {
            std::filesystem::create_directories(lora_path.utf8_string());
        }
        if (!std::filesystem::exists(vae_path.utf8_string())) {
            std::filesystem::create_directories(vae_path.utf8_string());
        }
        if (!std::filesystem::exists(embedding_path.utf8_string())) {
            std::filesystem::create_directories(embedding_path.utf8_string());
        }
        if (!std::filesystem::exists(tmp_path.utf8_string())) {
            std::filesystem::create_directories(tmp_path.utf8_string());
        }
        if (!std::filesystem::exists(taesd_path.utf8_string())) {
            std::filesystem::create_directories(taesd_path.utf8_string());
        }
        if (!std::filesystem::exists(esrgan_path.utf8_string())) {
            std::filesystem::create_directories(esrgan_path.utf8_string());
        }
        if (!std::filesystem::exists(presets_path.utf8_string())) {
            std::filesystem::create_directories(presets_path.utf8_string());
        }
        if (!std::filesystem::exists(jobs_path.utf8_string())) {
            std::filesystem::create_directories(jobs_path.utf8_string());
        }
        if (!std::filesystem::exists(controlnet_path.utf8_string())) {
            std::filesystem::create_directories(controlnet_path.utf8_string());
        }
        if (!std::filesystem::exists(imagespath.utf8_string())) {
            std::filesystem::create_directories(imagespath.utf8_string());
        }
        if (!std::filesystem::exists(thumbs_path.utf8_string())) {
            std::filesystem::create_directories(thumbs_path.utf8_string());
        }
        this->loadSamplerList();
        this->loadTypeList();
        this->loadSchedulerList();
    }

    this->firstCfgInit = false;
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
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_START, modelinfo);

    // get apikey from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.Load(SD_GUI_HOMEPAGE, username, password)) {
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
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FINISHED, modelinfo);
        return;
    } catch (const std::exception& e) {
        std::cerr << __FILE__ << ":" << __LINE__ << " " << e.what() << std::endl;
        wxMessageDialog dialog(this, e.what());
        modelinfo->civitaiPlainJson = response;
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FAILED, modelinfo);
        return;
    }
    modelinfo->civitaiPlainJson = response;
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FAILED, modelinfo);
}

void MainWindowUI::threadedModelInfoImageDownload(
    wxEvtHandler* eventHandler,
    sd_gui_utils::ModelFileInfo* modelinfo) {
    if (modelinfo->civitaiPlainJson.empty()) {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, std::string("No model info found."));
        return;
    }

    if (modelinfo->CivitAiInfo.images.empty()) {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, "No images found.");
        return;
    }

    // Remove old files
    for (const std::string& old_img : modelinfo->preview_images) {
        if (std::filesystem::exists(old_img)) {
            std::filesystem::remove(old_img);
        }
    }
    modelinfo->preview_images.clear();
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGES_START, modelinfo);

    // Get API key from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;

    wxSecretStore store = wxSecretStore::GetDefault();
    if (store.Load(SD_GUI_HOMEPAGE, username, password)) {
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
    int index = 0;
    for (CivitAi::image& img : modelinfo->CivitAiInfo.images) {
        std::ostringstream response;
        try {
            sd_gui_utils::SimpleCurl curl;

            // Set callback to write data to file
            std::string target_path = std::filesystem::path(this->cfg->datapath + "/" + modelinfo->sha256 + "_" + std::to_string(index) + ".tmp").generic_string();

            curl.getFile(img.url, headers, target_path);

            // Process downloaded image
            wxImage _tmpImg;
            _tmpImg.SetLoadFlags(_tmpImg.GetLoadFlags() & ~wxImage::Load_Verbose);

            if (_tmpImg.LoadFile(target_path)) {
                std::string new_path;
                if (_tmpImg.GetType() == wxBITMAP_TYPE_JPEG) {
                    new_path = std::filesystem::path(target_path).replace_extension(".jpg").generic_string();
                } else if (_tmpImg.GetType() == wxBITMAP_TYPE_PNG) {
                    new_path = std::filesystem::path(target_path).replace_extension(".png").generic_string();
                }

                if (!new_path.empty()) {
                    std::filesystem::rename(target_path, new_path);
                    modelinfo->preview_images.emplace_back(new_path);
                    img.local_path = new_path;

                    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGES_PROGRESS, modelinfo, img.url);
                    index++;
                }
            }
            _tmpImg.Destroy();
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, img.url);
        }
    }

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGES_DONE, modelinfo);
}

void MainWindowUI::PrepareModelConvert(sd_gui_utils::ModelFileInfo* modelInfo) {
    // sd convert params:
    // bool success = convert(params.model_path.c_str(), params.vae_path.c_str(), params.output_path.c_str(), params.wtype);
    std::filesystem::path modelIn(modelInfo->path);
    std::string name     = modelIn.filename().replace_extension("").generic_string();
    std::string newType  = this->m_type->GetStringSelection().utf8_string();
    std::string usingVae = "";
    name                 = name + "_" + newType + ".gguf";

    std::string modelOutName = modelIn.replace_filename(name).generic_string();
    std::cout << "Old name: " << modelInfo->path << " converted name: " << modelOutName << std::endl;

    std::shared_ptr<QM::QueueItem> item = std::make_shared<QM::QueueItem>();
    item->model                         = modelInfo->name;
    item->mode                          = QM::GenerationMode::CONVERT;
    item->params.mode                   = SDMode::CONVERT;
    item->params.n_threads              = this->cfg->n_threads;
    item->params.output_path            = modelOutName;
    item->params.model_path             = modelInfo->path;

    auto selectedwType = this->m_type->GetStringSelection();
    for (auto types : sd_gui_utils::sd_type_gui_names) {
        if (types.second == selectedwType) {
            item->params.wtype = (sd_type_t)types.first;
        }
    }

    auto selectedScheduler = this->m_scheduler->GetStringSelection();
    for (auto schedulers : sd_gui_utils::sd_scheduler_gui_names) {
        if (schedulers.second == selectedScheduler) {
            item->params.schedule = (schedule_t)schedulers.first;
            break;
        }
    }

    if (this->m_vae->GetCurrentSelection() > 0) {
        usingVae              = this->m_vae->GetStringSelection().utf8_string();
        item->params.vae_path = this->VaeFiles.at(usingVae);
    }

    wxString question = wxString::Format(_("Do you want to convert model %s with quantation %s to gguf format?"), modelInfo->name, newType);

    if (usingVae != "") {
        question = wxString::Format(_("Do you want to convert model %s with quantation %s and vae %s to gguf format?"), modelInfo->name, newType, usingVae);
    }

    wxMessageDialog dialog(this, question, wxString::Format(_("Convert model %s?"), modelInfo->name), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
    if (dialog.ShowModal() == wxID_YES) {
        if (std::filesystem::exists(modelOutName)) {
            wxString overwriteQuestion = wxString::Format(_("The file %s already exists. Do you want to overwrite it?"), modelOutName);
            wxMessageDialog overwriteDialog(this, overwriteQuestion, _("Overwrite File?"), wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
            if (overwriteDialog.ShowModal() != wxID_YES) {
                return;
            }
        }
        this->qmanager->AddItem(item, false);
    }
}

void MainWindowUI::OnHtmlLinkClicked(wxHtmlLinkEvent& event) {
    wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
}
void MainWindowUI::onWhatIsThis(wxCommandEvent& event) {
    wxLaunchDefaultBrowser("https://github.com/fszontagh/sd.cpp.gui.wx/wiki/LCM-&-SD3.5-&-FLUX-howtos");
}

bool MainWindowUI::ProcessEventHandler(std::string message) {
    try {
        nlohmann::json msg = nlohmann::json::parse(message);
        QM::QueueItem item = msg.get<QM::QueueItem>();

        auto itemPtr = this->qmanager->GetItemPtr(item.id);

        if (itemPtr == nullptr) {
            std::cerr << "[GUI] Could not find item " << item.id << std::endl;
            return false;
        }
        if (itemPtr->event == QM::QueueEvents::ITEM_FAILED) {
            std::cerr << "[GUI] Item " << item.id << " QM::QueueEvents::ITEM_FAILED, skipping" << std::endl;
            return true;
        }
        if (itemPtr->status == QM::QueueStatus::FAILED) {
            std::cerr << "[GUI] Item " << item.id << " QM::QueueStatus::FAILED, skipping" << std::endl;
            return true;
        }
        // mutex lock
        std::lock_guard<std::mutex> lock(this->mutex);

        if (itemPtr->update_index != item.update_index || itemPtr->event != item.event) {
            if (BUILD_TYPE == "Debug") {
                std::cout << "[GUI] Item " << item.id << " was updated, event: " << QM::QueueEvents_str.at(item.event) << std::endl;
            }
            this->extProcessLastEvent = item.event;

            *itemPtr = item;

            if (!itemPtr->rawImages.empty()) {
                for (unsigned int i = 0; i < itemPtr->rawImages.size(); i++) {
                    this->handleSdImage(itemPtr->rawImages[i], itemPtr, this->GetEventHandler());
                }
                itemPtr->rawImages.clear();
            }
            if (item.event == QM::QueueEvents::ITEM_FAILED) {
                if (this->extprocessLastError.empty() == true && this->extprocessLastError.empty() == false) {
                    itemPtr->status_message   = this->extprocessLastError;
                    this->extprocessLastError = "";
                }
            }

            this->qmanager->SendEventToMainWindow(item.event, itemPtr);
            return true;
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return false;
}

void MainWindowUI::ProcessStdOutEvent(const char* bytes, size_t n) {
    if (bytes == nullptr) {
        std::cerr << "[GUI] ProcessStdOutEvent: bytes is null" << std::endl;
        return;
    }

    std::string msg = std::string(bytes, n);
    if (msg.empty()) {
        std::cerr << "[GUI] ProcessStdOutEvent: msg is empty" << std::endl;
        return;
    }

    if (msg.find("PING") != std::string::npos) {
        return;
    }

    std::istringstream iss(msg);
    std::string line;
    try {
        while (std::getline(iss, line, '\n')) {
            this->writeLog(line + '\n', false);
        }
    } catch (const std::exception& e) {
        std::cerr << "[GUI] " << __FILE__ << ":" << __LINE__ << " ProcessStdOutEvent: exception: " << e.what() << std::endl;
    }
}

void MainWindowUI::ProcessStdErrEvent(const char* bytes, size_t n) {
    std::string msg = std::string(bytes, n);

    std::istringstream iss(msg);
    std::string line;
    while (std::getline(iss, line, '\n')) {
        if (!line.empty()) {
            this->extprocessLastError = line;
            this->writeLog(line + '\n', false);
        }
    }
}

void MainWindowUI::ProcessOutputThread() {
    while (this->extProcessNeedToRun == true) {
        if (subprocess_alive(this->subprocess) != 0) {
            static char stddata[1024]    = {0};
            static char stderrdata[1024] = {0};

            unsigned int size             = sizeof(stderrdata);
            unsigned int stdout_read_size = 0;
            unsigned int stderr_read_size = 0;

            stdout_read_size = subprocess_read_stdout(this->subprocess, stddata, size);
            stderr_read_size = subprocess_read_stderr(this->subprocess, stderrdata, size);

            if (stdout_read_size > 0 && std::string(stddata).find("(null)") == std::string::npos) {
                this->ProcessStdOutEvent(stddata, stdout_read_size);
            }
            if (stderr_read_size > 0 && std::string(stderrdata).find("(null)") == std::string::npos) {
                this->ProcessStdErrEvent(stderrdata, stderr_read_size);
            }
        }
        if (this->qmanager->GetCurrentItem() != nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME / 2));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
        }
    }
}

void MainWindowUI::ProcessCheckThread() {
    while (this->extProcessNeedToRun == true) {
        if (subprocess_alive(this->subprocess) != 0) {
            char* buffer = new char[SHARED_MEMORY_SIZE];
            this->sharedMemory->read(buffer, SHARED_MEMORY_SIZE);

            if (std::strlen(buffer) > 0) {
                bool state = this->ProcessEventHandler(std::string(buffer, std::strlen(buffer)));
                if (state == true) {
                    this->sharedMemory->clear();
                }
            }
            delete buffer;

            if (this->extProcessNeedToRun == false) {
                std::string exitMsg = "exit";
                this->sharedMemory->write(exitMsg.c_str(), exitMsg.size());
                {
                    std::lock_guard<std::mutex> lock(this->taskBarMutex);
                    this->m_statusBar166->SetStatusText(_("Stopping..."), 1);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME));
                return;
            }
            float sleepTime = EPROCESS_SLEEP_TIME;
            if (this->qmanager->GetCurrentItem() != nullptr && this->qmanager->GetCurrentItem()->status == QM::QueueStatus::RUNNING) {
                if (this->qmanager->GetCurrentItem()->stats.time_min > 0) {
                    sleepTime = this->qmanager->GetCurrentItem()->stats.time_min;
                } else {
                    sleepTime = static_cast<float>(EPROCESS_SLEEP_TIME) / 4.f;
                }
            }
            {  // it will be updated again, when a  job is finished
                if (this->jobsCountSinceSegfault == 0) {
                    std::lock_guard<std::mutex> lock(this->taskBarMutex);
                    this->m_statusBar166->SetStatusText(_("Process is ready"), 1);
                    if (this->qmanager->GetCurrentItem() == nullptr) {
                        this->m_stop_background_process->Enable();
                    } else {
                        this->m_stop_background_process->Disable();
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(sleepTime));

            continue;
        }
        // clear the last job to avoid restarting the failed job
        this->sharedMemory->clear();
        this->qmanager->resetRunning(_("External process stopped"));
        {
            std::lock_guard<std::mutex> lock(this->taskBarMutex);
            this->m_statusBar166->SetStatusText(_("Process is stopped"), 1);
        }
        delete this->subprocess;
        // restart
        const char* command_line[] = {this->extprocessCommand.c_str(), this->extProcessParam.c_str(), nullptr};
        this->subprocess           = new subprocess_s();

        int result = subprocess_create(command_line, subprocess_option_no_window | subprocess_option_combined_stdout_stderr | subprocess_option_enable_async | subprocess_option_search_user_path | subprocess_option_inherit_environment, this->subprocess);
        if (0 != result) {
            {
                std::lock_guard<std::mutex> lock(this->taskBarMutex);
                this->m_statusBar166->SetStatusText(_("Failed to restart the background process..."), 1);
                this->m_stop_background_process->Disable();
            }
        }
        this->jobsCountSinceSegfault  = 0;
        this->stepsCountSinceSegfault = 0;
        if (BUILD_TYPE == "Debug") {
            std::cout << "[GUI] restart sleep time: " << (EPROCESS_SLEEP_TIME * 10) << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(EPROCESS_SLEEP_TIME * 10));
    }
}
