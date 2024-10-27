#include "MainWindowUI.h"
#include <cstddef>
#include "utils.hpp"

MainWindowUI::MainWindowUI(wxWindow* parent)
    : mainUI(parent) {
    this->ini_path = wxStandardPaths::Get().GetUserConfigDir() +
                     wxFileName::GetPathSeparator() + "sd.ui.config.ini";
    this->sd_params                  = new sd_gui_utils::SDParams;
    this->currentInitialImage        = new wxImage();
    this->currentInitialImagePreview = new wxImage();
    this->currentUpscalerSourceImage = new wxImage();

    this->currentControlnetImage        = new wxImage();
    this->currentControlnetImagePreview = new wxImage();
    this->ControlnetOrigPreviewBitmap =
        this->m_controlnetImagePreview->GetBitmap();
    this->AppOrigPlaceHolderBitmap = this->m_img2img_preview->GetBitmap();

    this->m_prompt->GetStyle(0, defaultTextCtrlStyle);

    this->m_joblist->AppendTextColumn(_("Id"));
    this->m_joblist->AppendTextColumn(_("Created at"));
    this->m_joblist->AppendTextColumn(_("Type"));
    this->m_joblist->AppendTextColumn(_("Model"));
    this->m_joblist->AppendTextColumn(_("Sampler"));
    this->m_joblist->AppendTextColumn(_("Seed"));
    this->m_joblist->AppendProgressColumn(_("Progress"));  // progressbar
    this->m_joblist->AppendTextColumn(_("Speed"));         // speed
    this->m_joblist->AppendTextColumn(_("Status"));        // status

    this->m_joblist->GetColumn(0)->SetHidden(true);
    this->m_joblist->GetColumn(1)->SetSortable(true);
    this->m_joblist->GetColumn(1)->SetSortOrder(false);

    this->SetTitle(wxString::Format("%s - %s", PROJECT_NAME, SD_GUI_VERSION));
    this->TaskBar = new wxTaskBarIcon();

    this->TaskBarIcon = app_png_to_wx_bitmap();
    this->TaskBar->SetIcon(this->TaskBarIcon, this->GetTitle());

    wxIcon icon;
    icon.CopyFromBitmap(this->TaskBarIcon);
    this->SetIcon(icon);

    this->cfg = new sd_gui_utils::config;

    this->initConfig();

    wxPersistentRegisterAndRestore(this, this->GetName());

    this->qmanager     = new QM::QueueManager(this->GetEventHandler(), this->cfg->jobs);
    this->ModelManager = new ModelInfo::Manager(this->cfg->datapath);

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
    /// save and restore things
    // wxPersistenceManager::Get().RegisterAndRestore(this->m_notebook1302);
    // wxPersistenceManager::Get().RegisterAndRestore(this->m_joblist);
    // wxPersistenceManager::Get().RegisterAndRestore(this->m_data_model_list);

    Bind(wxEVT_THREAD, &MainWindowUI::OnThreadMessage, this);

    this->m_upscalerHelp->SetPage(wxString("Officially from sd.cpp, the following upscaler model is supported: <br/><a href=\"https://civitai.com/models/147821/realesrganx4plus-anime-6b\">RealESRGAN_x4Plus Anime 6B</a><br/>This is working sometimes too: <a href=\"https://civitai.com/models/147817/realesrganx4plus\">RealESRGAN_x4Plus</a>"));
}

void MainWindowUI::onSettings(wxCommandEvent& event) {
    if (this->qmanager->IsRunning()) {
        wxMessageDialog(
            this, _("Please wait to finish the currently running running jobs!"))
            .ShowModal();
        return;
    }
    auto bitmap = app_png_to_wx_bitmap();
    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    this->settingsWindow = new MainWindowSettings(this);
    this->settingsWindow->SetIcon(icon);
    this->Freeze();
    this->settingsWindow->Bind(wxEVT_CLOSE_WINDOW, &MainWindowUI::OnCloseSettings,
                               this);
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
    SdGetSystemInfoFunction sd_get_system_info = (SdGetSystemInfoFunction)this->sd_dll->GetSymbol("sd_get_system_info");
    auto sysinfo                               = sd_get_system_info();
    MainWindowAboutDialog* dialog              = new MainWindowAboutDialog(this);

    wxString about = wxString("<p><strong>Disclaimer</strong></p><p>Warning: This application may terminate unexpectedly via an abort signal during runtime. Use of this application is at your own risk. The developer assumes no responsibility or liability for any potential data loss, damage, or other issues arising from its usage. By using this application, you acknowledge and accept these terms.</p>");

    about.Append(wxString::Format(
        "<h2>%s</h2><p>Version: %s </p><p>Git version: %s</p><p>Website: <a target='_blank' href='%s'>%s</a></p><hr/>",
        PROJECT_NAME, SD_GUI_VERSION, GIT_HASH, SD_GUI_HOMEPAGE, PROJECT_NAME));

    about.Append(wxString::Format("<p>Loaded backend: %s</p>", usingBackend.c_str()));
    about.append(wxString::Format(
        "<p>Configuration folder: %s</p><p>Config file: %s</p><p>Data folder: %s</p>",
        wxStandardPaths::Get().GetUserConfigDir(), this->ini_path, this->cfg->datapath));

    about.Append(wxString::Format("<p>Model folder: %s</p>", this->cfg->model));
    about.Append(wxString::Format("<p>Embedding folder: %s</p>", this->cfg->embedding));
    about.Append(wxString::Format("<p>Lora folder: %s</p>", this->cfg->lora));
    about.Append(wxString::Format("<p>Vae folder: %s</p>", this->cfg->vae));
    about.Append(wxString::Format("<p>Controlnet folder: %s</p>", this->cfg->controlnet));
    about.Append(wxString::Format("<p>Esrgan folder: %s</p>", this->cfg->esrgan));
    about.Append(wxString::Format("<p>Taesd folder: %s</p>", this->cfg->taesd));
    about.Append(wxString::Format("<p>Image's output folder: %s</p>", this->cfg->output));
    about.Append(wxString::Format("<p>Jobs folder: %s</p>", this->cfg->jobs));
    about.Append(wxString::Format("<p>Presets folder: %s</p>", this->cfg->presets));

    about.append(wxString("<pre>"));
    about.append(wxString(sysinfo));
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
        this->civitwindow->SetTitle(wxString::Format(
            "%s | %s", this->GetTitle(), this->civitwindow->GetTitle()));
        this->civitwindow->Bind(wxEVT_THREAD, &MainWindowUI::OnCivitAiThreadMessage,
                                this);
        this->civitwindow->Bind(wxEVT_CLOSE_WINDOW,
                                &MainWindowUI::OnCloseCivitWindow, this);
        this->civitwindow->SetName("CivitAiDowloaded");
        wxPersistenceManager::Get().RegisterAndRestore(this->civitwindow);
        this->civitwindow->Show();
        return;
    }
    this->civitwindow->RequestUserAttention();
}

void MainWindowUI::onModelSelect(wxCommandEvent& event) {
    // check if really selected a model, or just the first element, which is
    // always exists...
    if (this->m_model->GetSelection() == 0) {
        this->m_generate1->Disable();
        this->m_generate2->Disable();
        this->m_statusBar166->SetStatusText(_("No model selected"));
        return;
    }
    auto name = this->m_model->GetStringSelection().ToStdString();
    // img2img has an image
    if (!this->m_open_image->GetPath().empty()) {
        this->m_generate1->Enable();
    }
    this->m_generate2->Enable();
    this->sd_params->model_path = this->ModelFiles.at(name);
    this->m_statusBar166->SetStatusText(
        wxString::Format(_("Model: %s"), this->sd_params->model_path));
}

void MainWindowUI::onTypeSelect(wxCommandEvent& event) {
    wxChoice* c = (wxChoice*)event.GetEventObject();
    if (c->GetStringSelection() == "Q4_2 - not supported" ||
        c->GetStringSelection() == "Q4_5 - not supported") {
        c->SetSelection(sizeof(sd_gui_utils::sd_type_names[0]) - 1);
    }
}

void MainWindowUI::onVaeSelect(wxCommandEvent& event) {
    // TODO: Implement onVaeSelect
}

void MainWindowUI::onRandomGenerateButton(wxCommandEvent& event) {
    this->m_seed->SetValue(sd_gui_utils::generateRandomInt(100000000, 999999999));
}

void MainWindowUI::OnWHChange(wxSpinEvent& event) {
    int w = this->m_width->GetValue();
    int h = this->m_height->GetValue();

    // if (!(w % 256 == 0) || !(h % 256 == 0)) {
    //     this->m_generate2->Enable(false);
    // wxSpinCtrl *ctrl = static_cast<wxSpinCtrl *>(event.GetEventObject());
    //  ctrl->SetBackgroundColour(wxColour(255, 0, 0));
    //     wxMessageDialog(this, _("The resolution should be divisible by 256"))
    //         .ShowModal();
    //     this->m_width->SetValue(this->init_width);
    //     this->m_height->SetValue(this->init_height);
    //     return;
    // } else {
    if (this->m_model->GetCurrentSelection() > 0) {
        this->m_generate2->Enable(true);
    }
    //}

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
    // int prev_selected = event.GetOldSelection();
    //  0 -> jobs and images
    //  1 -> txt2img
    //  2 -> img2img
    //  3 -> upscaler
    //  4 -> models
    if (selected ==
        2)  // on img2img and img2vid the vade_decode_only is false, otherwise true
    {
        this->m_vae_decode_only->SetValue(false);
    } else {
        this->m_vae_decode_only->SetValue(true);
    }
    if (selected == 0 || selected == 1 || selected == 2 || selected == 4) {
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

        if (selected == 2)  // not working on img2img.. this is hardcoded in sd.cpp
                            // to only 1 image...
        {
            this->m_batch_count->Disable();
            this->m_batch_count->SetValue(1);
        } else {
            this->m_batch_count->Enable();
            this->m_batch_count->SetToolTip("");
        }
        this->m_sampler->Enable();

        this->m_preset_list->Enable();
        this->m_save_preset->Enable();

        this->m_seed->Enable();
        this->m_steps->Enable();

        if (selected == 1 && this->m_controlnetImageOpen->GetPath().length() > 0) {
            this->m_width->Disable();
            this->m_height->Disable();
            this->m_button7->Disable();  // swap button
        } else {
            this->m_width->Enable();
            this->m_height->Enable();
            this->m_button7->Enable();
        }
        // img2img
        if (selected == 2 && !this->m_open_image->GetPath().empty()) {
            this->m_width->Disable();
            this->m_height->Disable();
            this->m_button7->Disable();  // swap button
        }
    }
    // upscaler
    if (selected == 3) {
        this->m_model->Disable();
        this->m_type->Enable();
        this->m_vae->Disable();
        this->m_vae_tiling->Disable();
        this->m_cfg->Disable();
        this->m_clip_skip->Disable();
        this->m_taesd->Disable();
        this->m_batch_count->Disable();
        this->m_sampler->Disable();
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

        int id               = store->GetItemData(item);
        QM::QueueItem* qitem = this->qmanager->GetItemPtr(id);

        menu->Append(1, _("Requeue"));
        if (qitem->mode == QM::GenerationMode::IMG2IMG ||
            qitem->mode == QM::GenerationMode::UPSCALE) {
            if (!std::filesystem::exists(qitem->initial_image)) {
                menu->Enable(1, false);
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
            menu->Append(8, qitem->status == QM::QueueStatus::PENDING ? _("Pause")
                                                                      : _("Resume"));
        }

        menu->AppendSeparator();
        menu->Append(99, "Delete");
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
        if (!modelinfo->civitaiPlainJson.empty() &&
            modelinfo->hash_progress_size == 0) {
            menu->Append(105, _("Force update info from CivitAi"));
        }

        if (modelinfo->model_type == sd_gui_utils::DirTypes::CHECKPOINT) {
            menu->Append(200, wxString::Format(_("Select model %s to the next job"),
                                               modelinfo->name));
            if (modelinfo->state == sd_gui_utils::CivitAiState::OK) {
                menu->Append(199, _("Open model on CivitAi.com in default browser"));
            }
        }
        if (modelinfo->model_type == sd_gui_utils::DirTypes::LORA) {
            auto shortname =
                std::filesystem::path(modelinfo->name).replace_extension().string();
            menu->Append(
                101, wxString::Format(_("Append to text2img prompt <lora:%s:0.5>"),
                                      shortname));
            menu->Append(102, wxString::Format(
                                  _("Append to text2img neg. prompt <lora:%s:0.5>"),
                                  shortname));
            menu->Append(103,
                         wxString::Format(_("Append to img2img prompt <lora:%s:0.5>"),
                                          shortname));
            menu->Append(104, wxString::Format(
                                  _("Append to img2img neg. prompt <lora:%s:0.5>"),
                                  shortname));
        }
        if (modelinfo->model_type == sd_gui_utils::DirTypes::EMBEDDING) {
            auto shortname =
                std::filesystem::path(modelinfo->name).replace_extension().string();
            menu->Append(
                111, wxString::Format(_("Append to text2img prompt %s"), shortname));
            menu->Append(112, wxString::Format(_("Append to text2img neg. prompt %s"),
                                               shortname));
            menu->Append(
                113, wxString::Format(_("Append to img2img prompt %s"), shortname));
            menu->Append(114, wxString::Format(_("Append to img2img neg. prompt %s"),
                                               shortname));
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
    auto file  = files[0];
    auto path  = std::filesystem::path(file.ToStdString());
    // check if file extension is img
    if (path.extension() != ".jpg" && path.extension() != ".png") {
        wxMessageBox(wxString::Format(_("Image type not supported: %s"),
                                      path.extension().string()));
        return;
    }

    wxImage img;
    if (!img.LoadFile(path.string())) {
        wxMessageBox(wxString::Format(_("Image type not supported: %s"),
                                      path.extension().string()));
        // invalid img
        return;
    }

    // only jpeg supported now
    if (img.GetType() != wxBITMAP_TYPE_JPEG) {
        wxMessageBox(wxString::Format(_("Only jpeg supported: %s"),
                                      path.extension().string()));
        return;
    }

    /// get meta
    try {
        auto image = Exiv2::ImageFactory::open(file.ToStdString());
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
                        this->imageCommentToGuiParams(getParams, sd_gui_utils::SDMode::TXT2IMG);
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
    wxTextCtrl* textCtrl = static_cast<wxTextCtrl*>(event.GetEventObject());

    // Szöveg megszerzése a szövegmezőből
    wxString text = textCtrl->GetValue();
    // Az egyező szavakhoz tartozó stílusok tárolása
    std::vector<sd_gui_utils::stringformat> formats;

    std::vector<std::string> words;
    wxStringTokenizer tokenizer(text, " \r\n,.", wxTOKEN_DEFAULT);
    while (tokenizer.HasMoreTokens()) {
        std::string word = tokenizer.GetNextToken().ToStdString();
        sd_gui_utils::stringformat _f;
        _f.textAttr = this->defaultTextCtrlStyle;
        _f.end      = tokenizer.GetPosition();
        _f.start    = _f.end - word.size();
        _f.string   = word;
        formats.emplace_back(_f);
    }
    auto modellist = this->ModelManager->getList();
    // Formázás alkalmazása az egyező szavakra
    for (const auto& format : formats) {
        if (format.string.length() < 4) {
            continue;
        }

        auto it =
            std::find_if(modellist.begin(), modellist.end(),
                         [&format](const sd_gui_utils::ModelFileInfo& fileInfo) {
                             return fileInfo.name == format.string;
                         });

        if (it != modellist.end()) {
            wxTextAttr needStyle = wxTextAttr(wxNullColour, wxColour(*wxLIGHT_GREY)),
                       neededStyle =
                           wxTextAttr(wxNullColour, wxColour(*wxLIGHT_GREY));
            if (textCtrl->GetStyle(format.start + 1, needStyle)) {
                if (needStyle.GetTextColour() != neededStyle.GetTextColour() ||
                    needStyle.GetBackgroundColour() !=
                        neededStyle.GetBackgroundColour()) {
                    textCtrl->SetStyle(format.start, format.end, neededStyle);
                    textCtrl->SetDefaultStyle(this->defaultTextCtrlStyle);
                }
            }
        }
    }

    event.Skip();
}

void MainWindowUI::OnNegPromptText(wxCommandEvent& event) {
    wxTextCtrl* ctrl = static_cast<wxTextCtrl*>(event.GetEventObject());
    ctrl->SetDefaultStyle(this->defaultTextCtrlStyle);

    std::string s;
    std::stringstream ss(ctrl->GetValue().ToStdString());
    std::vector<std::string> v;

    long curr_start_pos = 0;
    long curr_end_pos   = curr_start_pos + 1;

    while (getline(ss, s, ' ')) {
        v.push_back(s);
    }

    for (unsigned int i = 0; i < v.size(); i++) {
        curr_end_pos = curr_start_pos + v[i].length();
        // check if it is a embedding
        auto safetensor = std::filesystem::path(this->cfg->embedding + "/" + v[i] +
                                                ".safetensors");
        auto pt         = std::filesystem::path(this->cfg->embedding + "/" + v[i] + ".pt");
        if (std::filesystem::exists(safetensor) || std::filesystem::exists(pt)) {
            ctrl->SetStyle(curr_start_pos, curr_end_pos,
                           wxTextAttr(*wxGREEN, wxNullColour, *wxITALIC_FONT));
        }
        //   if (v[i].substr(0, 6) == "<lora:")
        //  {
        //      ctrl->SetStyle(curr_start_pos, curr_end_pos, wxTextAttr(*wxRED,
        //      wxNullColour, *wxITALIC_FONT));
        //  }
        curr_start_pos = curr_start_pos + v[i].length() + 1;
    }
}

void MainWindowUI::onGenerate(wxCommandEvent& event) {
    this->initConfig();
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
        QM::QueueItem* item      = new QM::QueueItem();
        item->params             = *this->sd_params;
        item->model              = this->m_upscaler_model->GetStringSelection().ToStdString();
        item->mode               = type;
        item->params.esrgan_path = this->EsrganFiles.at(
            this->m_upscaler_model->GetStringSelection().ToStdString());
        item->initial_image    = this->m_upscaler_filepicker->GetPath();
        item->params.mode      = sd_gui_utils::SDMode::MODE_COUNT;
        item->params.n_threads = this->cfg->n_threads;
        if (this->cfg->save_all_image) {
            item->images.emplace_back(new QM::QueueItemImage(
                {item->initial_image, QM::QueueItemImageType::INITIAL}));
        }
        this->qmanager->AddItem(item, false);
        return;
    }

    // prepare params
    // this->sd_params->model_path =
    // this->ModelFiles.at(this->m_model->GetStringSelection().ToStdString());
    auto mindex = this->m_model->GetCurrentSelection();
    sd_gui_utils::ModelFileInfo* modelinfo =
        reinterpret_cast<sd_gui_utils::ModelFileInfo*>(
            this->m_model->GetClientData(mindex));
    this->sd_params->model_path = modelinfo->path;

    if (this->m_taesd->GetCurrentSelection() > 0) {
        this->sd_params->taesd_path =
            this->TaesdFiles.at(this->m_taesd->GetStringSelection().ToStdString());
    } else {
        this->sd_params->taesd_path = "";
    }
    if (this->m_vae->GetCurrentSelection() > 0) {
        this->sd_params->vae_path =
            this->VaeFiles.at(this->m_vae->GetStringSelection().ToStdString());
    } else {
        this->sd_params->vae_path = "";
    }

    this->sd_params->lora_model_dir  = this->cfg->lora;
    this->sd_params->embeddings_path = this->cfg->embedding;
    this->sd_params->n_threads       = this->cfg->n_threads;

    if (type == QM::GenerationMode::TXT2IMG) {
        this->sd_params->prompt = this->m_prompt->GetValue().ToStdString();
        this->sd_params->negative_prompt =
            this->m_neg_prompt->GetValue().ToStdString();
    }
    if (type == QM::GenerationMode::IMG2IMG) {
        this->sd_params->prompt = this->m_prompt2->GetValue().ToStdString();
        this->sd_params->negative_prompt =
            this->m_neg_prompt2->GetValue().ToStdString();
    }
    this->sd_params->cfg_scale    = static_cast<float>(this->m_cfg->GetValue());
    this->sd_params->seed         = this->m_seed->GetValue();
    this->sd_params->clip_skip    = this->m_clip_skip->GetValue();
    this->sd_params->sample_steps = this->m_steps->GetValue();

    if (type == QM::GenerationMode::TXT2IMG) {
        this->sd_params->control_strength = this->m_controlnetStrength->GetValue();
    }

    if (type == QM::GenerationMode::IMG2IMG) {
        this->sd_params->strength = this->m_strength->GetValue();
    }

    if (this->m_controlnetModels->GetCurrentSelection() > 0 &&
        type == QM::GenerationMode::TXT2IMG) {
        this->sd_params->controlnet_path = this->ControlnetModels.at(
            this->m_controlnetModels->GetStringSelection().ToStdString());
        // do not set the control image if we have no model
        if (this->m_controlnetImageOpen->GetPath().length() > 0) {
            this->sd_params->control_image_path =
                this->m_controlnetImageOpen->GetPath().ToStdString();
        }
    } else {
        this->sd_params->controlnet_path    = "";
        this->sd_params->control_image_path = "";
    }

    this->sd_params->sample_method = (sample_method_t)this->m_sampler->GetCurrentSelection();

    if (this->m_type->GetCurrentSelection() != 0) {
        auto selected = this->m_type->GetStringSelection();
        for (auto types : sd_gui_utils::sd_type_gui_names) {
            if (types.second == selected) {
                this->sd_params->wtype = (sd_type_t)types.first;
            }
        }
    } else {
        this->sd_params->wtype = sd_type_t::SD_TYPE_COUNT;
    }

    if (this->m_scheduler->GetCurrentSelection() != 0) {
        auto selected = this->m_scheduler->GetStringSelection();
        for (auto schedulers : sd_gui_utils::sd_scheduler_gui_names) {
            if (schedulers.second == selected) {
                this->sd_params->schedule = (schedule_t)schedulers.first;
            }
        }
    } else {
        this->sd_params->schedule = schedule_t::DEFAULT;
    }

    this->sd_params->batch_count = this->m_batch_count->GetValue();

    this->sd_params->width  = this->m_width->GetValue();
    this->sd_params->height = this->m_height->GetValue();

    this->sd_params->vae_tiling = this->m_vae_tiling->GetValue();

    QM::QueueItem* item = new QM::QueueItem();
    item->params        = *this->sd_params;
    item->model         = this->m_model->GetStringSelection().ToStdString();
    item->mode          = type;

    if (type == QM::GenerationMode::IMG2IMG) {
        item->initial_image = this->currentInitialImagePath;
        if (this->cfg->save_all_image) {
            item->images.emplace_back(new QM::QueueItemImage(
                {item->initial_image, QM::QueueItemImageType::INITIAL}));
        }
    }

    if (item->params.seed == -1) {
        item->params.seed =
            sd_gui_utils::generateRandomInt(100000000, this->m_seed->GetMax());
        this->m_seed->SetValue(item->params.seed);
    }

    // add the queue item
    this->qmanager->AddItem(item);
}

void MainWindowUI::OnControlnetImageOpen(wxFileDirPickerEvent& event) {
    if (event.GetPath().empty()) {
        return;
    }
    this->onControlnetImageOpen(event.GetPath().ToStdString());
}

void MainWindowUI::OnControlnetImagePreviewButton(wxCommandEvent& event) {
    MainWindowImageDialog* dialog = new MainWindowImageDialog(this);
    dialog->SetName("controlnet_preview");
    wxImage img(*this->currentControlnetImage);
    auto size = img.GetSize();

    dialog->SetSize(size.GetWidth() + 100, size.GetHeight() + 100);
    wxString title = wxString::Format(_("Controlnet Image %dx%dpx"),
                                      size.GetWidth(), size.GetHeight());
    dialog->SetTitle(title);
    dialog->SetIcon(this->GetIcon());
    dialog->m_bitmap->SetBitmap(img);
    wxPersistenceManager::Get().RegisterAndRestore(dialog);
    dialog->ShowModal();
}

void MainWindowUI::OnControlnetImageDelete(wxCommandEvent& event) {
    this->m_controlnetImagePreviewButton->Disable();
    this->m_controlnetImageDelete->Disable();
    this->m_controlnetImageOpen->SetPath("");
    this->currentControlnetImage        = NULL;
    this->currentControlnetImagePreview = NULL;
    auto origSize                       = this->m_controlnetImagePreview->GetSize();
    this->m_controlnetImagePreview->SetBitmap(this->ControlnetOrigPreviewBitmap);
    this->m_controlnetImagePreview->SetSize(origSize);
    this->m_width->Enable();
    this->m_height->Enable();
    this->m_button7->Enable();  // swap resolution
    this->m_controlnetModels->Select(0);
}

void MainWindowUI::Onimg2imgDropFile(wxDropFilesEvent& event) {
    // only just one file.. sry...
    auto files = event.GetFiles();
    auto file  = files[0];

    this->onimg2ImgImageOpen(file.ToStdString());
}

void MainWindowUI::OnImageOpenFileChanged(wxFileDirPickerEvent& event) {
    if (event.GetPath().empty()) {
        this->m_generate1->Disable();
        return;
    }
    this->onimg2ImgImageOpen(event.GetPath().ToStdString());
}

void MainWindowUI::OnImg2ImgPreviewButton(wxCommandEvent& event) {
    MainWindowImageDialog* dialog = new MainWindowImageDialog(this);
    dialog->SetName("img2img_preview");
    wxImage img(*this->currentInitialImage);
    auto size = img.GetSize();

    dialog->SetSize(size.GetWidth() + 100, size.GetHeight() + 100);
    dialog->SetTitle(_("IMG2IMG - original image"));
    dialog->m_bitmap->SetBitmap(img);
    wxPersistenceManager::Get().RegisterAndRestore(dialog);
    dialog->ShowModal();
}

void MainWindowUI::OnDeleteInitialImage(wxCommandEvent& event) {
    this->currentInitialImage        = NULL;
    this->currentInitialImagePreview = NULL;
    this->currentInitialImagePath    = "";
    auto origSize                    = this->m_img2img_preview->GetSize();
    this->m_img2img_preview->SetBitmap(this->AppOrigPlaceHolderBitmap);
    this->m_img2img_preview->SetSize(origSize);
    this->m_img2im_preview_img->Disable();
    this->m_delete_initial_img->Disable();
    this->m_open_image->SetPath("");
    this->m_generate2->Disable();
}

void MainWindowUI::OnUpscalerDropFile(wxDropFilesEvent& event) {
    // only just one file.. sry...
    auto files = event.GetFiles();
    auto file  = files[0];
    this->onUpscaleImageOpen(file.ToStdString());
}

void MainWindowUI::OnImageOpenFilePickerChanged(wxFileDirPickerEvent& event) {
    if (event.GetPath().empty()) {
        this->m_generate2->Disable();
        return;
    }
    this->onUpscaleImageOpen(event.GetPath().ToStdString());
}

void MainWindowUI::OnDeleteUpscaleImage(wxCommandEvent& event) {
    this->m_static_upscaler_height->SetLabel("");
    this->m_static_upscaler_width->SetLabel("");
    this->m_static_upscaler_target_height->SetLabel("");
    this->m_static_upscaler_target_width->SetLabel("");
    auto origSize = this->m_upscaler_source_image->GetSize();
    this->m_upscaler_source_image->SetBitmap(this->AppOrigPlaceHolderBitmap);
    this->m_upscaler_source_image->SetSize(origSize);
    this->currentUpscalerSourceImage = NULL;
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
    if (this->currentUpscalerSourceImage->IsOk()) {
        this->m_static_upscaler_height->SetLabel(wxString::Format(
            _("%dpx"), this->currentUpscalerSourceImage->GetHeight()));
        this->m_static_upscaler_width->SetLabel(wxString::Format(
            _("%dpx"), this->currentUpscalerSourceImage->GetWidth()));

        int factor        = this->m_upscaler_factor->GetValue();
        int target_width  = factor * this->currentUpscalerSourceImage->GetWidth();
        int target_height = factor * this->currentUpscalerSourceImage->GetHeight();
        this->m_static_upscaler_target_height->SetLabel(
            wxString::Format(_("%dpx"), target_height));
        this->m_static_upscaler_target_width->SetLabel(
            wxString::Format(_("%dpx"), target_width));
    }
}

void MainWindowUI::OnCheckboxLoraFilter(wxCommandEvent& event) {
    auto value = this->m_modellist_filter->GetValue().ToStdString();
    this->refreshModelTable(value);
}

void MainWindowUI::OnCheckboxCheckpointFilter(wxCommandEvent& event) {
    auto value = this->m_modellist_filter->GetValue().ToStdString();
    this->refreshModelTable(value);
}

void MainWindowUI::OnModellistFilterKeyUp(wxKeyEvent& event) {
    auto value = this->m_modellist_filter->GetValue().ToStdString();
    this->refreshModelTable(value);
}

void MainWindowUI::OnDataModelActivated(wxDataViewEvent& event) {}

void MainWindowUI::OnDataModelSelected(wxDataViewEvent& event) {
    auto store = this->m_data_model_list->GetStore();
    auto row   = this->m_data_model_list->GetSelectedRow();
    if (row == wxNOT_FOUND) {
        return;
    }
    auto currentItem = store->GetItem(row);
    sd_gui_utils::ModelFileInfo* _item =
        reinterpret_cast<sd_gui_utils::ModelFileInfo*>(
            store->GetItemData(currentItem));
    sd_gui_utils::ModelFileInfo* modelinfo =
        this->ModelManager->getIntoPtr(_item->path);

    // download infos only when empty and sha256 is present
    if (modelinfo->civitaiPlainJson.empty() && !modelinfo->sha256.empty()) {
        this->threads.emplace_back(
            new std::thread(&MainWindowUI::threadedModelInfoDownload, this,
                            this->GetEventHandler(), modelinfo));
    }
    this->UpdateModelInfoDetailsFromModelList(modelinfo);
}

void MainWindowUI::onSamplerSelect(wxCommandEvent& event) {
    this->sd_params->sample_method =
        (sample_method_t)this->m_sampler->GetSelection();
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
            if (this->m_sampler->GetStringSelection().ToStdString() == sampler) {
                preset.sampler = (sample_method_t)index;
                break;
            }
            index++;
        }
        preset.batch = this->m_batch_count->GetValue();
        preset.name  = preset_name.ToStdString();
        preset.type  = this->m_type->GetStringSelection().ToStdString();
        if (this->m_notebook1302->GetSelection() == 1) {
            preset.mode = sd_gui_utils::modes_str[QM::GenerationMode::TXT2IMG];
        }
        if (this->m_notebook1302->GetSelection() == 2) {
            preset.mode = sd_gui_utils::modes_str[QM::GenerationMode::IMG2IMG];
        }

        nlohmann::json j(preset);
        std::string presetfile =
            wxString::Format("%s%s%s.json", this->cfg->presets,
                             wxString(wxFileName::GetPathSeparator()), preset.name)
                .ToStdString();

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
    auto name = this->m_preset_list->GetStringSelection().ToStdString();

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
    this->m_seed->SetValue(item.params.seed);
    this->m_width->SetValue(item.params.width);
    this->m_height->SetValue(item.params.height);
    this->m_steps->SetValue(item.params.sample_steps);
    this->m_clip_skip->SetValue(item.params.clip_skip);
    this->m_controlnetStrength->SetValue(item.params.control_strength);
    this->m_cfg->SetValue(item.params.cfg_scale);
    this->m_batch_count->SetValue(item.params.batch_count);

    if (!item.params.model_path.empty() &&
        std::filesystem::exists(item.params.model_path)) {
        sd_gui_utils::ModelFileInfo model = this->ModelManager->getInfo(item.params.model_path);
        this->ChangeModelByInfo(model);
        this->m_generate2->Enable();
        if (!this->m_open_image->GetPath().empty()) {
            this->m_generate1->Enable();
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
                for (unsigned int _u = 0; _u < this->m_controlnetModels->GetCount();
                     ++_u) {
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
    data.push_back(
        wxVariant(wxString::Format("%s %s", modelinfo->CivitAiInfo.model.name,
                                   modelinfo->CivitAiInfo.name)));
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
            wxImage resized = sd_gui_utils::cropResizeImage(
                img.local_path, 256, 256, wxColour(*wxBLACK), this->cfg->thumbs_path);
            wxStaticBitmap* bitmap =
                new wxStaticBitmap(this->m_scrolledWindow4, wxID_ANY, wxNullBitmap,
                                   wxDefaultPosition, wxSize(resized.GetSize()), 0);
            bitmap->Hide();
            bitmap->SetBitmap(resized);
            auto tooltip =
                wxString::Format(_("Resolution: %s Seed: %" PRId64 " Steps: %d"),
                                 img.meta.Size, img.meta.seed, img.meta.steps);
            this->bSizer891->Add(bitmap, 0, wxALL, 2);
            this->modelImagePreviews.emplace_back(bitmap);
            bitmap->Show();
            bitmap->Bind(wxEVT_RIGHT_UP, [img, bitmap, modelinfo, tooltip,
                                          this](wxMouseEvent& event) {
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
                        menu->Append(4,
                                     wxString::Format("Select model %s", imgsModel.name));
                    } else {
                        menu->Append(4, wxString::Format("Image's model not found: %s",
                                                         img.meta.hashes.model));
                        menu->Enable(4, false);
                    }
                }

                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, img, modelinfo](
                                                            wxCommandEvent& evt) {
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

void MainWindowUI::OnQueueItemManagerItemAdded(QM::QueueItem* item) {
    wxVector<wxVariant> data;

    auto created_at = sd_gui_utils::formatUnixTimestampToDate(item->created_at);

    data.push_back(wxVariant(std::to_string(item->id)));
    data.push_back(wxVariant(created_at));
    data.push_back(wxVariant(sd_gui_utils::modes_str[item->mode]));
    data.push_back(wxVariant(item->model));

    if (item->mode == QM::GenerationMode::UPSCALE) {
        data.push_back(wxVariant("--"));  // sample method
        data.push_back(wxVariant("--"));  // seed
    } else {
        data.push_back(wxVariant(
            sd_gui_utils::sample_method_str[(int)item->params.sample_method]));
        data.push_back(wxVariant(std::to_string(item->params.seed)));
    }

    data.push_back(item->status == QM::QueueStatus::DONE ? 100
                                                         : 1);     // progressbar
    data.push_back(wxString("-.--it/s"));                          // speed
    data.push_back(wxVariant(QM::QueueStatus_str[item->status]));  // status

    auto store = this->m_joblist->GetStore();

    // only store the queue item id... not the pointer
    store->PrependItem(data, wxUIntPtr(item->id));
    this->m_static_number_of_jobs->SetLabel(wxString::Format(
        _("Number of jobs: %d"), this->m_joblist->GetItemCount()));
}

void MainWindowUI::OnQueueItemManagerItemUpdated(QM::QueueItem* item) {}

MainWindowUI::~MainWindowUI() {
    FreeSdCtxFunction free_sd_ctx =
        (FreeSdCtxFunction)this->sd_dll->GetSymbol("free_sd_ctx");
    FreeUpscalerFunction free_upscaler_ctx =
        (FreeUpscalerFunction)this->sd_dll->GetSymbol("free_upscaler_ctx");
    if (this->civitwindow != nullptr) {
        this->civitwindow->Destroy();
    }
    if (this->modelLoaded) {
        free_sd_ctx(this->txt2img_sd_ctx);
    }
    if (this->upscaleModelLoaded) {
        free_upscaler_ctx(this->upscaler_sd_ctx);
    }
    for (auto& threadPtr : threads) {
        if (threadPtr->joinable()) {
            threadPtr->join();
        }
        delete threadPtr;
    }
    for (auto& pair : this->JobTableItems) {
        delete pair.second;
    }
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

    delete this->ModelManager;
    delete this->sd_params;
    delete this->qmanager;
    delete this->currentInitialImage;
    delete this->currentInitialImagePreview;
    delete this->currentControlnetImage;
    delete this->currentControlnetImagePreview;
    delete this->currentUpscalerSourceImage;
    delete this->cfg;

    //    delete this->fileConfig;

    this->TaskBar->Destroy();
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
        int id               = store->GetItemData(currentItem);
        QM::QueueItem* qitem = this->qmanager->GetItemPtr(id);

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
                this->m_prompt->SetValue(qitem->params.prompt);
                this->m_neg_prompt->SetValue(qitem->params.negative_prompt);
                break;
            case 4:
                this->m_prompt2->SetValue(qitem->params.prompt);
                this->m_neg_prompt2->SetValue(qitem->params.negative_prompt);
                break;
            case 5: {
                auto model = this->ModelManager->getInfo(qitem->params.model_path);
                this->ChangeModelByInfo(model);
            } break;
            case 6: {
                this->m_upscaler_filepicker->SetPath(qitem->images.back()->pathname);
                this->onUpscaleImageOpen(qitem->images.back()->pathname);
                this->m_notebook1302->SetSelection(3);  // switch to the upscaler
            } break;
            case 7: {
                this->m_open_image->SetPath(qitem->images.back()->pathname);
                this->onimg2ImgImageOpen(qitem->images.back()->pathname);
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
                    this->m_static_number_of_jobs->SetLabel(wxString::Format(
                        _("Number of jobs: %d"), this->m_joblist->GetItemCount()));
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
            reinterpret_cast<sd_gui_utils::ModelFileInfo*>(
                store->GetItemData(currentItem));
        auto shortname =
            std::filesystem::path(modelinfo->name).replace_extension().string();
        switch (tu) {
            case 100:
                this->threads.emplace_back(
                    new std::thread(&MainWindowUI::threadedModelHashCalc, this,
                                    this->GetEventHandler(), modelinfo));
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
                this->threads.emplace_back(
                    new std::thread(&MainWindowUI::threadedModelInfoDownload, this,
                                    this->GetEventHandler(), modelinfo));
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
                this->m_neg_prompt2->SetValue(wxString::Format(
                    "%s %s", this->m_neg_prompt2->GetValue(), shortname));
                break;
            case 115:
                this->threads.emplace_back(
                    new std::thread(&MainWindowUI::threadedModelInfoDownload, this,
                                    this->GetEventHandler(), modelinfo));
                break;
            case 199: {
                //
                wxString url = wxString::Format("https://civitai.com/models/%d",
                                                modelinfo->CivitAiInfo.modelId);
                wxLaunchDefaultBrowser(url);
            } break;
            case 200:
                this->ChangeModelByInfo(*modelinfo);
                break;
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

void MainWindowUI::refreshModelTable(std::string filter) {
    auto types = sd_gui_utils::DirTypes::LORA |
                 sd_gui_utils::DirTypes::CHECKPOINT |
                 sd_gui_utils::DirTypes::EMBEDDING;

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
                               [](unsigned char c) { return std::tolower(c); });
                std::transform(path.begin(), path.end(), path.begin(),
                               [](unsigned char c) { return std::tolower(c); });

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
                current_progress = static_cast<int>((ele.hash_progress_size * 100) /
                                                    ele.hash_fullsize);
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

std::string
MainWindowUI::paramsToImageComment(QM::QueueItem myItem,
                                   sd_gui_utils::ModelFileInfo modelInfo) {
    auto modelPath = std::filesystem::path(modelInfo.path);

    std::string comment = "charset=Unicode " + myItem.params.prompt;
    if (!myItem.params.negative_prompt.empty()) {
        comment = comment + wxString::Format("\nNegative prompt: %s",
                                             myItem.params.negative_prompt)
                                .ToStdString();
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
            PROJECT_NAME, sd_gui_utils::modes_str[(int)myItem.mode]);

    if (!myItem.params.vae_path.empty()) {
        auto vae_path = std::filesystem::path(myItem.params.vae_path);
        comment       = comment +
                  wxString::Format(
                      " VAE: %s", vae_path.filename().replace_extension().string());
    }

    return comment;
}

template <typename T>
inline void MainWindowUI::SendThreadEvent(wxEvtHandler* eventHandler,
                                          QM::QueueEvents eventType,
                                          const T& payload,
                                          std::string text) {
    wxThreadEvent* e = new wxThreadEvent();
    e->SetString(wxString::Format("%d:%d:%s",
                                  (int)sd_gui_utils::ThreadEvents::QUEUE,
                                  (int)eventType, text));
    e->SetPayload(payload);
    wxQueueEvent(eventHandler, e);
}
template <typename T>
void MainWindowUI::SendThreadEvent(wxEvtHandler* eventHandler,
                                   sd_gui_utils::ThreadEvents eventType,
                                   const T& payload,
                                   std::string text) {
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

    for (auto const& dir_entry :
         std::filesystem::recursive_directory_iterator(basepath)) {
        if (!dir_entry.exists() || !dir_entry.is_regular_file() ||
            !dir_entry.path().has_extension()) {
            continue;
        }

        std::filesystem::path path = dir_entry.path();

        std::string ext = path.extension().string();

        if (type == sd_gui_utils::DirTypes::CHECKPOINT) {
            if (ext != ".safetensors" && ext != ".ckpt" && ext != ".gguf") {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::LORA) {
            if (ext != ".safetensors" && ext != ".ckpt" && ext != ".gguf") {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::EMBEDDING) {
            if (ext != ".safetensors" && ext != ".pt") {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::VAE) {
            if (ext != ".safetensors" && ext != ".ckpt") {
                continue;
            }
        }

        if (type == sd_gui_utils::DirTypes::PRESETS) {
            if (ext != ".json") {
                continue;
            }
        }

        if (type == sd_gui_utils::DirTypes::TAESD) {
            if (ext != ".pth" && ext != ".safetensors" && ext != ".gguf") {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::ESRGAN) {
            if (ext != ".pth" && ext != ".pt") {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::CONTROLNET) {
            if (ext != ".safetensors" && ext != ".pth") {
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

            sd_gui_utils::ModelFileInfo* _minfo =
                this->ModelManager->getIntoPtr(dir_entry.path().string());

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
        this->writeLog(wxString::Format("Loaded checkpoints: %" PRIuMAX "\n",
                                        this->ModelFiles.size()));
        if (this->ModelFiles.size() > 0) {
            this->m_model->Enable();
        } else {
            this->m_model->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::LORA) {
        this->writeLog(wxString::Format("Loaded Loras: %" PRIuMAX "\n",
                                        this->LoraFiles.size()));
    }
    if (type == sd_gui_utils::DirTypes::EMBEDDING) {
        this->writeLog(wxString::Format("Loaded embeddings: %" PRIuMAX "\n",
                                        this->EmbeddingFiles.size()));
    }
    if (type == sd_gui_utils::DirTypes::VAE) {
        this->writeLog(
            wxString::Format("Loaded vaes: %" PRIuMAX "\n", this->VaeFiles.size()));
        if (this->VaeFiles.size() > 0) {
            this->m_vae->Enable();
        } else {
            this->m_vae->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::PRESETS) {
        this->writeLog(wxString::Format("Loaded presets: %" PRIuMAX "\n",
                                        this->Presets.size()));
        if (this->Presets.size() > 0) {
            this->m_preset_list->Enable();
        } else {
            this->m_preset_list->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::TAESD) {
        this->writeLog(wxString::Format("Loaded taesd: %" PRIuMAX "\n",
                                        this->TaesdFiles.size()));
        if (this->TaesdFiles.size() > 0) {
            this->m_taesd->Enable();
        } else {
            this->m_taesd->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::ESRGAN) {
        this->writeLog(wxString::Format("Loaded esrgan: %" PRIuMAX "\n",
                                        this->EsrganFiles.size()));
        if (this->EsrganFiles.size() > 0) {
            this->m_upscaler_model->Enable();
        } else {
            this->m_upscaler_model->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::CONTROLNET) {
        this->writeLog(wxString::Format("Loaded controlnet: %" PRIuMAX "\n",
                                        this->ControlnetModels.size()));
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
    this->initConfig();
    this->settingsWindow->Destroy();
    this->Thaw();
    this->Show();
}
void MainWindowUI::OnCloseCivitWindow(wxCloseEvent& event) {
    this->civitwindow->Destroy();
    this->civitwindow = nullptr;
}

void MainWindowUI::imageCommentToGuiParams(
    std::map<std::string, std::string> params,
    sd_gui_utils::SDMode mode) {
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
            if (mode == sd_gui_utils::SDMode::IMG2IMG) {
                this->m_neg_prompt2->SetValue(item.second);
            } else {
                this->m_neg_prompt->SetValue(item.second);
            }
        }
        if (item.first == "prompt") {
            if (mode == sd_gui_utils::SDMode::IMG2IMG) {
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

void MainWindowUI::onimg2ImgImageOpen(std::string file) {
    wxImage img;
    if (img.LoadFile(file)) {
        auto origSize = this->m_img2img_preview->GetSize();

        auto preview                     = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(),
                                                                              origSize.GetHeight());
        this->currentInitialImage        = new wxImage(img);
        this->currentInitialImagePreview = new wxImage(preview);
        this->currentInitialImagePath    = file;

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
        this->m_open_image->SetPath(file);

        // png not working... yet...
        if (img.GetType() == wxBITMAP_TYPE_PNG) {
            return;
        }

        try {
            auto image = Exiv2::ImageFactory::open(file);
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
                            this->imageCommentToGuiParams(getParams, sd_gui_utils::SDMode::IMG2IMG);
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

void MainWindowUI::onUpscaleImageOpen(std::string file) {
    wxImage img;
    if (img.LoadFile(file)) {
        this->currentUpscalerSourceImage = new wxImage(img);
        this->m_upscaler_filepicker->SetPath(file);
        auto origSize = this->m_upscaler_source_image->GetSize();
        auto preview  = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(),
                                                           origSize.GetHeight());

        this->m_generate_upscaler->Enable();
        this->m_upscaler_source_image->SetScaleMode(
            wxStaticBitmap::Scale_AspectFill);
        this->m_upscaler_source_image->SetBitmap(preview);
        this->m_upscaler_source_image->SetSize(origSize);

        this->m_static_upscaler_height->SetLabel(
            wxString::Format("%dpx", img.GetHeight()));
        this->m_static_upscaler_width->SetLabel(
            wxString::Format("%dpx", img.GetWidth()));

        int factor        = this->m_upscaler_factor->GetValue();
        int target_width  = factor * img.GetWidth();
        int target_height = factor * img.GetHeight();

        this->m_static_upscaler_target_height->SetLabel(
            wxString::Format("%dpx", target_height));
        this->m_static_upscaler_target_width->SetLabel(
            wxString::Format("%dpx", target_width));
        if (this->m_upscaler_model->GetCurrentSelection() != 0) {
            this->m_generate_upscaler->Enable();
        } else {
            this->m_generate_upscaler->Disable();
        }
    }
}

void MainWindowUI::StartGeneration(QM::QueueItem* myJob) {
    switch (myJob->mode) {
        case QM::GenerationMode::TXT2IMG:
            this->threads.emplace_back(new std::thread(
                &MainWindowUI::GenerateTxt2img, this, this->GetEventHandler(), myJob));
            break;
        case QM::GenerationMode::IMG2IMG:
            this->threads.emplace_back(new std::thread(
                &MainWindowUI::GenerateImg2img, this, this->GetEventHandler(), myJob));
            break;
        case QM::GenerationMode::UPSCALE:
            this->threads.emplace_back(new std::thread(
                &MainWindowUI::GenerateUpscale, this, this->GetEventHandler(), myJob));
            break;
        default:
            return;
            break;
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
    this->m_type->Select(selected);
}

void MainWindowUI::loadShcedulerList() {
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

void MainWindowUI::OnQueueItemManagerItemStatusChanged(QM::QueueItem* item) {
    auto store = this->m_joblist->GetStore();

    int lastCol = this->m_joblist->GetColumnCount() - 1;

    for (unsigned int i = 0; i < store->GetItemCount(); i++) {
        auto currentItem     = store->GetItem(i);
        int id               = store->GetItemData(currentItem);
        QM::QueueItem* qitem = this->qmanager->GetItemPtr(id);
        if (qitem->id == item->id) {
            store->SetValueByRow(wxVariant(QM::QueueStatus_str[item->status]), i, lastCol);
            store->RowValueChanged(i, lastCol);
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
    auto msg = e.GetString().ToStdString();

    std::string token   = msg.substr(0, msg.find(":"));
    std::string content = msg.substr(msg.find(":") + 1);
    sd_gui_utils::ThreadEvents threadEvent =
        (sd_gui_utils::ThreadEvents)std::stoi(token);

    if (threadEvent == sd_gui_utils::ThreadEvents::QUEUE) {
        // only numbers here...
        QM::QueueEvents event = (QM::QueueEvents)std::stoi(content);
        QM::QueueItem* item   = e.GetPayload<QM::QueueItem*>();
        this->UpdateJobInfoDetailsFromJobQueueList(item);
        std::string title;
        std::string message;
        switch (event) {
                // new item added
            case QM::QueueEvents::ITEM_ADDED:
                this->OnQueueItemManagerItemAdded(item);
                break;
                // item status changed
            case QM::QueueEvents::ITEM_STATUS_CHANGED:
                this->OnQueueItemManagerItemStatusChanged(item);
                break;
                // item updated... ? ? ?
            case QM::QueueEvents::ITEM_UPDATED:
                this->OnQueueItemManagerItemUpdated(item);
                break;
            case QM::QueueEvents::ITEM_START:  // this is just the item start, if no mode
                                               // loaded, then wil trigger model load
                                               // event
            {
                this->StartGeneration(item);
                message = wxString::Format(
                              _("%s is just stared to generate %d images\nModel: %s"),
                              sd_gui_utils::modes_str[item->mode],
                              item->params.batch_count, item->model)
                              .ToStdString();
                if (item->mode == QM::GenerationMode::UPSCALE) {
                    title = _("Upscaling started");
                    message =
                        wxString::Format(_("Upscaling the image is started: %s\nModel: %s"),
                                         item->initial_image, item->model);
                } else {
                    if (item->params.batch_count > 1) {
                        title = wxString::Format(_("%d images generation started"),
                                                 item->params.batch_count)
                                    .ToStdString();
                    } else {
                        title = _("One image generation started!");
                        message =
                            wxString::Format(
                                _("%s is just started to generate the image\nModel: %s"),
                                sd_gui_utils::modes_str[item->mode], item->model)
                                .ToStdString();
                    }
                }
                this->ShowNotification(title, message);
            } break;
            case QM::QueueEvents::ITEM_FINISHED: {
                item->finished_at = sd_gui_utils::GetCurrentUnixTimestamp();
                message           = wxString::Format(
                              _("%s is just finished to generate %d images\nModel: %s"),
                              sd_gui_utils::modes_str[item->mode],
                              item->params.batch_count, item->model)
                              .ToStdString();
                if (item->mode == QM::GenerationMode::UPSCALE) {
                    title = _("Upscaling done");
                    message =
                        wxString::Format(_("Upscaling the image is done: %s\nModel: %s"),
                                         item->initial_image, item->model);
                } else {
                    if (item->params.batch_count > 1) {
                        title = wxString::Format(_("%d images generation done"),
                                                 item->params.batch_count)
                                    .ToStdString();
                    } else {
                        title = _("Image generation done!");
                        message =
                            wxString::Format(
                                _("%s is just finished to generate the image\nModel: %s"),
                                sd_gui_utils::modes_str[item->mode], item->model)
                                .ToStdString();
                    }
                }
                this->ShowNotification(title, message);
            } break;
            case QM::QueueEvents::ITEM_MODEL_LOADED:  // MODEL_LOAD_DONE
                if (item->mode == QM::GenerationMode::IMG2IMG ||
                    item->mode == QM::GenerationMode::TXT2IMG) {
                    this->modelLoaded = true;
                }
                if (item->mode == QM::GenerationMode::UPSCALE) {
                    this->upscaleModelLoaded = true;
                }
                this->writeLog(wxString::Format(_("Model loaded: %s\n"), item->model));
                break;
            case QM::QueueEvents::ITEM_MODEL_LOAD_START:  // MODEL_LOAD_START
                this->writeLog(
                    wxString::Format(_("Model load started: %s\n"), item->model));
                break;
            case QM::QueueEvents::ITEM_MODEL_FAILED:  // MODEL_LOAD_ERROR
                this->writeLog(
                    wxString::Format(_("Model load failed: %s\n"), item->status_message));

                if (item->mode == QM::GenerationMode::IMG2IMG ||
                    item->mode == QM::GenerationMode::TXT2IMG) {
                    this->modelLoaded = false;
                }
                if (item->mode == QM::GenerationMode::UPSCALE) {
                    this->upscaleModelLoaded = false;
                }
                title   = _("Model load failed");
                message = wxString::Format(_("The '%s' just failed to load... for more "
                                             "details please see the logs!"),
                                           item->model)
                              .ToStdString();
                this->ShowNotification(title, message);
                break;
            case QM::QueueEvents::ITEM_GENERATION_STARTED:  // GENERATION_START
                if (item->mode == QM::GenerationMode::IMG2IMG ||
                    item->mode == QM::GenerationMode::TXT2IMG) {
                    this->writeLog(wxString::Format(
                        "Diffusion started. Seed: %" PRId64
                        " Batch: %d %dx%dpx Cfg: %.1f Steps: %d\n",
                        item->params.seed, item->params.batch_count, item->params.width,
                        item->params.height, item->params.cfg_scale,
                        item->params.sample_steps));
                }
                if (item->mode == QM::GenerationMode::UPSCALE) {
                    this->writeLog(
                        wxString::Format(_("Upscale start, factor: %d image: %s\n"),
                                         item->upscale_factor, item->initial_image));
                }
                break;
            case QM::QueueEvents::ITEM_FAILED:  // GENERATION_ERROR
                this->writeLog(
                    wxString::Format(_("Generation error: %s\n"), item->status_message));
                break;
            default:
                break;
        }
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_DONE) {
        sd_gui_utils::ModelFileInfo* modelinfo =
            e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->ModelManager->UpdateInfo(modelinfo);
        this->writeLog(wxString::Format(
            _("Model civitai image downloaded for model: %s\n"), modelinfo->name));
        // update if the current selected item is the updated item info
        if (this->m_data_model_list->GetSelectedItemsCount() > 0) {
            wxDataViewItem item = this->m_data_model_list->GetCurrentItem();
            sd_gui_utils::ModelFileInfo* info =
                reinterpret_cast<sd_gui_utils::ModelFileInfo*>(
                    this->m_data_model_list->GetItemData(item));
            if (info->path == modelinfo->path) {
                this->UpdateModelInfoDetailsFromModelList(modelinfo);
            }
        }
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_START) {
        sd_gui_utils::ModelFileInfo* modelinfo =
            e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        this->ModelManager->UpdateInfo(modelinfo);
        this->writeLog(wxString::Format(
            "Model civitai %" PRIuMAX " image(s) download started for model: %s\n",
            modelinfo->CivitAiInfo.images.size(), modelinfo->name));
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
        this->writeLog(wxString::Format(
            _("Model civitai info download start: %s\n"), modelinfo->name));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FINISHED) {
        sd_gui_utils::ModelFileInfo* modelinfo =
            e.GetPayload<sd_gui_utils::ModelFileInfo*>();
        sd_gui_utils::ModelFileInfo newInfo =
            this->ModelManager->updateCivitAiInfo(modelinfo);
        sd_gui_utils::ModelFileInfo* newInfoptr =
            this->ModelManager->getIntoPtr(newInfo.path);

        if (newInfo.state == sd_gui_utils::CivitAiState::OK) {
            this->writeLog(
                wxString::Format(_("Model civitai info download finished: %s\n"),
                                 newInfo.CivitAiInfo.name));
            this->threads.emplace_back(
                new std::thread(&MainWindowUI::threadedModelInfoImageDownload, this,
                                this->GetEventHandler(), newInfoptr));
        }
        if (newInfo.state == sd_gui_utils::CivitAiState::NOT_FOUND) {
            this->writeLog(
                wxString::Format(_("Model civitai info not found: %s Hash: %s\n"),
                                 newInfo.name, newInfo.sha256.substr(0, 10)));
        }
        if (newInfo.state == sd_gui_utils::CivitAiState::ERR) {
            this->writeLog(wxString::Format(
                _("Model civitai info unkown parsing error happened: %s Hash: %s\n"),
                newInfo.name, newInfo.sha256.substr(0, 10)));
        }
        // update anyway
        this->UpdateModelInfoDetailsFromModelList(modelinfo);
        // update table
        auto store = this->m_data_model_list->GetStore();
        int civitAiCol =
            this->m_data_model_list->GetColumnCount() - 2;  // civitai col

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto _item      = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto* _qitem =
                reinterpret_cast<sd_gui_utils::ModelFileInfo*>(_item_data);
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
        if (modelinfo->civitaiPlainJson.empty()) {
            this->threads.emplace_back(
                new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
        }
        nlohmann::json j(*modelinfo);
        std::ofstream file(modelinfo->meta_file);
        file << j;
        file.close();
        modelinfo->hash_progress_size = 0;
        return;
    }
    if (threadEvent == sd_gui_utils::HASHING_PROGRESS) {
        QM::QueueItem* myjob = e.GetPayload<QM::QueueItem*>();

        // update column
        auto store = this->m_joblist->GetStore();

        int progressCol = this->m_joblist->GetColumnCount() - 3;

        size_t _x            = myjob->hash_progress_size;
        size_t _m            = myjob->hash_fullsize;
        int current_progress = 0;
        auto _hr1            = sd_gui_utils::humanReadableFileSize(static_cast<double>(_x));
        auto _hr2            = sd_gui_utils::humanReadableFileSize(static_cast<double>(_m));

        if (_m != 0) {
            current_progress = static_cast<int>((_x * 100) / _m);
        }

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto currentItem     = store->GetItem(i);
            int id               = store->GetItemData(currentItem);
            QM::QueueItem* qitem = this->qmanager->GetItemPtr(id);
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
        QM::QueueItem* myjob = e.GetPayload<QM::QueueItem*>();

        // update column
        auto store = this->m_joblist->GetStore();

        int progressCol = this->m_joblist->GetColumnCount() - 3;

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto currentItem     = store->GetItem(i);
            int id               = store->GetItemData(currentItem);
            QM::QueueItem* qitem = this->qmanager->GetItemPtr(id);
            if (qitem->id == myjob->id) {
                store->SetValueByRow(0, i, progressCol);
                store->RowValueChanged(i, progressCol);
                this->m_joblist->Refresh();
                this->m_joblist->Update();
                break;
            }
        }
        this->ModelManager->getIntoPtr(myjob->params.model_path)
            ->hash_progress_size = 0;
        return;
    }
    if (threadEvent == sd_gui_utils::GENERATION_PROGRESS) {
        QM::QueueItem* myjob = e.GetPayload<QM::QueueItem*>();

        // update column
        auto store     = this->m_joblist->GetStore();
        wxString speed = wxString::Format(
            myjob->time > 1.0f ? "%.2fs/it" : "%.2fit/s",
            myjob->time > 1.0f || myjob->time == 0 ? myjob->time
                                                   : (1.0f / myjob->time));
        int progressCol        = this->m_joblist->GetColumnCount() - 3;
        int speedCol           = this->m_joblist->GetColumnCount() - 2;
        float current_progress = 100.f * (static_cast<float>(myjob->step) /
                                          static_cast<float>(myjob->steps));

        for (unsigned int i = 0; i < store->GetItemCount(); i++) {
            auto currentItem     = store->GetItem(i);
            int id               = store->GetItemData(currentItem);
            QM::QueueItem* qitem = this->qmanager->GetItemPtr(id);
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
    auto msg            = e.GetString().ToStdString();
    std::string token   = msg.substr(0, msg.find(":"));
    std::string content = msg.substr(msg.find(":") + 1);

    if (token == "DOWNLOAD_FINISH") {
        auto payload = e.GetPayload<CivitAi::DownloadItem*>();

        std::string name =
            std::filesystem::path(payload->local_file).filename().string();

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
        auto value = this->m_modellist_filter->GetValue().ToStdString();
        this->refreshModelTable(value);

        std::string title = _("Model download finished").ToStdString();
        std::string message =
            wxString::Format(_("The model download is finished: %s"), name)
                .ToStdString();
        this->ShowNotification(title, message);

        return;
    }
    if (token == "DOWNLOAD_ERROR") {
        auto payload      = e.GetPayload<CivitAi::DownloadItem*>();
        std::string title = _("Model download failed").ToStdString();
        std::string message =
            wxString::Format(
                _("The model download is failed: %s"),
                std::filesystem::path(payload->local_file).filename().string())
                .ToStdString();
        this->ShowNotification(title, message);
        return;
    }
}

void MainWindowUI::HandleSDProgress(int step, int steps, float time, void* data) {
    sd_gui_utils::VoidHolder* objs = (sd_gui_utils::VoidHolder*)data;
    wxEvtHandler* eventHandler     = (wxEvtHandler*)objs->p1;
    QM::QueueItem* myItem          = (QM::QueueItem*)objs->p2;
    myItem->step                   = step;
    myItem->steps                  = steps;
    myItem->time                   = time;

    MainWindowUI::SendThreadEvent(
        eventHandler, sd_gui_utils::ThreadEvents::GENERATION_PROGRESS, myItem,
        wxString::Format("%d/%d", step, steps).ToStdString());
}

void MainWindowUI::UpdateJobInfoDetailsFromJobQueueList(QM::QueueItem* item) {
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
    auto current_item         = store->GetItem(row);
    int id                    = store->GetItemData(current_item);
    QM::QueueItem* store_item = this->qmanager->GetItemPtr(id);
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
    data.push_back(wxVariant(wxString(sd_gui_utils::modes_str[item->mode])));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant(_("Model")));
    data.push_back(wxVariant(wxString(item->model)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

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
        item->mode == QM::GenerationMode::TXT2IMG) {
        data.push_back(wxVariant(_("Type")));
        data.push_back(wxVariant(wxString::Format(
            "%s", sd_gui_utils::sd_type_gui_names[item->params.wtype])));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Scheduler")));
        data.push_back(wxVariant(wxString::Format(
            "%s", sd_gui_utils::sd_scheduler_gui_names[item->params.schedule])));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    if (item->mode == QM::GenerationMode::IMG2IMG) {
        data.push_back(wxVariant(_("Init image")));
        data.push_back(wxVariant(wxString(item->initial_image)));
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
        data.push_back(wxVariant(wxString(item->params.prompt)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Neg. prompt")));
        data.push_back(wxVariant(wxString(item->params.negative_prompt)));
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

        data.push_back(wxVariant(_("Cfg scale")));
        data.push_back(wxVariant(wxString::Format("%.1f", item->params.cfg_scale)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Sampler")));
        data.push_back(wxVariant(wxString::Format(
            "%s", sd_gui_utils::sample_method_str[item->params.sample_method])));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("Steps")));
        data.push_back(
            wxVariant(wxString::Format("%d", item->params.sample_steps)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("VAE")));
        data.push_back(wxVariant(wxString(item->params.vae_path)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("VAE tiling")));
        data.push_back(wxVariant(
            wxString(item->params.vae_tiling == true ? _("yes") : _("no"))));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(_("TAESD")));
        data.push_back(wxVariant(wxString(item->params.taesd_path)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
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
            data.push_back(wxVariant(wxString(item->params.controlnet_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("CN img")));
            data.push_back(wxVariant(wxString(item->params.control_image_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant(_("CN strength")));
            data.push_back(
                wxVariant(wxString::Format("%.2f", item->params.control_strength)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
    }
    int index = 0;
    for (QM::QueueItemImage* img : item->images) {
        if (std::filesystem::exists(img->pathname)) {
            auto resized = sd_gui_utils::cropResizeImage(
                img->pathname, 256, 256, wxColour(*wxBLACK), this->cfg->thumbs_path);
            img->id = index;

            wxStaticBitmap* bitmap =
                new wxStaticBitmap(this->m_scrolledWindow41, wxID_ANY, resized,
                                   wxDefaultPosition, wxDefaultSize, 0);
            bitmap->Hide();
            this->bSizer8911->Add(bitmap, 0, wxALL, 2);
            bitmap->Show();
            this->jobImagePreviews.emplace_back(bitmap);
            bitmap->Bind(wxEVT_LEFT_DCLICK, [img](wxMouseEvent& event) {
                wxLaunchDefaultApplication(
                    std::filesystem::path(img->pathname).string());
            });
            // rightclick
            bitmap->Bind(wxEVT_RIGHT_UP, [img, bitmap, item,
                                          this](wxMouseEvent& event) {
                wxMenu* menu = new wxMenu();
                // menu->Append(99,tooltip);
                // menu->Enable(99,false);
                menu->Append(6, _("Open image"));
                menu->Append(7, _("Open parent folder"));
                menu->AppendSeparator();
                menu->Append(0, wxString::Format(_("Copy seed %" PRId64),
                                                 item->params.seed + img->id));
                menu->Append(1, _("Copy prompts to text2img"));
                menu->Append(2, _("Copy prompts to img2img"));
                // menu->Append(3, "Send the image to text2img");
                menu->Append(3, _("Send the image to img2img"));

                menu->Append(5, _("Upscale"));

                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, img,
                                                         item](wxCommandEvent& evt) {
                    auto id = evt.GetId();
                    switch (id) {
                        case 0: {
                            this->m_seed->SetValue(item->params.seed);
                        } break;
                        case 1: {
                            this->m_prompt->SetValue(item->params.prompt);
                            this->m_neg_prompt->SetValue(item->params.negative_prompt);
                        } break;
                        case 2: {
                            this->m_prompt2->SetValue(item->params.prompt);
                            this->m_neg_prompt2->SetValue(item->params.negative_prompt);
                        } break;
                        case 3: {
                            this->onimg2ImgImageOpen(img->pathname);
                        } break;
                        case 5: {
                            this->onUpscaleImageOpen(img->pathname);
                            this->m_notebook1302->SetSelection(3);
                        } break;
                        case 6: {
                            wxLaunchDefaultApplication(
                                std::filesystem::path(img->pathname).string());
                        } break;
                        case 7: {
                            wxLaunchDefaultApplication(
                                std::filesystem::path(img->pathname).parent_path().string());
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

QM::QueueItem* MainWindowUI::handleSdImage(sd_image_t result,
                                           QM::QueueItem* itemPtr,
                                           wxEvtHandler* eventHandler) {
    wxImage* img = new wxImage(result.width, result.height, result.data);

    if (!img->IsOk()) {
        itemPtr->status_message = std::string(_("Invalid image from diffusion..."));
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      itemPtr);
        return itemPtr;
    }
    std::string filename  = this->cfg->output;
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
    std::string filename_without_extension;
    filename = filename + wxString(wxFileName::GetPathSeparator()).ToStdString();
    filename = filename + sd_gui_utils::modes_str[itemPtr->mode];
    filename = filename + "_";
    filename = filename + std::to_string(itemPtr->id);
    filename = filename + "_";
    filename = filename + std::to_string(itemPtr->params.seed);
    filename = filename + "_";
    filename = filename + std::to_string(result.width) + "x" +
               std::to_string(result.height);
    filename = filename + "_";

    filename_without_extension = filename;
    filename                   = filename + extension;

    int _c = 0;
    while (std::filesystem::exists(filename)) {
        filename =
            filename_without_extension + "_" + std::to_string(_c) + extension;
        _c++;
    }

    if (!img->SaveFile(filename, imgHandler)) {
        itemPtr->status_message =
            wxString::Format(_("Failed to save image into %s"), filename);
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      itemPtr);
        return itemPtr;
    } else {
        itemPtr->images.emplace_back(
            new QM::QueueItemImage({filename, QM::QueueItemImageType::GENERATED}));
        if (itemPtr->params.control_image_path.length() > 0 &&
            this->cfg->save_all_image) {
            std::string ctrlFilename = this->cfg->output;
            ctrlFilename             = filename_without_extension + "_ctrlimg_" + extension;
            wxImage _ctrlimg(itemPtr->params.control_image_path);
            _ctrlimg.SaveFile(ctrlFilename);
            itemPtr->images.emplace_back(new QM::QueueItemImage(
                {ctrlFilename, QM::QueueItemImageType::CONTROLNET}));
        }
        // add generation parameters into the image meta
        if (this->cfg->image_type == sd_gui_utils::imageTypes::JPG) {
            std::string comment = this->paramsToImageComment(*itemPtr, this->ModelManager->getInfo(itemPtr->params.model_path));

            try {
                auto image = Exiv2::ImageFactory::open(filename);
                image->readMetadata();
                Exiv2::ExifData& exifData          = image->exifData();
                exifData["Exif.Photo.UserComment"] = comment;
                exifData["Exif.Image.XPComment"]   = comment;
                // image->setComment(comment);

                // PNG fájl metaadatainak frissítése
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
    sd_gui_utils::VoidHolder* holder =
        static_cast<sd_gui_utils::VoidHolder*>(custom_pointer);
    wxEvtHandler* eventHandler = (wxEvtHandler*)holder->p1;
    QM::QueueItem* myItem      = (QM::QueueItem*)holder->p2;
    // update the model list too
    sd_gui_utils::ModelFileInfo* modelinfo =
        (sd_gui_utils::ModelFileInfo*)holder->p3;
    modelinfo->hash_progress_size = readed_size;
    modelinfo->hash_fullsize      = myItem->hash_fullsize;
    myItem->hash_progress_size    = readed_size;

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::HASHING_PROGRESS, myItem);

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
}

void MainWindowUI::ShowNotification(std::string title, std::string message) {
    if (this->cfg->show_notifications) {
        wxNotificationMessage notification(title, message, this);
#ifdef _WIN64
        notification.UseTaskBarIcon(this->TaskBar);
#endif
#ifdef _WIN32
        notification.UseTaskBarIcon(this->TaskBar);
#endif
        notification.Show(this->cfg->notification_timeout);
    }

    this->TaskBar->SetIcon(this->TaskBarIcon,
                           wxString::Format("%s - %s", this->GetTitle(), title));
}

void MainWindowUI::HandleSDLog(sd_log_level_t level, const char* text, void* data) {
    // TODO: hadle visible levels from the GUI
    //  if (level == sd_log_level_t::SD_LOG_INFO || level ==
    //  sd_log_level_t::SD_LOG_ERROR)
    //  {
    auto* eventHandler = (wxEvtHandler*)data;
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::SD_MESSAGE, level, text);
    //  }
    // TaskBar->SetIcon(, this->GetTitle());
}

sd_ctx_t* MainWindowUI::LoadModelv2(wxEvtHandler* eventHandler, QM::QueueItem* myItem) {
    MainWindowUI::SendThreadEvent(eventHandler,
                                  QM::QueueEvents::ITEM_MODEL_LOAD_START, myItem);

    bool model_exists = this->ModelManager->exists(myItem->params.model_path);
    if (!model_exists) {
        this->ModelManager->addModel(myItem->params.model_path,
                                     sd_gui_utils::DirTypes::CHECKPOINT,
                                     myItem->model);
    }
    // check if the model have a sha256
    if (model_exists) {
        auto model = this->ModelManager->getInfo(myItem->params.model_path);
        if (model.sha256.empty()) {
            myItem->hash_fullsize            = model.size;
            sd_gui_utils::VoidHolder* holder = new sd_gui_utils::VoidHolder;
            holder->p1                       = (void*)eventHandler;
            holder->p2                       = (void*)myItem;
            holder->p3                       = (void*)this->ModelManager->getIntoPtr(model.path);
            this->voids.emplace_back(holder);
            auto hash = sd_gui_utils::sha256_file_openssl(
                model.path.c_str(), (void*)holder,
                &MainWindowUI::ModelHashingCallback);
            this->ModelManager->setHash(myItem->params.model_path, hash);
        }
    }
    NewSdCtxFunction new_sd_ctx =
        (NewSdCtxFunction)this->sd_dll->GetSymbol("new_sd_ctx");
    if (!new_sd_ctx) {
        myItem->status_message =
            wxString(_("Failed to load new_sd_ctx symbol from backend!"));
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      myItem);
        this->modelLoaded = false;
        return nullptr;
    }
    /*
        sd_ctx_t *sd_ctx_ = new_sd_ctx(
            sd_gui_utils::repairPath(myItem->params.model_path).c_str(),      //
       model path sd_gui_utils::repairPath(myItem->params.vae_path).c_str(), //
       vae path sd_gui_utils::repairPath(myItem->params.taesd_path).c_str(), //
       taesd path
            sd_gui_utils::repairPath(myItem->params.controlnet_path).c_str(), //
       controlnet path
            sd_gui_utils::repairPath(myItem->params.lora_model_dir).c_str(),  //
       lora path sd_gui_utils::repairPath(myItem->params.embeddings_path).c_str(),
       // embedding path (myItem->mode == QM::GenerationMode::TXT2IMG), // vae
       decode only (img2img = false) myItem->params.vae_tiling, // vae tiling
            false,                                                            //
       free params immediatelly myItem->params.n_threads, myItem->params.wtype,
            myItem->params.rng_type,
            myItem->params.schedule,
            myItem->params.keep_control_net_cpu);
            */
    sd_ctx_t* sd_ctx_ = new_sd_ctx(
        sd_gui_utils::repairPath(myItem->params.model_path).c_str(),   // model path
        sd_gui_utils::repairPath(myItem->params.clip_l_path).c_str(),  // clip path
        sd_gui_utils::repairPath(myItem->params.t5xxl_path).c_str(),   // t5xxl path
        sd_gui_utils::repairPath(myItem->params.diffusion_model_path)
            .c_str(),                                                 // t5xxl path
        sd_gui_utils::repairPath(myItem->params.vae_path).c_str(),    // vae path
        sd_gui_utils::repairPath(myItem->params.taesd_path).c_str(),  // taesd path
        sd_gui_utils::repairPath(myItem->params.controlnet_path)
            .c_str(),  // controlnet path
        sd_gui_utils::repairPath(myItem->params.lora_model_dir)
            .c_str(),  // lora path
        sd_gui_utils::repairPath(myItem->params.embeddings_path)
            .c_str(),  // embedding path
        sd_gui_utils::repairPath(myItem->params.stacked_id_embeddings_path)
            .c_str(),                         // stacked id embedding path
        this->m_vae_decode_only->GetValue(),  // vae decode only
        myItem->params.vae_tiling,            // vae tiling
        false,                                // free params immediatelly
        myItem->params.n_threads, myItem->params.wtype, myItem->params.rng_type,
        myItem->params.schedule, myItem->params.clip_on_cpu,
        myItem->params.control_net_cpu, myItem->params.vae_on_cpu

    );

    if (sd_ctx_ == NULL) {
        this->modelLoaded = false;
        return nullptr;
    } else {
        myItem->status_message = myItem->params.model_path;
        MainWindowUI::SendThreadEvent(eventHandler,
                                      QM::QueueEvents::ITEM_MODEL_LOADED, myItem);
        this->modelLoaded            = true;
        this->currentModel           = myItem->params.model_path;
        this->currentVaeModel        = myItem->params.vae_path;
        this->currentTaesdModel      = myItem->params.taesd_path;
        this->currentwType           = myItem->params.wtype;
        this->currentControlnetModel = myItem->params.controlnet_path;
        this->lastGenerationMode     = myItem->mode;
    }
    return sd_ctx_;
}

upscaler_ctx_t* MainWindowUI::LoadUpscaleModel(wxEvtHandler* eventHandler,
                                               QM::QueueItem* myItem) {
    NewUpscalerCtxFunction new_upscaler_ctx =
        (NewUpscalerCtxFunction)this->sd_dll->GetSymbol("new_upscaler_ctx");

    if (!new_upscaler_ctx) {
        myItem->status_message = wxString(_("Failed to load new_upscaler_ctx symbol from backend!"));
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, myItem);
        return NULL;
    }
    upscaler_ctx_t* u_ctx =
        new_upscaler_ctx(myItem->params.esrgan_path.c_str(), myItem->params.n_threads, myItem->params.wtype);
    if (u_ctx == NULL) {
        this->upscaleModelLoaded = false;
        return NULL;
    } else {
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_LOADED, myItem);
        this->currentUpscalerModel = myItem->params.esrgan_path;
        this->upscaleModelLoaded   = true;
    }
    return u_ctx;
}

void MainWindowUI::onControlnetImageOpen(std::string file) {
    wxImage img;
    if (img.LoadFile(file)) {
        this->m_controlnetImageOpen->SetPath(file);
        auto origSize = this->m_controlnetImagePreview->GetSize();
        auto preview  = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(),
                                                           origSize.GetHeight());

        this->currentControlnetImage        = new wxImage(img);
        this->currentControlnetImagePreview = new wxImage(preview);

        this->m_controlnetImagePreview->SetScaleMode(
            wxStaticBitmap::Scale_AspectFill);
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
    sd_gui_utils::ModelFileInfo minfo =
        this->ModelManager->findInfoByName(ModelName.ToStdString());

    for (unsigned int _z = 0; _z < this->m_model->GetCount(); _z++) {
        sd_gui_utils::ModelFileInfo* m =
            reinterpret_cast<sd_gui_utils::ModelFileInfo*>(
                this->m_model->GetClientData(_z));

        if (m == nullptr) {
            continue;
        }
        if (m->path == minfo.path) {
            this->m_model->SetSelection(_z);
            this->m_generate2->Enable();
            if (this->m_open_image->GetPath().length() > 0) {
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
    std::string oldSelection = this->m_model->GetStringSelection().ToStdString();

    this->LoadFileList(sd_gui_utils::DirTypes::CHECKPOINT);

    if (this->ModelFilesIndex.find(oldSelection) != this->ModelFilesIndex.end()) {
        this->m_model->SetSelection(this->ModelFilesIndex[oldSelection]);
    }
}

void MainWindowUI::initConfig() {
    if (this->fileConfig != nullptr) {
        delete this->fileConfig;
    }
    this->fileConfig =
        new wxFileConfig(PROJECT_NAME, SD_GUI_AUTHOR, this->ini_path,
                         wxEmptyString, wxCONFIG_USE_LOCAL_FILE);
    this->fileConfig->SetVendorName(SD_GUI_AUTHOR);
    this->fileConfig->SetAppName(PROJECT_NAME);

    wxConfigBase::Set(fileConfig);
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

    this->cfg->lora                 = this->fileConfig->Read("/paths/lora", lora_path).ToStdString();
    this->cfg->model                = this->fileConfig->Read("/paths/model", model_path).ToStdString();
    this->cfg->vae                  = this->fileConfig->Read("/paths/vae", vae_path).ToStdString();
    this->cfg->embedding            = this->fileConfig->Read("/paths/embedding", embedding_path).ToStdString();
    this->cfg->taesd                = this->fileConfig->Read("/paths/taesd", taesd_path).ToStdString();
    this->cfg->esrgan               = this->fileConfig->Read("/paths/esrgan", esrgan_path).ToStdString();
    this->cfg->controlnet           = this->fileConfig->Read("/paths/controlnet", controlnet_path).ToStdString();
    this->cfg->presets              = this->fileConfig->Read("/paths/presets", presets_path).ToStdString();
    this->cfg->jobs                 = this->fileConfig->Read("/paths/jobs", jobs_path).ToStdString();
    this->cfg->thumbs_path          = thumbs_path.ToStdString();
    this->cfg->tmppath              = tmp_path.ToStdString();
    this->cfg->output               = this->fileConfig->Read("/paths/output", imagespath).ToStdString();
    this->cfg->keep_model_in_memory = this->fileConfig->Read("/keep_model_in_memory", this->cfg->keep_model_in_memory);
    this->cfg->save_all_image       = this->fileConfig->Read("/save_all_image", this->cfg->save_all_image);
    this->cfg->n_threads            = this->fileConfig->Read("/n_threads", cores());
    this->cfg->show_notifications   = this->fileConfig->ReadBool("/show_notification", this->cfg->show_notifications);
    this->cfg->notification_timeout = this->fileConfig->Read("/notification_timeout", this->cfg->notification_timeout);
    this->cfg->image_quality        = this->fileConfig->Read("/image_quality", this->cfg->image_quality);

    int idx               = 0;
    auto saved_image_type = this->fileConfig->Read("/image_type", "JPG");

    for (auto type : sd_gui_utils::image_types_str) {
        if (saved_image_type == type) {
            this->cfg->image_type = (sd_gui_utils::imageTypes)idx;
            break;
        }
        idx++;
    }
    // populate data from sd_params as default...

    if (!this->modelLoaded && this->firstCfgInit) {
        this->m_cfg->SetValue(static_cast<double>(this->sd_params->cfg_scale));
        this->m_seed->SetValue(static_cast<int>(this->sd_params->seed));
        this->m_clip_skip->SetValue(this->sd_params->clip_skip);
        this->m_steps->SetValue(this->sd_params->sample_steps);
        this->m_width->SetValue(this->sd_params->width);
        this->m_height->SetValue(this->sd_params->height);
        this->m_batch_count->SetValue(this->sd_params->batch_count);
        this->loadSamplerList();
        this->loadTypeList();
        this->loadShcedulerList();

        // check if directories exists...
        if (!std::filesystem::exists(model_path.ToStdString())) {
            std::filesystem::create_directories(model_path.ToStdString());
        }
        if (!std::filesystem::exists(lora_path.ToStdString())) {
            std::filesystem::create_directories(lora_path.ToStdString());
        }
        if (!std::filesystem::exists(vae_path.ToStdString())) {
            std::filesystem::create_directories(vae_path.ToStdString());
        }
        if (!std::filesystem::exists(embedding_path.ToStdString())) {
            std::filesystem::create_directories(embedding_path.ToStdString());
        }
        if (!std::filesystem::exists(tmp_path.ToStdString())) {
            std::filesystem::create_directories(tmp_path.ToStdString());
        }
        if (!std::filesystem::exists(taesd_path.ToStdString())) {
            std::filesystem::create_directories(taesd_path.ToStdString());
        }
        if (!std::filesystem::exists(esrgan_path.ToStdString())) {
            std::filesystem::create_directories(esrgan_path.ToStdString());
        }
        if (!std::filesystem::exists(presets_path.ToStdString())) {
            std::filesystem::create_directories(presets_path.ToStdString());
        }
        if (!std::filesystem::exists(jobs_path.ToStdString())) {
            std::filesystem::create_directories(jobs_path.ToStdString());
        }
        if (!std::filesystem::exists(controlnet_path.ToStdString())) {
            std::filesystem::create_directories(controlnet_path.ToStdString());
        }
        if (!std::filesystem::exists(imagespath.ToStdString())) {
            std::filesystem::create_directories(imagespath.ToStdString());
        }
        if (!std::filesystem::exists(thumbs_path.ToStdString())) {
            std::filesystem::create_directories(thumbs_path.ToStdString());
        }
    }
    this->firstCfgInit = false;
}

void MainWindowUI::OnExit(wxEvent& event) {
    this->Close();
}

void MainWindowUI::threadedModelHashCalc(
    wxEvtHandler* eventHandler,
    sd_gui_utils::ModelFileInfo* modelinfo) {
    modelinfo->hash_fullsize = modelinfo->size;

    sd_gui_utils::VoidHolder* holder = new sd_gui_utils::VoidHolder;
    holder->p1                       = (void*)eventHandler;
    holder->p2                       = (void*)modelinfo;

    modelinfo->sha256 = sd_gui_utils::sha256_file_openssl(
        modelinfo->path.c_str(), (void*)holder,
        &MainWindowUI::ModelStandaloneHashingCallback);
    MainWindowUI::SendThreadEvent(
        eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_DONE,
        modelinfo);
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
        headers.emplace_back("Authorization: Bearer " + apikey.ToStdString());
    }

    try {
        request.get(url, headers, response);

        modelinfo->civitaiPlainJson = response;
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FINISHED, modelinfo);
        return;
    }

    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
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
        headers.push_back("Authorization: Bearer " + apikey.ToStdString());
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

void MainWindowUI::loadDll(wxDynamicLibrary* dll, const std::string& usingBackend) {
    this->usingBackend        = usingBackend;
    this->sd_dll              = dll;
    this->sd_set_log_callback = (SdSetLogCallbackFunction)this->sd_dll->GetSymbol("sd_set_log_callback");
    if (!this->sd_set_log_callback) {
        wxMessageBox(_("Something went wrong. Invalid backend lib"));
        this->Close();
    }
    this->sd_set_log_callback(MainWindowUI::HandleSDLog, (void*)this->GetEventHandler());

    this->sd_set_progress_callback = (SdSetProgressCallbackFunction)this->sd_dll->GetSymbol("sd_set_progress_callback");
    if (!this->sd_set_progress_callback) {
        wxMessageBox(_("Something went wrong. Invalid backend lib"));
        this->Close();
    }
}

void MainWindowUI::GenerateTxt2img(wxEvtHandler* eventHandler,
                                   QM::QueueItem* myItem) {
    std::lock_guard<std::mutex> lock(this->mutex);

    Txt2ImgFunction txt2img = (Txt2ImgFunction)this->sd_dll->GetSymbol("txt2img");

    if (!txt2img) {
        myItem->status_message =
            wxString(_("Failed to load img2img symbol from backend!"));
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      myItem);
        return;
    }
    FreeSdCtxFunction free_sd_ctx =
        (FreeSdCtxFunction)this->sd_dll->GetSymbol("free_sd_ctx");

    sd_gui_utils::VoidHolder* vparams = new sd_gui_utils::VoidHolder;
    vparams->p1                       = (void*)this->GetEventHandler();
    vparams->p2                       = (void*)myItem;

    this->sd_set_progress_callback(MainWindowUI::HandleSDProgress,
                                   (void*)vparams);

    this->voids.emplace_back(vparams);

    if (!this->modelLoaded) {
        this->txt2img_sd_ctx         = this->LoadModelv2(eventHandler, myItem);
        this->currentModel           = myItem->params.model_path;
        this->currentVaeModel        = myItem->params.vae_path;
        this->currentTaesdModel      = myItem->params.taesd_path;
        this->currentwType           = myItem->params.wtype;
        this->currentControlnetModel = myItem->params.controlnet_path;
    } else {  // the model must be reloaded when: models is changed, vae is
              // changed, taesd is changed
        if (myItem->params.model_path != this->currentModel ||
            this->currentVaeModel != myItem->params.vae_path ||
            this->currentTaesdModel != myItem->params.taesd_path ||
            this->currentwType != myItem->params.wtype ||
            this->currentControlnetModel != myItem->params.controlnet_path ||
            this->lastGenerationMode != myItem->mode) {
            free_sd_ctx(this->txt2img_sd_ctx);
            this->txt2img_sd_ctx = this->LoadModelv2(eventHandler, myItem);
        }
    }

    if (!this->modelLoaded || this->txt2img_sd_ctx == nullptr) {
        myItem->status_message =
            wxString::Format("Model load failed: %s", myItem->model).ToStdString();
        MainWindowUI::SendThreadEvent(eventHandler,
                                      QM::QueueEvents::ITEM_MODEL_FAILED, myItem);
        return;
    }

    auto start = std::chrono::system_clock::now();

    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_GENERATION_STARTED, myItem);
    sd_image_t* control_image = NULL;
    sd_image_t* results;
    // prepare controlnet image, if have
    // if we have, but no model, the myItem will not contains any image
    if (myItem->params.control_image_path.length() > 0) {
        if (std::filesystem::exists(myItem->params.control_image_path)) {
            int c = 0;
            int w, h;
            stbi_uc* input_image_buffer =
                stbi_load(myItem->params.control_image_path.c_str(), &w, &h, &c, 3);
            control_image =
                new sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
            input_image_buffer = NULL;
            delete input_image_buffer;
        }
    }
    /*
      results = txt2img(this->txt2img_sd_ctx, myItem->params.prompt.c_str(),
                        myItem->params.negative_prompt.c_str(),
                        myItem->params.clip_skip, myItem->params.cfg_scale,
                        myItem->params.width, myItem->params.height,
                        myItem->params.sample_method, myItem->params.sample_steps,
                        myItem->params.seed, myItem->params.batch_count,
                        control_image, myItem->params.control_strength);*/
    myItem->started_at = sd_gui_utils::GetCurrentUnixTimestamp();

    results = txt2img(
        this->txt2img_sd_ctx, myItem->params.prompt.c_str(),
        myItem->params.negative_prompt.c_str(), myItem->params.clip_skip,
        myItem->params.cfg_scale, myItem->params.guidance, myItem->params.width,
        myItem->params.height, myItem->params.sample_method,
        myItem->params.sample_steps, myItem->params.seed,
        myItem->params.batch_count, NULL, myItem->params.control_strength,
        myItem->params.style_ratio, myItem->params.normalize_input, "");

    control_image = NULL;
    delete control_image;
    if (results == NULL) {
        myItem->status_message = "Empty results after txt2img!";
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      myItem);
        delete results;
        return;
    }
    /* save image(s) */

    // const auto p1 = std::chrono::system_clock::now();
    //  auto ctime =
    //  std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    for (int i = 0; i < myItem->params.batch_count; i++) {
        if (results[i].data == NULL) {
            continue;
        }
        this->handleSdImage(results[i], myItem, eventHandler);
        free(results[i].data);
        results[i].data = NULL;
    }

    auto end                                      = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    auto msg = wxString::Format("Image generation done in %fs. Saved into %s", elapsed_seconds.count(), this->cfg->output).ToStdString();
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MESSAGE, NULL, msg);
    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FINISHED, myItem);
}

void MainWindowUI::GenerateImg2img(wxEvtHandler* eventHandler,
                                   QM::QueueItem* item) {
    std::lock_guard<std::mutex> lock(this->mutex);

    Img2ImgFunction img2img = (Img2ImgFunction)this->sd_dll->GetSymbol("img2img");

    if (!img2img) {
        item->status_message =
            wxString(_("Failed to load img2img symbol from backend!"));
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      item);
        return;
    }
    FreeSdCtxFunction free_sd_ctx =
        (FreeSdCtxFunction)this->sd_dll->GetSymbol("free_sd_ctx");

    sd_gui_utils::VoidHolder* vparams = new sd_gui_utils::VoidHolder;
    vparams->p1                       = (void*)this->GetEventHandler();
    vparams->p2                       = (void*)item;

    this->sd_set_progress_callback(MainWindowUI::HandleSDProgress,
                                   (void*)vparams);
    this->voids.emplace_back(vparams);

    if (!this->modelLoaded) {
        this->txt2img_sd_ctx         = this->LoadModelv2(eventHandler, item);
        this->currentModel           = item->params.model_path;
        this->currentVaeModel        = item->params.vae_path;
        this->currentTaesdModel      = item->params.taesd_path;
        this->currentwType           = item->params.wtype;
        this->currentControlnetModel = item->params.controlnet_path;
    } else {  // the model must be reloaded when: models is changed, vae is
              // changed, taesd is changed
        if (item->params.model_path != this->currentModel ||
            this->currentVaeModel != item->params.vae_path ||
            this->currentTaesdModel != item->params.taesd_path ||
            this->currentwType != item->params.wtype ||
            this->lastGenerationMode != item->mode) {
            free_sd_ctx(this->txt2img_sd_ctx);
            this->txt2img_sd_ctx = this->LoadModelv2(eventHandler, item);
        }
    }

    if (!this->modelLoaded) {
        item->status_message =
            wxString::Format("Model load failed: %s", item->model).ToStdString();
        MainWindowUI::SendThreadEvent(eventHandler,
                                      QM::QueueEvents::ITEM_MODEL_FAILED, item);
        return;
    }

    auto start = std::chrono::system_clock::now();

    if (!std::filesystem::exists(item->initial_image)) {
        item->status_message =
            wxString::Format("Can not open initial image: %s", item->initial_image)
                .ToStdString();
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      item);
        return;
    }

    int c = 0;
    int w, h;
    stbi_uc* input_image_buffer =
        stbi_load(item->initial_image.c_str(), &w, &h, &c, 3);
    sd_image_t input_image = {(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
    input_image_buffer     = NULL;
    delete input_image_buffer;

    MainWindowUI::SendThreadEvent(eventHandler,
                                  QM::QueueEvents::ITEM_GENERATION_STARTED, item);
    /*sd_image_t *results = img2img(
        this->txt2img_sd_ctx, input_image, item->params.prompt.c_str(),
        item->params.negative_prompt.c_str(), item->params.clip_skip,
        item->params.cfg_scale, item->params.width, item->params.height,
        item->params.sample_method, item->params.sample_steps,
        item->params.strength, item->params.seed, item->params.batch_count);*/

    item->started_at = sd_gui_utils::GetCurrentUnixTimestamp();

    sd_image_t* results = img2img(
        this->txt2img_sd_ctx, input_image, item->params.prompt.c_str(),
        item->params.negative_prompt.c_str(), item->params.clip_skip,
        item->params.cfg_scale, item->params.guidance, item->params.width,
        item->params.height, item->params.sample_method,
        item->params.sample_steps, item->params.strength, item->params.seed,
        item->params.batch_count, NULL, item->params.control_strength,
        item->params.style_ratio, item->params.normalize_input, NULL);

    if (results == NULL) {
        item->status_message = "Empty results after img2img!";
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      item);
        delete results;
        return;
    }

    /* save image(s) */

    // const auto p1 = std::chrono::system_clock::now();
    //  auto ctime =
    //  std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    for (int i = 0; i < item->params.batch_count; i++) {
        if (results[i].data == NULL) {
            continue;
        }

        item = this->handleSdImage(results[i], item, eventHandler);
        free(results[i].data);
        results[i].data = NULL;
    }
    free(results);

    auto end                                      = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    auto msg = wxString::Format("Image generation done in %fs. Saved into %s",
                                elapsed_seconds.count(), this->cfg->output)
                   .ToStdString();
    MainWindowUI::SendThreadEvent(eventHandler,
                                  sd_gui_utils::ThreadEvents::MESSAGE, NULL, msg);
    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FINISHED,
                                  item);
}

void MainWindowUI::GenerateUpscale(wxEvtHandler* eventHandler,
                                   QM::QueueItem* item) {
    std::lock_guard<std::mutex> lock(this->mutex);

    UpscalerFunction upscale =
        (UpscalerFunction)this->sd_dll->GetSymbol("upscale");

    if (!upscale) {
        item->status_message =
            wxString(_("Failed to load upscale symbol from backend!"));
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      item);
        return;
    }
    FreeUpscalerFunction free_upscaler_ctx =
        (FreeUpscalerFunction)this->sd_dll->GetSymbol("free_upscaler_ctx");
    FreeSdCtxFunction free_sd_ctx =
        (FreeSdCtxFunction)this->sd_dll->GetSymbol("free_sd_ctx");

    sd_gui_utils::VoidHolder* vparams = new sd_gui_utils::VoidHolder;
    vparams->p1                       = (void*)this->GetEventHandler();
    vparams->p2                       = (void*)item;

    this->sd_set_progress_callback(MainWindowUI::HandleSDProgress,
                                   (void*)vparams);

    if (this->currentUpscalerModel != item->params.esrgan_path) {
        if (this->upscaleModelLoaded == true) {
            free_upscaler_ctx(this->upscaler_sd_ctx);
            this->upscaleModelLoaded = false;
        }
    }

    if (this->m_keep_other_models_in_memory->GetValue() == false &&
        this->modelLoaded == true) {
        free_sd_ctx(this->txt2img_sd_ctx);
        this->modelLoaded = false;
    }

    if (this->upscaleModelLoaded == false) {
        MainWindowUI::SendThreadEvent(eventHandler,
                                      QM::QueueEvents::ITEM_MODEL_LOAD_START, item);
        this->upscaler_sd_ctx = this->LoadUpscaleModel(eventHandler, item);

        if (!this->upscaleModelLoaded || this->upscaler_sd_ctx == NULL) {
            item->status_message =
                wxString::Format("Model load failed: %s", item->params.esrgan_path)
                    .ToStdString();
            MainWindowUI::SendThreadEvent(eventHandler,
                                          QM::QueueEvents::ITEM_MODEL_FAILED, item);
            this->upscaleModelLoaded = false;
            return;
        }
        this->upscaleModelLoaded = true;
    }

    if (std::filesystem::exists(item->initial_image)) {
        int c = 0;
        int w, h;
        stbi_uc* input_image_buffer =
            stbi_load(item->initial_image.c_str(), &w, &h, &c, 3);
        sd_image_t control_image =
            sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
        input_image_buffer = NULL;
        delete input_image_buffer;

        item->started_at = sd_gui_utils::GetCurrentUnixTimestamp();

        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_GENERATION_STARTED, item);
        sd_image_t upscaled_image = upscale(this->upscaler_sd_ctx, control_image, item->upscale_factor);

        // save img
        wxImage* img = new wxImage(upscaled_image.width, upscaled_image.height,
                                   upscaled_image.data);
        if (!img->IsOk()) {
            item->status_message = "Failed to save image.";
            MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                          item);
            return;
        }

        std::string filename  = this->cfg->output;
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

        std::string filename_without_extension;
        filename =
            filename + wxString(wxFileName::GetPathSeparator()).ToStdString();
        filename = filename + std::to_string(item->id);
        filename = filename + "_";
        filename = filename + std::to_string(item->params.seed);
        filename = filename + "_upscaled";
        filename = filename + "_";
        filename = filename + std::to_string(img->GetWidth()) + "x" + std::to_string(img->GetHeight());

        filename_without_extension = filename;
        filename                   = filename + extension;

        if (img->SaveFile(filename, imgHandler)) {
            item->images.emplace_back(new QM::QueueItemImage{filename});
            MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FINISHED, item);
        } else {
            item->status_message =
                wxString::Format(_("Failed to save image into %s"), filename).ToStdString();
            MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, item);
        }

        // save img
        if (!this->m_keep_upscaler_in_memory->GetValue()) {
            this->upscaleModelLoaded = false;
            free_upscaler_ctx(this->upscaler_sd_ctx);
        }
    } else {
        item->status_message = wxString::Format(_("Can not open initial image: %s"),
                                                item->initial_image)
                                   .ToStdString();
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED,
                                      item);
        return;
    }
}

void MainWindowUI::OnHtmlLinkClicked(wxHtmlLinkEvent& event) {
    wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
}