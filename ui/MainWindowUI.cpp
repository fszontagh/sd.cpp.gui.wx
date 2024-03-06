#include "MainWindowUI.h"

MainWindowUI::MainWindowUI(wxWindow *parent)
    : UI(parent)
{
    this->ini_path = wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator() + "sd.ui.config.ini";
    this->sd_params = new sd_gui_utils::SDParams;
    this->currentInitialImage = new wxImage();
    this->currentInitialImagePreview = new wxImage();
    this->currentUpscalerSourceImage = new wxImage();

    this->currentControlnetImage = new wxImage();
    this->currentControlnetImagePreview = new wxImage();

    this->previewImageList = new wxImageList();
    this->modelPreviewImageList = new wxImageList();
    // this->defaultTextCtrlStyle =
    this->m_prompt->GetStyle(0, defaultTextCtrlStyle);

    this->m_joblist->AppendTextColumn("Id");
    this->m_joblist->AppendTextColumn("Created at");
    this->m_joblist->AppendTextColumn("Type");
    this->m_joblist->AppendTextColumn("Model");
    this->m_joblist->AppendTextColumn("Sampler");
    this->m_joblist->AppendTextColumn("Seed");
    this->m_joblist->AppendProgressColumn("Progress"); // progressbar
    this->m_joblist->AppendTextColumn("Speed");        // speed
    this->m_joblist->AppendTextColumn("Status");       // status

    this->m_joblist->GetColumn(0)->SetHidden(true);
    this->m_joblist->GetColumn(1)->SetSortable(true);
    this->m_joblist->GetColumn(1)->SetSortOrder(false);

    this->SetTitle(this->GetTitle() + " - " + SD_GUI_VERSION);
    this->TaskBar = new wxTaskBarIcon();

    // this->TaskBarMenu = new wxMenu();

    auto bitmap = app_png_to_wx_bitmap();
    TaskBar->SetIcon(bitmap, this->GetTitle());

    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    this->SetIcon(icon);

    this->cfg = new sd_gui_utils::config;

    this->initConfig();

    wxPersistentRegisterAndRestore(this, this->GetName());

    this->qmanager = new QM::QueueManager(this->GetEventHandler(), this->cfg->jobs);
    this->ModelManager = new ModelInfo::Manager(this->cfg->datapath);

    // set SD logger
    sd_set_log_callback(MainWindowUI::HandleSDLog, (void *)this->GetEventHandler());

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

    Bind(wxEVT_THREAD, &MainWindowUI::OnThreadMessage, this);
    const char *system_info = sd_get_system_info();
    this->logs->AppendText(system_info);
}

void MainWindowUI::onSettings(wxCommandEvent &event)
{
    auto bitmap = app_png_to_wx_bitmap();
    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    this->settingsWindow = new MainWindowSettings(this);
    this->settingsWindow->SetIcon(icon);
    this->settingsWindow->Bind(wxEVT_CLOSE_WINDOW, &MainWindowUI::OnCloseSettings, this);
    this->settingsWindow->Show();
}

void MainWindowUI::onModelsRefresh(wxCommandEvent &event)
{
    this->loadModelList();
    this->loadVaeList();
    this->loadTaesdList();
    this->loadControlnetList();
    this->loadEsrganList();
}

void MainWindowUI::onModelSelect(wxCommandEvent &event)
{
    // check if really selected a model, or just the first element, which is always exists...
    if (this->m_model->GetSelection() == 0)
    {
        this->m_generate1->Disable();
        this->m_generate2->Disable();
        this->m_statusBar166->SetStatusText("Model: none");
        return;
    }
    auto name = this->m_model->GetStringSelection().ToStdString();
    // img2img has an image
    if (!this->m_open_image->GetPath().empty())
    {
        this->m_generate1->Enable();
    }
    this->m_generate2->Enable();
    this->sd_params->model_path = this->ModelFiles.at(name);
    this->m_statusBar166->SetStatusText("Model: " + this->sd_params->model_path);
}

void MainWindowUI::onTypeSelect(wxCommandEvent &event)
{
    wxChoice *c = (wxChoice *)event.GetEventObject();
    if (c->GetStringSelection() == "Q4_2 - not supported" || c->GetStringSelection() == "Q4_5 - not supported")
    {
        c->SetSelection(sizeof(sd_gui_utils::sd_type_names[0]) - 1);
    }
}

void MainWindowUI::onVaeSelect(wxCommandEvent &event)
{
    // TODO: Implement onVaeSelect
}

void MainWindowUI::onRandomGenerateButton(wxCommandEvent &event)
{
    this->m_seed->SetValue(sd_gui_utils::generateRandomInt(100000000, 999999999));
}

void MainWindowUI::onResolutionSwap(wxCommandEvent &event)
{
    auto oldW = this->m_width->GetValue();
    auto oldH = this->m_height->GetValue();

    this->m_height->SetValue(oldW);
    this->m_width->SetValue(oldH);
}

void MainWindowUI::m_notebook1302OnNotebookPageChanged(wxNotebookEvent &event)
{
    event.Skip();
    int selected = event.GetSelection();
    if (selected == wxNOT_FOUND)
    {
        return;
    }
    // int prev_selected = event.GetOldSelection();
    //  0 -> jobs and images
    //  1 -> txt2img
    //  2 -> img2img
    //  3 -> upscaler
    //  4 -> models
    if (selected == 2) // on img2img and img2vid the vade_decode_only is false, otherwise true
    {
        this->m_vae_decode_only->SetValue(false);
    }
    else
    {
        this->m_vae_decode_only->SetValue(true);
    }
    if (selected == 0 || selected == 1 || selected == 2 || selected == 4)
    {
        if (this->m_model->GetCount() > 1)
        {
            this->m_model->Enable();
        }

        this->m_type->Enable();

        if (this->m_vae->GetCount() > 1)
        {
            this->m_vae->Enable();
        }
        this->m_vae_tiling->Enable();
        this->m_cfg->Enable();
        this->m_clip_skip->Enable();

        if (this->m_taesd->GetCount() > 0)
        {
            this->m_taesd->Enable();
        }

        this->m_batch_count->Enable();
        this->m_sampler->Enable();

        this->m_preset_list->Enable();
        this->m_save_preset->Enable();

        this->m_seed->Enable();
        this->m_steps->Enable();

        if (selected == 1 && this->m_controlnetImageOpen->GetPath().length() > 0)
        {
            this->m_width->Disable();
            this->m_height->Disable();
        }
        else
        {
            this->m_width->Enable();
            this->m_height->Enable();
        }
        // img2img
        if (selected == 2 && !this->m_open_image->GetPath().empty())
        {
            this->m_width->Disable();
            this->m_height->Disable();
        }
    }
    // upscaler
    if (selected == 3)
    {
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
    }
}

void MainWindowUI::onJobsStart(wxCommandEvent &event)
{
    // TODO: Implement onJobsStart
}

void MainWindowUI::onJobsPause(wxCommandEvent &event)
{
    // TODO: Implement onJobsPause
    this->qmanager->PauseAll();
}

void MainWindowUI::onJobsDelete(wxCommandEvent &event)
{
    // TODO: Implement onJobsDelete
    this->qmanager->RestartQueue();
}

void MainWindowUI::OnJobListItemActivated(wxDataViewEvent &event)
{
    // implemented in OnJobListItemSelection
}

void MainWindowUI::onContextMenu(wxDataViewEvent &event)
{

    auto column = event.GetColumn();
    if (column == -1)
    {
        return;
    }

    auto *source = (wxDataViewListCtrl *)event.GetEventObject();
    wxMenu *menu = new wxMenu();

    menu->SetClientData((void *)source);

    if (source == this->m_joblist)
    {
        auto item = event.GetItem();

        wxDataViewListStore *store = this->m_joblist->GetStore();
        QM::QueueItem *_qitem = reinterpret_cast<QM::QueueItem *>(store->GetItemData(item));
        QM::QueueItem *qitem = new QM::QueueItem(this->qmanager->GetItem(_qitem->id));

        menu->Append(1, "Requeue");
        if (qitem->mode == QM::GenerationMode::IMG2IMG || qitem->mode == QM::GenerationMode::UPSCALE)
        {
            if (!std::filesystem::exists(qitem->initial_image))
            {
                menu->Enable(1, false);
            }
        }
        if (qitem->params.controlnet_path.length() > 0 && qitem->params.control_image_path.length() > 0 && !std::filesystem::exists(qitem->params.control_image_path))
        {
            menu->Enable(1, false);
        }

        if (qitem->mode != QM::GenerationMode::UPSCALE && qitem->mode != QM::GenerationMode::CONVERT)
        {
            menu->Append(2, wxString::Format("Copy to text2img %d", qitem->id));
            menu->Append(3, wxString::Format("Copy prompts to text2img %d", qitem->id));
            menu->Append(4, wxString::Format("Copy prompts to img2img %d", qitem->id));
            menu->Append(5, wxString::Format("Select model %s", qitem->model));

            if (qitem->images.size() > 0)
            {
                menu->Append(6, wxString::Format("Send last image to the Upscale tab"));
                menu->Append(7, wxString::Format("Send last image to the img2img tab"));
            }
        }
        if (qitem->mode == QM::GenerationMode::UPSCALE)
        {
            if (qitem->images.size() > 0)
            {
                menu->Append(6, wxString::Format("Send last upscaled image to Upscale tab"));
            }
        }

        menu->AppendSeparator();
        menu->Append(99, "Delete");
        if (this->qmanager->GetItem(qitem->id).status == QM::QueueStatus::RUNNING || this->qmanager->GetItem(qitem->id).status == QM::QueueStatus::HASHING)
        {
            menu->Enable(1, false);
            menu->Enable(99, false);
        }
    }

    if (source == this->m_data_model_list)
    {
        // auto item = event.GetItem();
        auto item = this->m_data_model_list->GetCurrentItem();

        wxDataViewListStore *store = this->m_data_model_list->GetStore();
        sd_gui_utils::ModelFileInfo *modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(store->GetItemData(item));

        if (!modelinfo->sha256.empty())
        {
            menu->Append(100, "RE-Calculate Hash");
        }
        else
        {
            menu->Append(100, "Calculate Hash");
        }
        if (!modelinfo->civitaiPlainJson.empty() && modelinfo->hash_progress_size == 0)
        {
            menu->Append(105, "Force update info from CivitAi");
        }

        if (modelinfo->model_type == sd_gui_utils::DirTypes::CHECKPOINT)
        {
            menu->Append(200, wxString::Format("Select model %s to the next job", modelinfo->name));
        }
        if (modelinfo->model_type == sd_gui_utils::DirTypes::LORA)
        {
            menu->Append(101, wxString::Format("Append to text2img prompt <lora:%s:0.5>", modelinfo->name));
            menu->Append(102, wxString::Format("Append to text2img neg. prompt <lora:%s:0.5>", modelinfo->name));
            menu->Append(103, wxString::Format("Append to img2img prompt <lora:%s:0.5>", modelinfo->name));
            menu->Append(104, wxString::Format("Append to img2img neg. prompt <lora:%s:0.5>", modelinfo->name));
        }
        if (modelinfo->model_type == sd_gui_utils::DirTypes::EMBEDDING)
        {
            menu->Append(111, wxString::Format("Append to text2img prompt %s", modelinfo->name));
            menu->Append(112, wxString::Format("Append to text2img neg. prompt %s", modelinfo->name));
            menu->Append(113, wxString::Format("Append to img2img prompt %s", modelinfo->name));
            menu->Append(114, wxString::Format("Append to img2img neg. prompt %s", modelinfo->name));
        }
    }
    menu->Bind(wxEVT_COMMAND_MENU_SELECTED, &MainWindowUI::OnPopupClick, this);
    PopupMenu(menu);
}

void MainWindowUI::OnJobListItemSelection(wxDataViewEvent &event)
{
    auto store = this->m_joblist->GetStore();

    auto row = this->m_joblist->GetSelectedRow();
    auto currentItem = store->GetItem(row);
    QM::QueueItem *qitem = reinterpret_cast<QM::QueueItem *>(store->GetItemData(currentItem));

    auto item = this->qmanager->GetItem(qitem->id);

    if (item.status == QM::QueueStatus::RUNNING ||
        item.status == QM::QueueStatus::PENDING ||
        item.status == QM::QueueStatus::HASHING)
    {
        this->m_joblist_item_details->DeleteAllItems();
        this->previewImageList->Destroy();
        this->previewImageList->Create(256, 256);
        this->m_job_details_imagelist->ClearAll();
        return;
    }

    std::cerr << "Selected job: " << qitem->id << std::endl;
    this->m_joblist_item_details->DeleteAllItems();

    wxVector<wxVariant> data;

    data.push_back(wxVariant("ID"));
    data.push_back(wxVariant(wxString::Format("%d", item.id)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("Created at"));
    data.push_back(wxVariant(sd_gui_utils::formatUnixTimestampToDate(item.created_at)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("Finished at"));
    data.push_back(wxVariant(sd_gui_utils::formatUnixTimestampToDate(item.finished_at)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("Mode"));
    data.push_back(wxVariant(wxString(sd_gui_utils::modes_str[item.mode])));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("Model"));
    data.push_back(wxVariant(wxString(item.model)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("Threads"));
    data.push_back(wxVariant(wxString::Format("%d", item.params.n_threads)));
    this->m_joblist_item_details->AppendItem(data);
    data.clear();

    if (item.mode == QM::GenerationMode::UPSCALE)
    {
        data.push_back(wxVariant("Factor"));
        data.push_back(wxVariant(wxString::Format("%" PRId32, item.upscale_factor)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    if (item.mode == QM::GenerationMode::IMG2IMG || item.mode == QM::GenerationMode::TXT2IMG)
    {
        data.push_back(wxVariant("Type"));
        data.push_back(wxVariant(wxString::Format("%s", sd_gui_utils::sd_type_gui_names[item.params.wtype])));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }

    if (item.mode == QM::GenerationMode::IMG2IMG)
    {
        data.push_back(wxVariant("Init image"));
        data.push_back(wxVariant(wxString(item.initial_image)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }
    if (item.mode == QM::GenerationMode::UPSCALE)
    {
    }
    if (item.mode == QM::GenerationMode::TXT2IMG || item.mode == QM::GenerationMode::IMG2IMG)
    {
        data.push_back(wxVariant("Prompt"));
        data.push_back(wxVariant(wxString(item.params.prompt)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("Neg. prompt"));
        data.push_back(wxVariant(wxString(item.params.negative_prompt)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("Seed"));
        data.push_back(wxVariant(wxString::Format("%" PRId64, item.params.seed)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("Clip skip"));
        data.push_back(wxVariant(wxString::Format("%d", item.params.clip_skip)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("Cfg scale"));
        data.push_back(wxVariant(wxString::Format("%.1f", item.params.cfg_scale)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("Sampler"));
        data.push_back(wxVariant(wxString::Format("%s", sd_gui_utils::sample_method_str[item.params.sample_method])));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        if (item.mode == QM::GenerationMode::IMG2IMG)
        {
            data.push_back(wxVariant("Strength"));
            data.push_back(wxVariant(wxString::Format("%.2f", item.params.strength)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }

        data.push_back(wxVariant("Steps"));
        data.push_back(wxVariant(wxString::Format("%d", item.params.sample_steps)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("VAE"));
        data.push_back(wxVariant(wxString(item.params.vae_path)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("VAE tiling"));
        data.push_back(wxVariant(wxString(item.params.vae_tiling == true ? _("yes") : _("no"))));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("TAESD"));
        data.push_back(wxVariant(wxString(item.params.taesd_path)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();
    }
    if (item.mode == QM::GenerationMode::TXT2IMG)
    {
        data.push_back(wxVariant("Width"));
        data.push_back(wxVariant(wxString::Format("%dpx", item.params.width)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("Height"));
        data.push_back(wxVariant(wxString::Format("%dpx", item.params.height)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant("Batch count"));
        data.push_back(wxVariant(wxString::Format("%d", item.params.batch_count)));
        this->m_joblist_item_details->AppendItem(data);
        data.clear();

        if (!item.params.controlnet_path.empty())
        {
            data.push_back(wxVariant("CN model"));
            data.push_back(wxVariant(wxString(item.params.controlnet_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant("CN img"));
            data.push_back(wxVariant(wxString(item.params.control_image_path)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();

            data.push_back(wxVariant("CN strength"));
            data.push_back(wxVariant(wxString::Format("%.2f", item.params.control_strength)));
            this->m_joblist_item_details->AppendItem(data);
            data.clear();
        }
    }
    this->previewImageList->Destroy();
    this->previewImageList->Create(256, 256);

    this->m_job_details_imagelist->ClearAll();

    if (!item.initial_image.empty())
    {
        item.images.insert(item.images.begin(), item.initial_image);
    }
    if (!item.params.control_image_path.empty())
    {
        item.images.insert(item.images.begin(), item.params.control_image_path);
    }

    // add imgs
    for (auto img : item.images)
    {
        if (std::filesystem::exists(img))
        {
            wxImage _img;
            if (_img.LoadFile(img))
            {
                auto resized = sd_gui_utils::cropResizeImage(_img, 256, 256, wxColour(*wxBLACK));
                wxBitmap bmap(resized);
                this->previewImageList->Add(bmap);
            }
        }
    }

    this->m_job_details_imagelist->SetImageList(this->previewImageList, wxIMAGE_LIST_NORMAL);
    int idx = 0;
    for (auto img : item.images)
    {
        std::filesystem::path *image_path = new std::filesystem::path(img);
        if (std::filesystem::exists(*image_path))
        {
            auto id = this->m_job_details_imagelist->InsertItem(idx, image_path->filename().string(), idx);
            this->m_job_details_imagelist->SetItemPtrData(id, wxUIntPtr(image_path));
            idx++;
        }
        else
        {
            auto id = this->m_job_details_imagelist->InsertItem(idx, wxString::Format("Deleted: %s", image_path->filename().string()));
            this->m_job_details_imagelist->SetItemPtrData(id, wxUIntPtr(image_path));
            // this->m_job_details_imagelist->SetItemBackgroundColour(id, wxColour(255,255,255));// light yellow, image not found
            this->m_job_details_imagelist->SetItemTextColour(id, wxColour(247, 88, 35)); // near orange, but red
        }
    }
}

void MainWindowUI::OnJobDetailsImagelistItemActivated(wxListEvent &event)
{
    //
    auto item = event.GetItem();
    std::filesystem::path *p = reinterpret_cast<std::filesystem::path *>(item.GetData());
    if (std::filesystem::exists(*p))
    {
        wxLaunchDefaultApplication(p->string());
    }
}

void MainWindowUI::onTxt2ImgFileDrop(wxDropFilesEvent &event)
{
    // only just one file.. sry...
    auto files = event.GetFiles();
    auto file = files[0];
    auto path = std::filesystem::path(file.ToStdString());
    // check if file extension is img
    if (path.extension() != ".jpg" && path.extension() != ".png")
    {
        std::cerr << "Not jpg or png: " << path.extension() << std::endl;
        return;
    }

    wxImage img;
    if (!img.LoadFile(path.string()))
    {
        std::cerr << "Invalid image " << file << std::endl;
        // invalid img
        return;
    }

    // only jpeg supported now
    if (img.GetType() != wxBITMAP_TYPE_JPEG)
    {
        std::cerr << "Not jpg: " << file << std::endl;
        return;
    }

    /// get meta
    try
    {

        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(file.ToStdString());
        if (!image->good())
        {
            return;
        }
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        if (!exifData.empty())
        {
            std::string ex;
            Exiv2::ExifData::iterator it;
            std::string usercomment;
            for (it = exifData.begin(); it != exifData.end(); ++it)
            {
                // ex += fmt::format("Key: {} ", it->key());
                if (it->key() == "Exif.Photo.UserComment")
                {
                    usercomment = it->getValue()->toString();
                    if (!usercomment.empty())
                    {
                        std::map<std::string, std::string> getParams = sd_gui_utils::parseExifPrompts(usercomment);
                        this->imageCommentToGuiParams(getParams, sd_gui_utils::SDMode::TXT2IMG);
                        for (auto _s : getParams)
                        {
                            std::cerr << "key: " << _s.first << " val: " << _s.second << "\n";
                        }
                        break;
                    }
                }
            }
        }
    }
    catch (Exiv2::Error &e)
    {
        std::cerr << "Err: " << e.what() << std::endl;
    }
}

/// TODO: store embeddings like checkpoints and loras, the finetune this
void MainWindowUI::OnPromptText(wxCommandEvent &event)
{

    wxTextCtrl *textCtrl = static_cast<wxTextCtrl *>(event.GetEventObject());

    // Szöveg megszerzése a szövegmezőből
    wxString text = textCtrl->GetValue();
    // Az egyező szavakhoz tartozó stílusok tárolása
    std::vector<sd_gui_utils::stringformat> formats;

    std::vector<std::string> words;
    wxStringTokenizer tokenizer(text, " \r\n,.", wxTOKEN_DEFAULT);
    while (tokenizer.HasMoreTokens())
    {
        std::string word = tokenizer.GetNextToken().ToStdString();
        sd_gui_utils::stringformat _f;
        _f.textAttr = this->defaultTextCtrlStyle;
        _f.end = tokenizer.GetPosition();
        _f.start = _f.end - word.size();
        _f.string = word;
        formats.emplace_back(_f);
    }
    auto modellist = this->ModelManager->getList();
    // Formázás alkalmazása az egyező szavakra
    for (const auto &format : formats)
    {
        if (format.string.length() < 4)
        {
            continue;
        }

        auto it = std::find_if(modellist.begin(), modellist.end(), [&format](const sd_gui_utils::ModelFileInfo &fileInfo)
                               { return fileInfo.name == format.string; });

        if (it != modellist.end())
        {
            wxTextAttr needStyle = wxTextAttr(wxNullColour, wxColour(*wxLIGHT_GREY)), neededStyle = wxTextAttr(wxNullColour, wxColour(*wxLIGHT_GREY));
            if (textCtrl->GetStyle(format.start + 1, needStyle))
            {
                if (needStyle.GetTextColour() != neededStyle.GetTextColour() || needStyle.GetBackgroundColour() != neededStyle.GetBackgroundColour())
                {
                    textCtrl->SetStyle(format.start, format.end, neededStyle);
                    textCtrl->SetDefaultStyle(this->defaultTextCtrlStyle);
                }
            }
        }
    }

    event.Skip();
}

void MainWindowUI::OnNegPromptText(wxCommandEvent &event)
{
    wxTextCtrl *ctrl = static_cast<wxTextCtrl *>(event.GetEventObject());
    ctrl->SetDefaultStyle(this->defaultTextCtrlStyle);

    std::string s;
    std::stringstream ss(ctrl->GetValue().ToStdString());
    std::vector<std::string> v;

    long curr_start_pos = 0;
    long curr_end_pos = curr_start_pos + 1;

    while (getline(ss, s, ' '))
    {
        v.push_back(s);
    }

    for (int i = 0; i < v.size(); i++)
    {
        curr_end_pos = curr_start_pos + v[i].length();
        // check if it is a embedding
        auto safetensor = std::filesystem::path(this->cfg->embedding + "/" + v[i] + ".safetensors");
        auto pt = std::filesystem::path(this->cfg->embedding + "/" + v[i] + ".pt");
        if (std::filesystem::exists(safetensor) || std::filesystem::exists(pt))
        {
            ctrl->SetStyle(curr_start_pos, curr_end_pos, wxTextAttr(*wxGREEN, wxNullColour, *wxITALIC_FONT));
        }
        //   if (v[i].substr(0, 6) == "<lora:")
        //  {
        //      ctrl->SetStyle(curr_start_pos, curr_end_pos, wxTextAttr(*wxRED, wxNullColour, *wxITALIC_FONT));
        //  }
        curr_start_pos = curr_start_pos + v[i].length() + 1;
    }
}

void MainWindowUI::onGenerate(wxCommandEvent &event)
{
    this->initConfig();
    auto type = QM::GenerationMode::TXT2IMG;
    int pageId = this->m_notebook1302->GetSelection();
    switch (pageId)
    {
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

    if (type == QM::GenerationMode::UPSCALE)
    {

        QM::QueueItem item;
        item.params = *this->sd_params;
        item.model = this->m_upscaler_model->GetStringSelection().ToStdString();
        item.mode = type;
        item.params.esrgan_path = this->EsrganFiles.at(this->m_upscaler_model->GetStringSelection().ToStdString());
        item.initial_image = this->m_upscaler_filepicker->GetPath();
        item.params.mode = sd_gui_utils::SDMode::MODE_COUNT;
        item.params.n_threads = this->cfg->n_threads;
        this->qmanager->AddItem(item, false);
        return;
    }

    // prepare params
    // this->sd_params->model_path = this->ModelFiles.at(this->m_model->GetStringSelection().ToStdString());
    auto mindex = this->m_model->GetCurrentSelection();
    sd_gui_utils::ModelFileInfo *modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(this->m_model->GetClientData(mindex));
    this->sd_params->model_path = modelinfo->path;

    if (this->m_taesd->GetCurrentSelection() > 0)
    {
        this->sd_params->taesd_path = this->TaesdFiles.at(this->m_taesd->GetStringSelection().ToStdString());
    }
    else
    {
        this->sd_params->taesd_path = "";
    }
    if (this->m_vae->GetCurrentSelection() > 0)
    {
        this->sd_params->vae_path = this->VaeFiles.at(this->m_vae->GetStringSelection().ToStdString());
    }
    else
    {
        this->sd_params->vae_path = "";
    }

    this->sd_params->lora_model_dir = this->cfg->lora;
    this->sd_params->embeddings_path = this->cfg->embedding;
    this->sd_params->n_threads = this->cfg->n_threads;

    if (type == QM::GenerationMode::TXT2IMG)
    {
        this->sd_params->prompt = this->m_prompt->GetValue().ToStdString();
        this->sd_params->negative_prompt = this->m_neg_prompt->GetValue().ToStdString();
    }
    if (type == QM::GenerationMode::IMG2IMG)
    {
        this->sd_params->prompt = this->m_prompt2->GetValue().ToStdString();
        this->sd_params->negative_prompt = this->m_neg_prompt2->GetValue().ToStdString();
    }
    this->sd_params->cfg_scale = static_cast<float>(this->m_cfg->GetValue());
    this->sd_params->seed = this->m_seed->GetValue();
    this->sd_params->clip_skip = this->m_clip_skip->GetValue();
    this->sd_params->sample_steps = this->m_steps->GetValue();

    if (type == QM::GenerationMode::TXT2IMG)
    {
        this->sd_params->control_strength = this->m_controlnetStrength->GetValue();
    }

    if (type == QM::GenerationMode::IMG2IMG)
    {
        this->sd_params->strength = this->m_strength->GetValue();
    }

    if (this->m_controlnetModels->GetCurrentSelection() > 0 && type == QM::GenerationMode::TXT2IMG)
    {
        this->sd_params->controlnet_path = this->ControlnetModels.at(this->m_controlnetModels->GetStringSelection().ToStdString());
        // do not set the control image if we have no model
        if (this->m_controlnetImageOpen->GetPath().length() > 0)
        {
            this->sd_params->control_image_path = this->m_controlnetImageOpen->GetPath().ToStdString();
        }
    }
    else
    {
        this->sd_params->controlnet_path = "";
        this->sd_params->control_image_path = "";
    }

    this->sd_params->sample_method = (sample_method_t)this->m_sampler->GetCurrentSelection();

    if (this->m_type->GetCurrentSelection() != 0)
    {
        auto selected = this->m_type->GetStringSelection();
        for (auto types : sd_gui_utils::sd_type_gui_names)
        {
            if (types.second == selected)
            {
                this->sd_params->wtype = (sd_type_t)types.first;
            }
        }
    }
    else
    {
        this->sd_params->wtype = sd_type_t::SD_TYPE_COUNT;
    }

    this->sd_params->batch_count = this->m_batch_count->GetValue();

    this->sd_params->width = this->m_width->GetValue();
    this->sd_params->height = this->m_height->GetValue();

    // this->sd_params->vae_tiling
    this->sd_params->vae_tiling = this->m_vae_tiling->GetValue();
    // at img2img is false
    // this->sd_params->vae_decode_only = this->m_vae_decode_only->GetValue();

    QM::QueueItem item;
    item.params = *this->sd_params;
    item.model = this->m_model->GetStringSelection().ToStdString();
    item.mode = type;
    if (type == QM::GenerationMode::IMG2IMG)
    {
        item.initial_image = this->currentInitialImagePath;
    }

    if (item.params.seed == -1)
    {
        item.params.seed = sd_gui_utils::generateRandomInt(this->m_seed->GetMin(), this->m_seed->GetMax());
        this->m_seed->SetValue(item.params.seed);
    }

    // add the queue item
    auto id = this->qmanager->AddItem(item);
}

void MainWindowUI::OnControlnetImageOpen(wxFileDirPickerEvent &event)
{
    if (event.GetPath().empty())
    {
        return;
    }
    wxImage img;
    if (img.LoadFile(event.GetPath()))
    {

        auto origSize = this->m_controlnetImagePreview->GetSize();
        this->ControlnetOrigPreviewBitmap = this->m_controlnetImagePreview->GetBitmap();
        auto preview = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(), origSize.GetHeight());

        this->currentControlnetImage = new wxImage(img);
        this->currentControlnetImagePreview = new wxImage(preview);

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
        this->m_button7->Disable(); // swap resolution
    }
}

void MainWindowUI::OnControlnetImagePreviewButton(wxCommandEvent &event)
{
    MainWindowImageDialog *dialog = new MainWindowImageDialog(this);
    wxImage img(*this->currentControlnetImage);
    auto size = img.GetSize();

    dialog->SetSize(size.GetWidth() + 100, size.GetHeight() + 100);
    wxString title = wxString::Format("Controlnet Image %dx%dpx", size.GetWidth(), size.GetHeight());
    dialog->SetTitle(title);
    dialog->m_bitmap->SetBitmap(img);
    dialog->ShowModal();
}

void MainWindowUI::OnControlnetImageDelete(wxCommandEvent &event)
{
    this->m_controlnetImagePreviewButton->Disable();
    this->m_controlnetImageDelete->Disable();
    this->m_controlnetImageOpen->SetPath("");
    this->currentControlnetImage = NULL;
    this->currentControlnetImagePreview = NULL;
    auto origSize = this->m_controlnetImagePreview->GetSize();
    this->m_controlnetImagePreview->SetBitmap(this->ControlnetOrigPreviewBitmap);
    this->m_controlnetImagePreview->SetSize(origSize);
    this->m_width->Enable();
    this->m_height->Enable();
    this->m_button7->Enable(); // swap resolution
    this->m_controlnetModels->Select(0);
}

void MainWindowUI::Onimg2imgDropFile(wxDropFilesEvent &event)
{
    // only just one file.. sry...
    auto files = event.GetFiles();
    auto file = files[0];

    this->onimg2ImgImageOpen(file.ToStdString());
}

void MainWindowUI::OnImageOpenFileChanged(wxFileDirPickerEvent &event)
{

    if (event.GetPath().empty())
    {
        this->m_generate1->Disable();
        return;
    }
    this->onimg2ImgImageOpen(event.GetPath().ToStdString());
}

void MainWindowUI::OnImg2ImgPreviewButton(wxCommandEvent &event)
{
    MainWindowImageDialog *dialog = new MainWindowImageDialog(this);
    wxImage img(*this->currentInitialImage);
    auto size = img.GetSize();

    dialog->SetSize(size.GetWidth() + 100, size.GetHeight() + 100);
    dialog->SetTitle("IMG2IMG - original image");
    dialog->m_bitmap->SetBitmap(img);
    dialog->ShowModal();
}

void MainWindowUI::OnDeleteInitialImage(wxCommandEvent &event)
{
    this->currentInitialImage = NULL;
    this->currentInitialImagePreview = NULL;
    this->currentInitialImagePath = "";
    auto origSize = this->m_img2img_preview->GetSize();
    this->m_img2img_preview->SetBitmap(this->AppOrigPlaceHolderBitmap);
    this->m_img2img_preview->SetSize(origSize);
    this->m_img2im_preview_img->Disable();
    this->m_delete_initial_img->Disable();
    this->m_open_image->SetPath("");
    this->m_generate2->Disable();
}

void MainWindowUI::OnUpscalerDropFile(wxDropFilesEvent &event)
{
    // only just one file.. sry...
    auto files = event.GetFiles();
    auto file = files[0];
    this->onUpscaleImageOpen(file.ToStdString());
}

void MainWindowUI::OnImageOpenFilePickerChanged(wxFileDirPickerEvent &event)
{
    if (event.GetPath().empty())
    {
        this->m_generate2->Disable();
        return;
    }
    this->onUpscaleImageOpen(event.GetPath().ToStdString());
}

void MainWindowUI::OnDeleteUpscaleImage(wxCommandEvent &event)
{
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

void MainWindowUI::OnUpscalerModelSelection(wxCommandEvent &event)
{
    if (this->m_upscaler_model->GetCurrentSelection() != 0)
    {
        if (!this->m_upscaler_filepicker->GetPath().empty())
        {
            this->m_generate_upscaler->Enable();
            return;
        }
    }
    this->m_generate_upscaler->Disable();
}

void MainWindowUI::OnUpscalerFactorChange(wxSpinEvent &event)
{

    if (this->currentUpscalerSourceImage->IsOk())
    {
        this->m_static_upscaler_height->SetLabel(wxString::Format("%dpx", this->currentUpscalerSourceImage->GetHeight()));
        this->m_static_upscaler_width->SetLabel(wxString::Format("%dpx", this->currentUpscalerSourceImage->GetWidth()));

        int factor = this->m_upscaler_factor->GetValue();
        int target_width = factor * this->currentUpscalerSourceImage->GetWidth();
        int target_height = factor * this->currentUpscalerSourceImage->GetHeight();
        this->m_static_upscaler_target_height->SetLabel(wxString::Format("%dpx", target_height));
        this->m_static_upscaler_target_width->SetLabel(wxString::Format("%dpx", target_width));
    }
}

void MainWindowUI::OnCheckboxLoraFilter(wxCommandEvent &event)
{
    auto value = this->m_modellist_filter->GetValue().ToStdString();
    this->refreshModelTable(value);
}

void MainWindowUI::OnCheckboxCheckpointFilter(wxCommandEvent &event)
{
    auto value = this->m_modellist_filter->GetValue().ToStdString();
    this->refreshModelTable(value);
}

void MainWindowUI::OnModellistFilterKeyUp(wxKeyEvent &event)
{
    auto value = this->m_modellist_filter->GetValue().ToStdString();
    this->refreshModelTable(value);
}

void MainWindowUI::OnDataModelActivated(wxDataViewEvent &event)
{
}

void MainWindowUI::OnDataModelSelected(wxDataViewEvent &event)
{
    auto store = this->m_data_model_list->GetStore();
    auto row = this->m_data_model_list->GetSelectedRow();
    auto currentItem = store->GetItem(row);
    sd_gui_utils::ModelFileInfo *_item = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(store->GetItemData(currentItem));
    sd_gui_utils::ModelFileInfo *modelinfo = this->ModelManager->getIntoPtr(_item->path);

    // download infos only when empty and sha256 is present
    if (modelinfo->civitaiPlainJson.empty() && !modelinfo->sha256.empty())
    {
        this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
    }
    this->UpdateModelInfoDetailsFromModelList(modelinfo);
}

void MainWindowUI::onSamplerSelect(wxCommandEvent &event)
{
    this->sd_params->sample_method = (sample_method_t)this->m_sampler->GetSelection();
}

void MainWindowUI::onSavePreset(wxCommandEvent &event)
{
    wxTextEntryDialog dlg(this, "Please specify a name (only alphanumeric)");
    dlg.SetTextValidator(wxFILTER_ALPHA | wxFILTER_DIGITS);
    if (dlg.ShowModal() == wxID_OK)
    {
        sd_gui_utils::generator_preset preset;

        wxString preset_name = dlg.GetValue();
        preset.cfg = this->m_cfg->GetValue();
        // do not save the seed
        // preset.seed = this->m_seed->GetValue();
        preset.clip_skip = this->m_clip_skip->GetValue();
        preset.steps = this->m_steps->GetValue();
        preset.width = this->m_width->GetValue();
        preset.height = this->m_height->GetValue();
        preset.sampler = (sample_method_t)this->m_sampler->GetSelection();
        preset.batch = this->m_batch_count->GetValue();
        preset.name = preset_name.ToStdString();
        preset.mode = "text2image";
        nlohmann::json j(preset);
        std::string presetfile = fmt::format("{}{}{}.json",
                                             this->cfg->presets,
                                             wxString(wxFileName::GetPathSeparator()).ToStdString(),
                                             preset.name);

        std::ofstream file(presetfile);
        file << j;
        file.close();
        this->LoadPresets();
    }
}

void MainWindowUI::onLoadPreset(wxCommandEvent &event)
{
    auto selected = this->m_preset_list->GetCurrentSelection();
    auto name = this->m_preset_list->GetString(selected);

    for (auto preset : this->Presets)
    {
        if (preset.second.name == name)
        {
            this->m_cfg->SetValue(preset.second.cfg);
            this->m_clip_skip->SetValue(preset.second.clip_skip);
            // do not save seed
            // this->m_seed->SetValue(preset.second.seed);
            this->m_steps->SetValue(preset.second.steps);
            // when the width || height input is disabled, do not modify it (eg.: controlnet works...)
            if (this->m_width->IsEnabled() || this->m_height->IsEnabled())
            {
                this->m_width->SetValue(preset.second.width);
                this->m_height->SetValue(preset.second.height);
            }
            this->m_sampler->SetSelection(preset.second.sampler);
            this->m_batch_count->SetValue(preset.second.batch);
        }
    }
}

void MainWindowUI::onSelectPreset(wxCommandEvent &event)
{
    if (this->m_preset_list->GetCurrentSelection() == 0)
    {
        this->m_load_preset->Disable();
        this->m_delete_preset->Disable();
    }
    else
    {
        this->m_load_preset->Enable();
        this->m_delete_preset->Enable();
    }
}

void MainWindowUI::onDeletePreset(wxCommandEvent &event)
{

    auto name = this->m_preset_list->GetStringSelection().ToStdString();

    if (this->Presets.find(name) != this->Presets.end())
    {
        auto preset = this->Presets[name];
        std::remove(preset.path.c_str());
        this->LoadPresets();
    }
}

void MainWindowUI::ChangeGuiFromQueueItem(QM::QueueItem item)
{
    this->m_seed->SetValue(item.params.seed);
    this->m_width->SetValue(item.params.width);
    this->m_height->SetValue(item.params.height);
    this->m_steps->SetValue(item.params.sample_steps);
    this->m_clip_skip->SetValue(item.params.clip_skip);
    this->m_controlnetStrength->SetValue(item.params.control_strength);
    this->m_cfg->SetValue(item.params.cfg_scale);
    this->m_batch_count->SetValue(item.params.batch_count);

    if (!item.params.model_path.empty() && std::filesystem::exists(item.params.model_path))
    {
        sd_gui_utils::ModelFileInfo model = this->ModelManager->getInfo(item.params.model_path);
        this->ChangeModelByInfo(model);
    }

    this->m_sampler->SetSelection(item.params.sample_method);

    if (!item.params.taesd_path.empty())
    {
        int index = 0;
        for (auto taesd : this->TaesdFiles)
        {
            if (item.params.taesd_path == taesd.second)
            {
                for (int _u = 0; _u < this->m_taesd->GetCount(); ++_u)
                {
                    if (this->m_taesd->GetString(_u) == taesd.first)
                    {
                        this->m_taesd->Select(_u);
                        break;
                    }
                }
            }
            index++;
        }
    }

    if (!item.params.vae_path.empty() && std::filesystem::exists(item.params.vae_path))
    {
        for (auto vae : this->VaeFiles)
        {
            if (item.params.vae_path == vae.second)
            {
                for (int _u = 0; _u < this->m_vae->GetCount(); ++_u)
                {
                    if (this->m_vae->GetString(_u) == vae.first)
                    {
                        this->m_vae->Select(_u);
                        break;
                    }
                }
            }
        }
    }

    auto type_name = sd_gui_utils::sd_type_gui_names[item.params.wtype];
    for (int index = 0; index < this->m_type->GetCount(); ++index)
    {
        if (this->m_type->GetString(index) == wxString(type_name))
        {
            this->m_type->Select(index);
            break;
        }
    }

    this->m_vae_tiling->SetValue(item.params.vae_tiling);

    if (!item.params.control_image_path.empty())
    {
        if (std::filesystem::exists(item.params.control_image_path))
        {
            wxImage img;
            img.LoadFile(item.params.control_image_path);
            this->m_controlnetImagePreview->SetBitmap(img);
            this->m_controlnetImageOpen->SetPath(item.params.control_image_path);
        }

        for (auto cnmodel : this->ControlnetModels)
        {
            if (cnmodel.second == item.params.controlnet_path)
            {
                for (int _u = 0; _u < this->m_controlnetModels->GetCount(); ++_u)
                {
                    if (this->m_controlnetModels->GetString(_u) == cnmodel.first)
                    {
                        this->m_controlnetModels->Select(_u);
                        break;
                    }
                }
            }
        }
    }
}

void MainWindowUI::UpdateModelInfoDetailsFromModelList(sd_gui_utils::ModelFileInfo *modelinfo)
{
    this->m_model_details->DeleteAllItems();
    this->m_model_details_description->Hide();
    this->m_model_details_description->SetPage("");
    this->m_data_model_url->SetLabel("");
    this->m_data_model_url->SetURL("");
    this->modelPreviewImageList->Destroy();
    this->modelPreviewImageList->Create(256, 256);
    this->m_model_details_imagelist->DeleteAllItems();

    if (modelinfo->state == sd_gui_utils::CivitAiState::NOT_FOUND || modelinfo->state == sd_gui_utils::CivitAiState::ERR)
    {
        return;
    }
    if (modelinfo->civitaiPlainJson.empty())
    {
        return;
    }

    this->m_data_model_url->SetLabel(modelinfo->CivitAiInfo.model.name + modelinfo->CivitAiInfo.model.name);
    wxString url = wxString::Format("https://civitai.com/models/%d", modelinfo->CivitAiInfo.modelId);
    this->m_data_model_url->SetURL(url);

    wxVector<wxVariant> data;

    data.push_back(wxVariant("Name"));
    data.push_back(wxVariant(wxString::Format("%s %s", modelinfo->CivitAiInfo.name, modelinfo->CivitAiInfo.model.name)));
    this->m_model_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("File name"));
    data.push_back(wxVariant(wxString::Format("%s", modelinfo->path)));
    this->m_model_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("Hash"));
    data.push_back(wxVariant(wxString::Format("%s", modelinfo->sha256)));
    this->m_model_details->AppendItem(data);
    data.clear();

    data.push_back(wxVariant("Type"));
    data.push_back(wxVariant(wxString::Format("%s", modelinfo->CivitAiInfo.model.type)));
    this->m_model_details->AppendItem(data);
    data.clear();

    if (modelinfo->CivitAiInfo.description.empty())
    {
        this->m_model_details_description->Hide();
        this->m_model_details_description->SetPage("");
    }
    else
    {
        this->m_model_details_description->Show();
        // this->m_model_details_description->SetLabelMarkup(modelinfo->CivitAiInfo.description);
        this->m_model_details_description->SetPage(modelinfo->CivitAiInfo.description);
    }
    int idx = 0;
    for (auto file : modelinfo->CivitAiInfo.files)
    {
        data.push_back(wxVariant(wxString::Format("#%d id", idx)));
        data.push_back(wxVariant(wxString::Format("%d", file.id)));
        this->m_model_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(wxString::Format("#%d name", idx)));
        data.push_back(wxVariant(wxString::Format("%s", file.name)));
        this->m_model_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(wxString::Format("#%d format", idx)));
        data.push_back(wxVariant(wxString::Format("%s", file.metadata.format)));
        this->m_model_details->AppendItem(data);
        data.clear();

        data.push_back(wxVariant(wxString::Format("#%d type", idx)));
        data.push_back(wxVariant(wxString::Format("%s", file.metadata.fp)));
        this->m_model_details->AppendItem(data);
        data.clear();
        idx++;
    }

    for (auto img : modelinfo->preview_images)
    {
        if (!std::filesystem::exists(img))
        {
            continue;
        }
        wxImage pimg;
        if (!pimg.LoadFile(img))
        {
            continue;
        }
        auto resized = sd_gui_utils::cropResizeImage(pimg, 256, 256, wxColour(*wxBLACK));
        wxBitmap bmap(resized);
        this->modelPreviewImageList->Add(resized);
    }
    this->m_model_details_imagelist->SetImageList(this->modelPreviewImageList, wxIMAGE_LIST_NORMAL);

    int index0 = 0;
    for (auto img : modelinfo->preview_images)
    {
        std::filesystem::path *image_path = new std::filesystem::path(img);
        if (std::filesystem::exists(*image_path))
        {
            auto id = this->m_model_details_imagelist->InsertItem(idx, image_path->filename().string(), index0);
            this->m_model_details_imagelist->SetItemPtrData(id, wxUIntPtr(image_path));
            index0++;
        }
        else
        {
            auto id = this->m_model_details_imagelist->InsertItem(index0, wxString::Format("Deleted: %s", image_path->filename().string()));
            this->m_model_details_imagelist->SetItemPtrData(id, wxUIntPtr(image_path));
            this->m_model_details_imagelist->SetItemTextColour(id, wxColour(247, 88, 35)); // near orange, but red
        }
    }
}

void MainWindowUI::OnQueueItemManagerItemAdded(QM::QueueItem item)
{
    wxVector<wxVariant> data;

    auto created_at = sd_gui_utils::formatUnixTimestampToDate(item.created_at);

    data.push_back(wxVariant(std::to_string(item.id)));
    data.push_back(wxVariant(created_at));
    data.push_back(wxVariant(sd_gui_utils::modes_str[item.mode]));
    data.push_back(wxVariant(item.model));
    if (item.mode == QM::GenerationMode::UPSCALE)
    {
        data.push_back(wxVariant("--")); // sample method
        data.push_back(wxVariant("--")); // seed
    }
    else
    {
        data.push_back(wxVariant(sd_gui_utils::sample_method_str[(int)item.params.sample_method]));
        data.push_back(wxVariant(std::to_string(item.params.seed)));
    }

    data.push_back(item.status == QM::QueueStatus::DONE ? 100 : 1); // progressbar
    data.push_back(wxString("-.--it/s"));                           // speed
    data.push_back(wxVariant(QM::QueueStatus_str[item.status]));    // status

    auto store = this->m_joblist->GetStore();

    QM::QueueItem *nItem = new QM::QueueItem(item);

    this->JobTableItems[item.id] = nItem;
    //  store->AppendItem(data, wxUIntPtr(this->JobTableItems[item.id]));
    store->PrependItem(data, wxUIntPtr(this->JobTableItems[item.id]));
}

void MainWindowUI::OnQueueItemManagerItemUpdated(QM::QueueItem item)
{
}

MainWindowUI::~MainWindowUI()
{
    if (this->modelLoaded)
    {
        free_sd_ctx(this->txt2img_sd_ctx);
    }
    if (this->upscaleModelLoaded)
    {
        free_upscaler_ctx(this->upscaler_sd_ctx);
    }
    for (auto &t : this->threads)
    {
        t->join();
    }

    this->TaskBar->Destroy();
}

void MainWindowUI::loadSamplerList()
{
    this->m_sampler->Clear();
    for (auto sampler : sd_gui_utils::sample_method_str)
    {
        int _u = this->m_sampler->Append(sampler);

        if (sampler == sd_gui_utils::sample_method_str[this->sd_params->sample_method])
        {
            this->m_sampler->Select(_u);
        }
    }
}

void MainWindowUI::loadControlnetList()
{
    this->LoadFileList(sd_gui_utils::DirTypes::CONTROLNET);
}

void MainWindowUI::loadEmbeddingList()
{
    this->LoadFileList(sd_gui_utils::DirTypes::EMBEDDING);
}

void MainWindowUI::OnPopupClick(wxCommandEvent &evt)
{
    // void *data = static_cast<wxMenu *>(evt.GetEventObject())->GetClientData();
    wxMenu *m = (wxMenu *)evt.GetClientObject();
    auto te = evt.GetInt();
    auto tu = evt.GetId();

    // 100 for queueitem list table
    if (tu < 100)
    {
        wxDataViewListStore *store = this->m_joblist->GetStore();
        auto currentItem = this->m_joblist->GetCurrentItem();
        auto currentRow = this->m_joblist->GetSelectedRow();

        QM::QueueItem *_qitem = reinterpret_cast<QM::QueueItem *>(store->GetItemData(currentItem));
        QM::QueueItem *qitem = new QM::QueueItem(this->qmanager->GetItem(_qitem->id));

        /*
                1 Copy and queue
                2 copy to text2img
                3 copy prompts to text2image
                4 copy prompts to img2img
                5 Details
                99 delete
        */

        switch (tu)
        {
        case 1:
            this->qmanager->Duplicate(qitem->id);
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
        case 5:
        {
            auto model = this->ModelManager->getInfo(qitem->params.model_path);
            this->ChangeModelByInfo(model);
        }
        break;
        case 6:
        {
            this->m_upscaler_filepicker->SetPath(qitem->images.back());
            this->onUpscaleImageOpen(qitem->images.back());
            this->m_notebook1302->SetSelection(3); // switch to the upscaler
        }
        break;
        case 7:
        {
            this->m_open_image->SetPath(qitem->images.back());
            this->onimg2ImgImageOpen(qitem->images.back());
            this->m_notebook1302->SetSelection(2); // switch to the img2img
        }
        break;
        case 99:
        {
            if (this->qmanager->DeleteJob(qitem->id))
            {
                store->DeleteItem(currentRow);
            }
        }
        break;
        }
        delete qitem;
    }
    /*
    101   lora txt2img
    102   lora txt2img neg
    103   lora img2img
    104   lora img2img neg
    */
    if (tu >= 100)
    {
        wxDataViewListStore *store = this->m_data_model_list->GetStore();
        int currow = this->m_data_model_list->GetSelectedRow();
        auto currentItem = store->GetItem(currow);

        sd_gui_utils::ModelFileInfo *modelinfo = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(store->GetItemData(currentItem));

        switch (tu)
        {
        case 100:
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelHashCalc, this, this->GetEventHandler(), modelinfo));
            break;
        case 101:
            this->m_prompt->SetValue(fmt::format("{} <lora:{}:0.5>", this->m_prompt->GetValue().ToStdString(), modelinfo->name));
            break;
        case 102:
            this->m_neg_prompt->SetValue(fmt::format("{} <lora:{}:0.5>", this->m_neg_prompt->GetValue().ToStdString(), modelinfo->name));
            break;
        case 103:
            this->m_prompt2->SetValue(fmt::format("{} <lora:{}:0.5>", this->m_prompt2->GetValue().ToStdString(), modelinfo->name));
            break;
        case 104:
            this->m_neg_prompt2->SetValue(fmt::format("{} <lora:{}:0.5>", this->m_neg_prompt2->GetValue().ToStdString(), modelinfo->name));
            break;
        case 105:
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
            break;
        case 111:
            this->m_prompt->SetValue(fmt::format("{} {}", this->m_prompt->GetValue().ToStdString(), modelinfo->name));
            break;
        case 112:
            this->m_neg_prompt->SetValue(fmt::format("{} {}", this->m_neg_prompt->GetValue().ToStdString(), modelinfo->name));
            break;
        case 113:
            this->m_prompt2->SetValue(fmt::format("{} {}", this->m_prompt2->GetValue().ToStdString(), modelinfo->name));
            break;
        case 114:
            this->m_neg_prompt2->SetValue(fmt::format("{} {}", this->m_neg_prompt2->GetValue().ToStdString(), modelinfo->name));
            break;
        case 115:
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
            break;
        case 200:
            this->ChangeModelByInfo(*modelinfo);
            break;
        }
    }
}

void MainWindowUI::loadVaeList()
{

    this->LoadFileList(sd_gui_utils::DirTypes::VAE);
}

void MainWindowUI::loadEsrganList()
{
    this->LoadFileList(sd_gui_utils::DirTypes::ESRGAN);
}

void MainWindowUI::refreshModelTable(std::string filter)
{

    auto types = sd_gui_utils::DirTypes::LORA | sd_gui_utils::DirTypes::CHECKPOINT | sd_gui_utils::DirTypes::EMBEDDING;

    if (!this->m_checkbox_lora_filter->IsChecked())
    {
        types &= ~sd_gui_utils::DirTypes::LORA;
    }

    if (!this->m_checkbox_filter_checkpoints->IsChecked())
    {
        types &= ~sd_gui_utils::DirTypes::CHECKPOINT;
    }
    if (!this->m_checkbox_filter_embeddings->IsChecked())
    {
        types &= ~sd_gui_utils::DirTypes::EMBEDDING;
    }

    auto store = this->m_data_model_list->GetStore();

    std::vector<sd_gui_utils::ModelFileInfo> list = this->ModelManager->getList();
    std::vector<sd_gui_utils::ModelFileInfo> filtered_list;

    std::copy_if(list.begin(), list.end(), std::back_inserter(filtered_list),
                 [types](const sd_gui_utils::ModelFileInfo &info)
                 { return sd_gui_utils::filterByModelType(info.model_type, types); });

    if (!filter.empty())
    {
        std::vector<sd_gui_utils::ModelFileInfo> tmp_list;
        std::copy_if(filtered_list.begin(), filtered_list.end(), std::back_inserter(tmp_list),
                     [types, filter](const sd_gui_utils::ModelFileInfo &model)
                     {
                         auto name = model.name;
                         auto path = model.path;
                         std::transform(name.begin(), name.end(), name.begin(),
                                        [](unsigned char c)
                                        { return std::tolower(c); });
                         std::transform(path.begin(), path.end(), path.begin(),
                                        [](unsigned char c)
                                        { return std::tolower(c); });

                         return (path.find(filter) != std::string::npos || name.find(filter) != std::string::npos) ||
                                ((!model.civitaiPlainJson.empty() && model.hash_progress_size != 0) && (model.CivitAiInfo.name.find(filter) != std::string::npos || model.CivitAiInfo.model.name.find(filter) != std::string::npos));
                     });
        filtered_list = tmp_list;
    }

    store->DeleteAllItems();
    for (auto ele : filtered_list)
    {

        wxVector<wxVariant> itemData;
        itemData.push_back(ele.name);                                   // name
        itemData.push_back(ele.size_f);                                 // size
        itemData.push_back(sd_gui_utils::dirtypes_str[ele.model_type]); // type
        itemData.push_back(ele.sha256.substr(0, 10));                   // autov2 hash
        itemData.push_back(sd_gui_utils::civitai_state_str[ele.state]); // civitai state
        if (ele.hash_progress_size > 0)
        {
            int current_progress = 0;
            if (ele.hash_fullsize != 0)
            {
                current_progress = static_cast<int>((ele.hash_progress_size * 100) / ele.hash_fullsize);
            }
            itemData.push_back(current_progress); // progress bar
        }
        else
        {
            itemData.push_back(ele.sha256.empty() ? 0 : 100); // progress bar
        }
        store->AppendItem(itemData, (wxUIntPtr)this->ModelManager->getIntoPtr(ele.path)); // setClientData
    }
    // remove which not needed
    this->m_data_model_list->Refresh();
}

void MainWindowUI::loadTaesdList()
{

    this->LoadFileList(sd_gui_utils::DirTypes::TAESD);
}

std::string MainWindowUI::paramsToImageComment(QM::QueueItem myItem, sd_gui_utils::ModelFileInfo modelInfo)
{
    auto modelPath = std::filesystem::path(modelInfo.path);

    std::string comment = "charset=Unicode " + myItem.params.prompt;
    if (!myItem.params.negative_prompt.empty())
    {
        comment = comment + fmt::format("\nNegative prompt: {}", myItem.params.negative_prompt);
    }
    comment = comment + fmt::format("\nSteps: {}, Seed: {}, Sampler: {}, CFG scale: {}, Size: {}x{}, Parser: sd.cpp, Model: {}, Model hash: {}, Backend: sd.cpp, App: sd.cpp.gui.wx, Operations: {}",
                                    myItem.params.sample_steps,
                                    myItem.params.seed,
                                    sd_gui_utils::sample_method_str[(int)myItem.params.sample_method],
                                    myItem.params.cfg_scale,
                                    myItem.params.width, myItem.params.height,
                                    modelPath.filename().replace_extension().string(),
                                    modelInfo.sha256.substr(0, 10), // autov2 hash (the first 10 char from sha256 :) )
                                    sd_gui_utils::modes_str[(int)myItem.mode]);

    if (!myItem.params.vae_path.empty())
    {
        auto vae_path = std::filesystem::path(myItem.params.vae_path);
        comment = comment + fmt::format("VAE: {}", vae_path.filename().replace_extension().string());
    }

    return comment;
}

template <typename T>
inline void MainWindowUI::SendThreadEvent(wxEvtHandler *eventHandler, QM::QueueEvents eventType, const T &payload, std::string text)
{
    wxThreadEvent *e = new wxThreadEvent();
    e->SetString(wxString::Format("%d:%d:%s", (int)sd_gui_utils::ThreadEvents::QUEUE, (int)eventType, text));
    e->SetPayload(payload);
    wxQueueEvent(eventHandler, e);
}
template <typename T>
void MainWindowUI::SendThreadEvent(wxEvtHandler *eventHandler, sd_gui_utils::ThreadEvents eventType, const T &payload, std::string text)
{
    wxThreadEvent *e = new wxThreadEvent();
    e->SetString(wxString::Format("%d:%s", (int)eventType, text));
    e->SetPayload(payload);
    wxQueueEvent(eventHandler, e);
}
void MainWindowUI::LoadFileList(sd_gui_utils::DirTypes type)
{
    std::string basepath;

    switch (type)
    {
    case sd_gui_utils::DirTypes::VAE:
        this->VaeFiles.clear();
        this->m_vae->Clear();
        this->m_vae->Append("-none-");
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
        this->m_model->Append("-none-");
        this->m_model->Select(0);
        basepath = this->cfg->model;
        break;
    case sd_gui_utils::DirTypes::PRESETS:
        this->Presets.clear();
        this->m_preset_list->Clear();
        this->m_preset_list->Append("-none-");
        this->m_preset_list->Select(0);
        basepath = this->cfg->presets;
        break;
    case sd_gui_utils::DirTypes::TAESD:
    {
        this->m_taesd->Clear();
        this->m_taesd->Append("-none-");
        this->m_taesd->Select(0);
        basepath = this->cfg->taesd;
    }
    break;
    case sd_gui_utils::DirTypes::EMBEDDING:
    {
        basepath = this->cfg->embedding;
    }
    break;
    case sd_gui_utils::DirTypes::CONTROLNET:
        this->m_controlnetModels->Clear();
        this->m_controlnetModels->Append("-none-");
        this->m_controlnetModels->Select(0);
        basepath = this->cfg->controlnet;
        break;
    case sd_gui_utils::DirTypes::ESRGAN:
        this->m_upscaler_model->Clear();
        this->m_upscaler_model->Append("-none-");
        this->m_upscaler_model->Select(0);
        basepath = this->cfg->esrgan;
        break;
    }

    int i = 0;
    for (auto const &dir_entry : std::filesystem::recursive_directory_iterator(basepath))
    {
        if (!dir_entry.exists() || !dir_entry.is_regular_file() || !dir_entry.path().has_extension())
        {
            continue;
        }

        std::filesystem::path path = dir_entry.path();

        std::string ext = path.extension().string();

        if (type == sd_gui_utils::DirTypes::CHECKPOINT)
        {
            if (ext != ".safetensors" && ext != ".ckpt" && ext != ".gguf")
            {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::LORA)
        {
            if (ext != ".safetensors" && ext != ".ckpt" && ext != ".gguf")
            {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::EMBEDDING)
        {
            if (ext != ".safetensors" && ext != ".pt")
            {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::VAE)
        {
            if (ext != ".safetensors" && ext != ".ckpt")
            {
                continue;
            }
        }

        if (type == sd_gui_utils::DirTypes::PRESETS)
        {
            if (ext != ".json")
            {
                continue;
            }
        }

        if (type == sd_gui_utils::DirTypes::TAESD)
        {
            if (ext != ".pth" && ext != ".safetensors" && ext != ".gguf")
            {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::ESRGAN)
        {
            if (ext != ".pth")
            {
                continue;
            }
        }
        if (type == sd_gui_utils::DirTypes::CONTROLNET)
        {
            if (ext != ".safetensors" && ext != ".pth")
            {
                continue;
            }
        }
        std::string name = path.filename().replace_extension("").string();
        // prepend the subdirectory to the modelname
        // // wxFileName::GetPathSeparator()
        auto path_name = path.string();
        sd_gui_utils::replace(path_name, basepath, "");
        sd_gui_utils::replace(path_name, "//", "");
        sd_gui_utils::replace(path_name, "\\\\", "");
        sd_gui_utils::replace(path_name, ext, "");

        name = path_name.substr(1);

        if (type == sd_gui_utils::DirTypes::CHECKPOINT)
        {
            this->ModelFiles.emplace(name, dir_entry.path().string());
            this->ModelFilesIndex[name] = this->ModelFiles.size();
            this->ModelManager->addModel(dir_entry.path().string(), type, name);

            sd_gui_utils::ModelFileInfo *_minfo = this->ModelManager->getIntoPtr(dir_entry.path().string());

            auto lid = this->m_model->Append(name, (void *)_minfo);
        }
        if (type == sd_gui_utils::DirTypes::LORA)
        {
            this->LoraFiles.emplace(name, dir_entry.path().string());
            this->ModelManager->addModel(dir_entry.path().string(), type, name);
        }
        if (type == sd_gui_utils::DirTypes::EMBEDDING)
        {
            this->EmbeddingFiles.emplace(name, dir_entry.path().string());
            this->ModelManager->addModel(dir_entry.path().string(), type, name);
        }
        if (type == sd_gui_utils::DirTypes::ESRGAN)
        {
            this->EsrganFiles.emplace(name, dir_entry.path().string());
            this->m_upscaler_model->Append(name);
        }
        if (type == sd_gui_utils::DirTypes::VAE)
        {
            this->m_vae->Append(name);
            this->VaeFiles.emplace(name, dir_entry.path().string());
        }
        if (type == sd_gui_utils::DirTypes::PRESETS)
        {
            sd_gui_utils::generator_preset preset;
            std::ifstream f(path.string());
            try
            {
                nlohmann::json data = nlohmann::json::parse(f);
                preset = data;
                preset.path = path.string();
                this->m_preset_list->Append(preset.name);
                this->Presets.emplace(preset.name, preset);
            }
            catch (const std::exception &e)
            {
                std::remove(path.string().c_str());
                std::cerr << e.what() << " file: " << path.string() << '\n';
            }
        }
        if (type == sd_gui_utils::DirTypes::TAESD)
        {
            this->m_taesd->Append(name);
            this->TaesdFiles.emplace(name, dir_entry.path().string());
        }
        if (type == sd_gui_utils::DirTypes::CONTROLNET)
        {
            this->m_controlnetModels->Append(name);
            this->ControlnetModels.emplace(name, dir_entry.path().string());
        }
    }

    if (type == sd_gui_utils::DirTypes::CHECKPOINT)
    {
        this->logs->AppendText(fmt::format("Loaded checkpoints: {}\n", this->ModelFiles.size()));
        if (this->ModelFiles.size() > 0)
        {
            this->m_model->Enable();
        }
        else
        {
            this->m_model->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::LORA)
    {
        this->logs->AppendText(fmt::format("Loaded Loras: {}\n", this->LoraFiles.size()));
    }
    if (type == sd_gui_utils::DirTypes::EMBEDDING)
    {
        this->logs->AppendText(fmt::format("Loaded embeddings: {}\n", this->EmbeddingFiles.size()));
    }
    if (type == sd_gui_utils::DirTypes::VAE)
    {
        this->logs->AppendText(fmt::format("Loaded vaes: {}\n", this->VaeFiles.size()));
        if (this->VaeFiles.size() > 0)
        {
            this->m_vae->Enable();
        }
        else
        {
            this->m_vae->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::PRESETS)
    {
        this->logs->AppendText(fmt::format("Loaded presets: {}\n", this->Presets.size()));
        if (this->Presets.size() > 0)
        {
            this->m_preset_list->Enable();
        }
        else
        {
            this->m_preset_list->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::TAESD)
    {
        this->logs->AppendText(fmt::format("Loaded taesd: {}\n", this->TaesdFiles.size()));
        if (this->TaesdFiles.size() > 0)
        {
            this->m_taesd->Enable();
        }
        else
        {
            this->m_taesd->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::ESRGAN)
    {
        this->logs->AppendText(fmt::format("Loaded esrgan: {}\n", this->EsrganFiles.size()));
        if (this->EsrganFiles.size() > 0)
        {
            this->m_upscaler_model->Enable();
        }
        else
        {
            this->m_upscaler_model->Disable();
        }
    }
    if (type == sd_gui_utils::DirTypes::CONTROLNET)
    {
        this->logs->AppendText(fmt::format("Loaded controlnet: {}\n", this->ControlnetModels.size()));
        if (this->ControlnetModels.size() > 0)
        {
            this->m_controlnetModels->Enable();
        }
        else
        {
            this->m_controlnetModels->Disable();
        }
    }
}
void MainWindowUI::loadLoraList()
{
    this->LoadFileList(sd_gui_utils::DirTypes::LORA);
}

void MainWindowUI::OnCloseSettings(wxCloseEvent &event)
{
    this->initConfig();
    this->settingsWindow->Destroy();
}

void MainWindowUI::imageCommentToGuiParams(std::map<std::string, std::string> params, sd_gui_utils::SDMode mode)
{
    bool modelFound = false;
    for (auto item : params)
    {
        // sampler
        if (item.first == "sampler")
        {
            /// we default sampler is euler_a :)
            /// in automatic, the default is unipc, we dont have it
            if (item.second == "Default")
            {
                this->m_sampler->Select(0);
                continue;
            }
            unsigned int index = 0;
            for (auto sampler : sd_gui_utils::sample_method_str)
            {
                if (sampler == item.second)
                {
                    this->m_sampler->Select(index);
                    break;
                }
                index++;
            }
        }
        // the seed
        if (item.first == "seed")
        {
            if (this->m_seed->GetMax() >= std::atoi(item.second.c_str()))
            {
                this->m_seed->SetValue(std::atoi(item.second.c_str()));
            }
        }
        if (item.first == "size")
        {
            size_t pos = item.second.find("x");
            std::string w, h;
            w = item.second.substr(0, pos);
            h = item.second.substr(pos + 1);
            this->m_width->SetValue(std::atoi(w.c_str()));
            this->m_height->SetValue(std::atoi(h.c_str()));
        }
        if (item.first == "steps")
        {
            this->m_steps->SetValue(std::atoi(item.second.c_str()));
        }
        if (item.first == "cfgscale")
        {
            this->m_cfg->SetValue(static_cast<double>(std::atof(item.second.c_str())));
        }
        if (item.first == "negative_prompt")
        {
            if (mode == sd_gui_utils::SDMode::IMG2IMG)
            {
                this->m_neg_prompt2->SetValue(item.second);
            }
            else
            {
                this->m_neg_prompt->SetValue(item.second);
            }
        }
        if (item.first == "prompt")
        {
            if (mode == sd_gui_utils::SDMode::IMG2IMG)
            {
                this->m_prompt2->SetValue(item.second);
            }
            else
            {
                this->m_prompt->SetValue(item.second);
            }
        }
        // first check by hash
        if (item.first == "modelhash" && !modelFound)
        {
            auto check = this->ModelManager->getByHash(item.second);
            if (!check.path.empty())
            {
                this->m_model->Select(this->ModelFilesIndex[check.name]);
                modelFound = true;
                continue;
            }
        }
        if (item.first == "model" && !modelFound)
        {
            // get by name
            auto check = this->ModelManager->getInfoByName(item.second);
            if (!check.path.empty())
            {
                this->m_model->Select(this->ModelFilesIndex[check.name]);
                modelFound = true;
                continue;
            }
            // search by name
            auto check2 = this->ModelManager->findInfoByName(item.second);
            if (!check.path.empty())
            {
                this->m_model->Select(this->ModelFilesIndex[check2.name]);
                modelFound = true;
                continue;
            }
        }

        if (item.first == "vae")
        {
            unsigned int index = 0;
            for (auto vae : this->VaeFiles)
            {
                if (vae.second.find(item.second) != std::string::npos)
                {
                    this->m_vae->Select(index);
                    continue;
                }
                index++;
            }
        }
    }
}

void MainWindowUI::onimg2ImgImageOpen(std::string file)
{
    wxImage img;
    if (img.LoadFile(file))
    {
        auto origSize = this->m_img2img_preview->GetSize();

        auto preview = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(), origSize.GetHeight());
        this->currentInitialImage = new wxImage(img);
        this->currentInitialImagePreview = new wxImage(preview);
        this->currentInitialImagePath = file;

        this->m_img2img_preview->SetScaleMode(wxStaticBitmap::Scale_AspectFill);
        this->AppOrigPlaceHolderBitmap = this->m_img2img_preview->GetBitmap();
        this->m_img2img_preview->SetBitmap(preview);
        this->m_img2img_preview->SetSize(origSize);

        this->m_img2im_preview_img->Enable();
        this->m_delete_initial_img->Enable();
        if (this->m_model->GetSelection() > 0)
        {
            this->m_generate1->Enable();
        }
        this->m_open_image->SetPath(file);

        // png not working... yet...
        if (img.GetType() == wxBITMAP_TYPE_PNG)
        {
            return;
        }

        try
        {

            Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(file);
            if (!image->good())
            {
                return;
            }
            image->readMetadata();
            Exiv2::ExifData &exifData = image->exifData();
            if (!exifData.empty())
            {
                std::string ex;
                Exiv2::ExifData::iterator it;
                std::string usercomment;
                for (it = exifData.begin(); it != exifData.end(); ++it)
                {
                    // ex += fmt::format("Key: {} ", it->key());
                    if (it->key() == "Exif.Photo.UserComment")
                    {
                        usercomment = it->getValue()->toString();
                        if (!usercomment.empty())
                        {
                            std::map<std::string, std::string> getParams = sd_gui_utils::parseExifPrompts(usercomment);
                            this->imageCommentToGuiParams(getParams, sd_gui_utils::SDMode::IMG2IMG);
                            for (auto _s : getParams)
                            {
                                std::cerr << "key: " << _s.first << " val: " << _s.second << "\n";
                            }
                            break;
                        }
                    }
                }
            }
        }
        catch (Exiv2::Error &e)
        {
            std::cerr << "Err: " << e.what() << std::endl;
        }
    }
    else
    {
        wxMessageBox("Can not open image!");
    }
}

void MainWindowUI::onUpscaleImageOpen(std::string file)
{
    wxImage img;
    if (img.LoadFile(file))
    {
        this->currentUpscalerSourceImage = new wxImage(img);
        this->m_upscaler_filepicker->SetPath(file);
        auto origSize = this->m_upscaler_source_image->GetSize();
        this->AppOrigPlaceHolderBitmap = this->m_upscaler_source_image->GetBitmap();

        auto preview = sd_gui_utils::ResizeImageToMaxSize(img, origSize.GetWidth(), origSize.GetHeight());

        this->m_generate_upscaler->Enable();
        this->m_upscaler_source_image->SetScaleMode(wxStaticBitmap::Scale_AspectFill);
        this->m_upscaler_source_image->SetBitmap(preview);
        this->m_upscaler_source_image->SetSize(origSize);

        this->m_static_upscaler_height->SetLabel(wxString::Format("%dpx", img.GetHeight()));
        this->m_static_upscaler_width->SetLabel(wxString::Format("%dpx", img.GetWidth()));

        int factor = this->m_upscaler_factor->GetValue();
        int target_width = factor * img.GetWidth();
        int target_height = factor * img.GetHeight();

        this->m_static_upscaler_target_height->SetLabel(wxString::Format("%dpx", target_height));
        this->m_static_upscaler_target_width->SetLabel(wxString::Format("%dpx", target_width));
        if (this->m_upscaler_model->GetCurrentSelection() != 0)
        {
            this->m_generate_upscaler->Enable();
        }
        else
        {
            this->m_generate_upscaler->Disable();
        }
    }
}

void MainWindowUI::StartGeneration(QM::QueueItem myJob)
{

    if (myJob.mode == QM::GenerationMode::TXT2IMG)
    {
        std::thread *p = new std::thread(&MainWindowUI::GenerateTxt2img, this, this->GetEventHandler(), myJob);
        this->threads.emplace_back(p);
    }
    if (myJob.mode == QM::GenerationMode::IMG2IMG)
    {
        std::thread *p = new std::thread(&MainWindowUI::GenerateImg2img, this, this->GetEventHandler(), myJob);
        this->threads.emplace_back(p);
    }
    if (myJob.mode == QM::GenerationMode::UPSCALE)
    {
        std::thread *p = new std::thread(&MainWindowUI::GenerateUpscale, this, this->GetEventHandler(), myJob);
        this->threads.emplace_back(p);
    }
}

void MainWindowUI::loadTypeList()
{
    this->m_type->Clear();
    unsigned int selected = 0;
    for (auto type : sd_gui_utils::sd_type_gui_names)
    {

        this->m_type->Append(type.second);
        if (type.second == "Default")
        {
            this->m_type->Select(selected);
        }
        selected++;
    }
}

void MainWindowUI::ModelStandaloneHashingCallback(size_t readed_size, std::string sha256, void *custom_pointer)
{
    sd_gui_utils::VoidHolder *holder = static_cast<sd_gui_utils::VoidHolder *>(custom_pointer);

    wxEvtHandler *eventHandler = (wxEvtHandler *)holder->p1;
    sd_gui_utils::ModelFileInfo *modelinfo = (sd_gui_utils::ModelFileInfo *)holder->p2;
    modelinfo->hash_progress_size = readed_size;
    if (readed_size > 0)
    {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
    }
}

void MainWindowUI::OnQueueItemManagerItemStatusChanged(QM::QueueItem item)
{
    auto store = this->m_joblist->GetStore();

    int lastCol = this->m_joblist->GetColumnCount() - 1;

    for (unsigned int i = 0; i < store->GetItemCount(); i++)
    {
        auto _item = store->GetItem(i);
        auto _item_data = store->GetItemData(_item);
        auto *_qitem = reinterpret_cast<QM::QueueItem *>(_item_data);
        if (_qitem->id == item.id)
        {
            store->SetValueByRow(wxVariant(QM::QueueStatus_str[item.status]), i, lastCol);
            this->m_joblist->Refresh();
            break;
        }
    }
}

void MainWindowUI::OnThreadMessage(wxThreadEvent &e)
{
    if (e.GetSkipped() == false)
    {
        e.Skip();
    }
    auto msg = e.GetString().ToStdString();

    std::string token = msg.substr(0, msg.find(":"));
    std::string content = msg.substr(msg.find(":") + 1);
    sd_gui_utils::ThreadEvents threadEvent = (sd_gui_utils::ThreadEvents)std::stoi(token);

    if (threadEvent == sd_gui_utils::ThreadEvents::QUEUE)
    {
        // only numbers here...
        QM::QueueEvents event = (QM::QueueEvents)std::stoi(content);
        QM::QueueItem payload = e.GetPayload<QM::QueueItem>();
        std::string title;
        std::string message;
        switch (event)
        {
            // new item added
        case QM::QueueEvents::ITEM_ADDED:
            this->OnQueueItemManagerItemAdded(payload);
            break;
            // item status changed
        case QM::QueueEvents::ITEM_STATUS_CHANGED:
            this->OnQueueItemManagerItemStatusChanged(payload);
            break;
            // item updated... ? ? ?
        case QM::QueueEvents::ITEM_UPDATED:
            this->OnQueueItemManagerItemUpdated(payload);
            break;
        case QM::QueueEvents::ITEM_START: // this is just the item start, if no mode loaded, then wil trigger model load event
            this->StartGeneration(payload);
            message = wxString::Format("%s is just stared to generate %d images\nModel: %s", sd_gui_utils::modes_str[payload.mode], payload.params.batch_count, payload.model).ToStdString();
            if (payload.mode == QM::GenerationMode::UPSCALE)
            {
                title = "Upscaling started";
                message = wxString::Format("Upscaling the image is started: %s\nModel: %s", payload.initial_image, payload.model);
            }
            else
            {
                if (payload.params.batch_count > 1)
                {
                    title = wxString::Format("%d images generation started", payload.params.batch_count).ToStdString();
                }
                else
                {
                    title = "One image generation started!";
                    message = wxString::Format("%s is just started to generate the image\nModel: %s", sd_gui_utils::modes_str[payload.mode], payload.model).ToStdString();
                }
            }
            this->ShowNotification(title, message);

            break;
        case QM::QueueEvents::ITEM_FINISHED:
            message = wxString::Format("%s is just finished to generate %d images\nModel: %s", sd_gui_utils::modes_str[payload.mode], payload.params.batch_count, payload.model).ToStdString();
            if (payload.mode == QM::GenerationMode::UPSCALE)
            {
                title = "Upscaling done";
                message = wxString::Format("Upscaling the image is done: %s\nModel: %s", payload.initial_image, payload.model);
            }
            else
            {
                if (payload.params.batch_count > 1)
                {
                    title = wxString::Format("%d images generation done", payload.params.batch_count).ToStdString();
                }
                else
                {
                    title = "One image generation done!";
                    message = wxString::Format("%s is just finished to generate the image\nModel: %s", sd_gui_utils::modes_str[payload.mode], payload.model).ToStdString();
                }
            }
            this->ShowNotification(title, message);
            break;
        case QM::QueueEvents::ITEM_MODEL_LOADED: // MODEL_LOAD_DONE
            if (payload.mode == QM::GenerationMode::IMG2IMG || payload.mode == QM::GenerationMode::TXT2IMG)
            {
                this->modelLoaded = true;
            }
            if (payload.mode == QM::GenerationMode::UPSCALE)
            {
                this->upscaleModelLoaded = true;
            }
            this->logs->AppendText(fmt::format("Model loaded: {}\n", payload.model));
            break;
        case QM::QueueEvents::ITEM_MODEL_LOAD_START: // MODEL_LOAD_START
            this->logs->AppendText(fmt::format("Model load start: {}\n", payload.model));
            break;
        case QM::QueueEvents::ITEM_MODEL_FAILED: // MODEL_LOAD_ERROR
            this->logs->AppendText(fmt::format("Model load error: {}\n", payload.status_message));

            if (payload.mode == QM::GenerationMode::IMG2IMG || payload.mode == QM::GenerationMode::TXT2IMG)
            {
                this->modelLoaded = false;
            }
            if (payload.mode == QM::GenerationMode::UPSCALE)
            {
                this->upscaleModelLoaded = false;
            }
            title = "Model load failed";
            message = wxString::Format("The '%s' just failed to load... for more details please see the logs!", payload.model).ToStdString();
            this->ShowNotification(title, message);
            break;
        case QM::QueueEvents::ITEM_GENERATION_STARTED: // GENERATION_START
            if (payload.mode == QM::GenerationMode::IMG2IMG || payload.mode == QM::GenerationMode::TXT2IMG)
            {
                this->logs->AppendText(fmt::format("Diffusion started. Seed: {} Batch: {} {}x{}px Cfg: {} Steps: {}\n",
                                                   payload.params.seed,
                                                   payload.params.batch_count,
                                                   payload.params.width,
                                                   payload.params.height,
                                                   payload.params.cfg_scale,
                                                   payload.params.sample_steps));
            }
            if (payload.mode == QM::GenerationMode::UPSCALE)
            {
                this->logs->AppendText(wxString::Format("Upscale start, factor: %d image: %s\n", payload.upscale_factor, payload.initial_image));
            }
            break;
        case QM::QueueEvents::ITEM_FAILED: // GENERATION_ERROR
            this->logs->AppendText(fmt::format("Generation error: {}\n", payload.status_message));
            break;
        default:
            break;
        }
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_DONE)
    {
        sd_gui_utils::ModelFileInfo *modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo *>();
        this->ModelManager->UpdateInfo(modelinfo);
        this->logs->AppendText(fmt::format("Model civitai image downloaded for model: {}\n", modelinfo->name));
        // update if the current selected item is the updated item info
        if (this->m_data_model_list->GetSelectedItemsCount() > 0)
        {
            wxDataViewItem item = this->m_data_model_list->GetCurrentItem();
            sd_gui_utils::ModelFileInfo *info = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(this->m_data_model_list->GetItemData(item));
            if (info->path == modelinfo->path)
            {
                this->UpdateModelInfoDetailsFromModelList(modelinfo);
            }
        }
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_IMAGES_START)
    {
        sd_gui_utils::ModelFileInfo *modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo *>();
        this->ModelManager->UpdateInfo(modelinfo);
        this->logs->AppendText(fmt::format("Model civitai {} image(s) download started for model: {}\n", modelinfo->CivitAiInfo.images.size(), modelinfo->name));
        return;
    }

    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FAILED)
    {
        sd_gui_utils::ModelFileInfo *modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo *>();
        this->logs->AppendText(fmt::format("Model civitai info download error: {}\n", modelinfo->name));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_START)
    {
        sd_gui_utils::ModelFileInfo *modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo *>();
        this->logs->AppendText(fmt::format("Model civitai info download start: {}\n", modelinfo->name));
        return;
    }
    if (threadEvent == sd_gui_utils::ThreadEvents::MODEL_INFO_DOWNLOAD_FINISHED)
    {
        sd_gui_utils::ModelFileInfo *modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo *>();
        sd_gui_utils::ModelFileInfo newInfo = this->ModelManager->updateCivitAiInfo(modelinfo);
        sd_gui_utils::ModelFileInfo *newInfoptr = this->ModelManager->getIntoPtr(newInfo.path);

        if (newInfo.state == sd_gui_utils::CivitAiState::OK)
        {
            this->logs->AppendText(fmt::format("Model civitai info download finished: {}\n", newInfo.CivitAiInfo.name));
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoImageDownload, this, this->GetEventHandler(), newInfoptr));
        }
        if (newInfo.state == sd_gui_utils::CivitAiState::NOT_FOUND)
        {
            this->logs->AppendText(fmt::format("Model civitai info not found: {} Hash: {}\n", newInfo.name, newInfo.sha256.substr(0, 10)));
        }
        if (newInfo.state == sd_gui_utils::CivitAiState::ERR)
        {
            this->logs->AppendText(fmt::format("Model civitai info unkown parsing error happened: {} Hash: {}\n", newInfo.name, newInfo.sha256.substr(0, 10)));
        }
        // update anyway
        this->UpdateModelInfoDetailsFromModelList(modelinfo);
        // update table
        auto store = this->m_data_model_list->GetStore();
        int civitAiCol = this->m_data_model_list->GetColumnCount() - 2; // civitai col

        for (unsigned int i = 0; i < store->GetItemCount(); i++)
        {
            auto _item = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto *_qitem = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(_item_data);
            if (_qitem->path == modelinfo->path)
            {
                store->SetValueByRow(sd_gui_utils::civitai_state_str[modelinfo->state], i, civitAiCol);
                this->m_data_model_list->Refresh();
                break;
            }
        }
        // update table

        return;
    }

    /// status of hashing from modellist
    if (threadEvent == sd_gui_utils::STANDALONE_HASHING_PROGRESS)
    {
        // modelinfo
        sd_gui_utils::ModelFileInfo *modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo *>();
        auto store = this->m_data_model_list->GetStore();
        int progressCol = this->m_data_model_list->GetColumnCount() - 1; // progressbar col
        int hashCol = this->m_data_model_list->GetColumnCount() - 3;
        size_t _x = modelinfo->hash_progress_size;
        size_t _m = modelinfo->hash_fullsize;
        int current_progress = 0;
        auto _hr1 = sd_gui_utils::humanReadableFileSize(static_cast<double>(_x));
        auto _hr2 = sd_gui_utils::humanReadableFileSize(static_cast<double>(_m));

        if (_m != 0)
        {
            current_progress = static_cast<int>((_x * 100) / _m);
        }

        for (unsigned int i = 0; i < store->GetItemCount(); i++)
        {
            auto _item = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto *_qitem = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(_item_data);
            if (_qitem->path == modelinfo->path)
            {
                store->SetValueByRow(current_progress, i, progressCol);
                store->SetValueByRow(modelinfo->sha256.substr(0, 10), i, hashCol);
                this->m_data_model_list->Refresh();
                break;
            }
        }
    }
    if (threadEvent == sd_gui_utils::STANDALONE_HASHING_DONE)
    {
        sd_gui_utils::ModelFileInfo *modelinfo = e.GetPayload<sd_gui_utils::ModelFileInfo *>();
        if (modelinfo->civitaiPlainJson.empty())
        {
            this->threads.emplace_back(new std::thread(&MainWindowUI::threadedModelInfoDownload, this, this->GetEventHandler(), modelinfo));
        }
        nlohmann::json j(*modelinfo);
        std::ofstream file(modelinfo->meta_file);
        file << j;
        file.close();
        modelinfo->hash_progress_size = 0;
        return;
    }
    if (threadEvent == sd_gui_utils::HASHING_PROGRESS)
    {
        QM::QueueItem myjob = e.GetPayload<QM::QueueItem>();

        // update column
        auto store = this->m_joblist->GetStore();

        int progressCol = this->m_joblist->GetColumnCount() - 3;

        size_t _x = myjob.hash_progress_size;
        size_t _m = myjob.hash_fullsize;
        int current_progress = 0;
        auto _hr1 = sd_gui_utils::humanReadableFileSize(static_cast<double>(_x));
        auto _hr2 = sd_gui_utils::humanReadableFileSize(static_cast<double>(_m));

        if (_m != 0)
        {
            current_progress = static_cast<int>((_x * 100) / _m);
        }

        for (unsigned int i = 0; i < store->GetItemCount(); i++)
        {
            auto _item = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto *_qitem = reinterpret_cast<QM::QueueItem *>(_item_data);
            if (_qitem->id == myjob.id)
            {
                store->SetValueByRow(current_progress, i, progressCol);
                this->m_joblist->Refresh();
                break;
            }
        }
        return;
    }
    if (threadEvent == sd_gui_utils::HASHING_DONE)
    {
        QM::QueueItem myjob = e.GetPayload<QM::QueueItem>();

        // update column
        auto store = this->m_joblist->GetStore();

        int progressCol = this->m_joblist->GetColumnCount() - 3;

        for (unsigned int i = 0; i < store->GetItemCount(); i++)
        {
            auto _item = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto *_qitem = reinterpret_cast<QM::QueueItem *>(_item_data);
            if (_qitem->id == myjob.id)
            {
                store->SetValueByRow(0, i, progressCol);
                this->m_joblist->Refresh();
                break;
            }
        }
        this->ModelManager->getIntoPtr(myjob.params.model_path)->hash_progress_size = 0;
        return;
    }
    if (threadEvent == sd_gui_utils::GENERATION_PROGRESS)
    {
        QM::QueueItem myjob = e.GetPayload<QM::QueueItem>();

        // update column
        auto store = this->m_joblist->GetStore();
        wxString speed = wxString::Format(myjob.time > 1.0f ? "%.2fs/it" : "%.2fit/s", myjob.time > 1.0f || myjob.time == 0 ? myjob.time : (1.0f / myjob.time));
        int progressCol = this->m_joblist->GetColumnCount() - 3;
        int speedCol = this->m_joblist->GetColumnCount() - 2;
        float current_progress = 100.f * (static_cast<float>(myjob.step) / static_cast<float>(myjob.steps));

        for (unsigned int i = 0; i < store->GetItemCount(); i++)
        {
            auto _item = store->GetItem(i);
            auto _item_data = store->GetItemData(_item);
            auto *_qitem = reinterpret_cast<QM::QueueItem *>(_item_data);
            if (_qitem->id == myjob.id)
            {
                store->SetValueByRow(static_cast<int>(current_progress), i, progressCol);
                store->SetValueByRow(speed, i, speedCol);
                this->m_joblist->Refresh();
                break;
            }
        }
        return;
    }
    if (threadEvent == sd_gui_utils::SD_MESSAGE)
    {
        if (content.length() < 1)
        {
            return;
        }
        this->logs->AppendText(fmt::format("{}", content));
        return;
    }
    if (threadEvent == sd_gui_utils::MESSAGE)
    {
        this->logs->AppendText(fmt::format("{}\n", content));
        return;
    }
}

void MainWindowUI::HandleSDProgress(int step, int steps, float time, void *data)
{
    sd_gui_utils::VoidHolder *objs = (sd_gui_utils::VoidHolder *)data;
    wxEvtHandler *eventHandler = (wxEvtHandler *)objs->p1;
    QM::QueueItem *myItem = (QM::QueueItem *)objs->p2;
    myItem->step = step;
    myItem->steps = steps;
    myItem->time = time;

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::GENERATION_PROGRESS, *myItem, fmt::format("{}/{}", step, steps));
}

QM::QueueItem MainWindowUI::handleSdImage(sd_image_t result, QM::QueueItem item, wxEvtHandler *eventHandler)
{
    wxImage *img = new wxImage(result.width, result.height, result.data);

    if (!img->IsOk())
    {
        item.status_message = fmt::format("Invalid image from diffusion...");
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, item);
        return item;
    }
    std::string filename = this->cfg->output;
    std::string extension = ".jpg";
    auto imgHandler = wxBITMAP_TYPE_JPEG;

    if (this->cfg->image_type == sd_gui_utils::imageTypes::PNG)
    {
        extension = ".png";
        imgHandler = wxBITMAP_TYPE_PNG;
    }
    if (this->cfg->image_type == sd_gui_utils::imageTypes::JPG)
    {
        extension = ".jpg";
        imgHandler = wxBITMAP_TYPE_JPEG;
        img->SetOption("quality", this->cfg->image_quality);
    }
    std::string filename_without_extension;
    filename = filename + wxFileName::GetPathSeparator();
    filename = filename + sd_gui_utils::modes_str[item.mode];
    filename = filename + "_";
    filename = filename + std::to_string(item.id);
    filename = filename + "_";
    filename = filename + std::to_string(item.params.seed);
    filename = filename + "_";
    filename = filename + std::to_string(result.width) + "x" + std::to_string(result.height);
    filename = filename + "_";

    filename_without_extension = filename;
    filename = filename + extension;

    int _c = 0;
    while (std::filesystem::exists(filename))
    {
        filename = filename_without_extension + "_" + std::to_string(_c) + extension;
        _c++;
    }

    if (!img->SaveFile(filename, imgHandler))
    {
        item.status_message = fmt::format("Failed to save image into {}", filename);
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, item);
        // delete img;
        return item;
    }
    else
    {
        item.images.emplace_back(filename);
        if (item.params.control_image_path.length() > 0 && this->cfg->save_all_image)
        {
            std::string ctrlFilename = this->cfg->output;
            ctrlFilename = filename_without_extension + "_ctrlimg_" + extension;
            wxImage _ctrlimg(item.params.control_image_path);
            _ctrlimg.SaveFile(ctrlFilename);
        }
        // add generation parameters into the image meta
        if (this->cfg->image_type == sd_gui_utils::imageTypes::JPG)
        {
            std::string comment = this->paramsToImageComment(item, this->ModelManager->getInfo(item.params.model_path));

            try
            {
                Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(filename);
                image->readMetadata();
                Exiv2::ExifData &exifData = image->exifData();
                exifData["Exif.Photo.UserComment"] = comment;
                exifData["Exif.Image.XPComment"] = comment;
                // image->setComment(comment);

                // PNG fájl metaadatainak frissítése
                image->setExifData(exifData);
                image->writeMetadata();
            }
            catch (Exiv2::Error &e)
            {
                std::cerr << "Err: " << e.what() << std::endl;
            }
        }
    }
    free(result.data);
    result.data = NULL;
    delete img;
    return item;
}

void MainWindowUI::ModelHashingCallback(size_t readed_size, std::string sha256, void *custom_pointer)
{
    sd_gui_utils::VoidHolder *holder = static_cast<sd_gui_utils::VoidHolder *>(custom_pointer);
    wxEvtHandler *eventHandler = (wxEvtHandler *)holder->p1;
    QM::QueueItem *myItem = (QM::QueueItem *)holder->p2;
    // update the model list too
    sd_gui_utils::ModelFileInfo *modelinfo = (sd_gui_utils::ModelFileInfo *)holder->p3;
    modelinfo->hash_progress_size = readed_size;
    modelinfo->hash_fullsize = myItem->hash_fullsize;
    myItem->hash_progress_size = readed_size;

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::HASHING_PROGRESS, *myItem);

    /*
     wxThreadEvent *e = new wxThreadEvent();
     e->SetString(wxString::Format("%d:placeholder", sd_gui_utils::HASHING_PROGRESS));
     e->SetPayload(*myItem);
     wxQueueEvent(eventHandler, e);
     */

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_PROGRESS, modelinfo);
    // send info to the model list tooo
    /*
    wxThreadEvent *r = new wxThreadEvent();
    e->SetString(wxString::Format("%d:placeholder", sd_gui_utils::STANDALONE_HASHING_PROGRESS));
    r->SetPayload(modelinfo);
    wxQueueEvent(eventHandler, r);
    */
}

void MainWindowUI::ShowNotification(std::string title, std::string message)
{
    if (this->cfg->show_notifications)
    {
        wxNotificationMessage notification(title, message, this);
        notification.UseTaskBarIcon(this->TaskBar);
        notification.Show(this->cfg->notification_timeout);
    }
}

void MainWindowUI::HandleSDLog(sd_log_level_t level, const char *text, void *data)
{
    // TODO: hadle visible levels from the GUI
    if (level == sd_log_level_t::SD_LOG_INFO || level == sd_log_level_t::SD_LOG_ERROR)
    {
        auto *eventHandler = (wxEvtHandler *)data;
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::SD_MESSAGE, level, text);
    }
}

sd_ctx_t *MainWindowUI::LoadModelv2(wxEvtHandler *eventHandler, QM::QueueItem myItem)
{
    /*
    wxThreadEvent *e = new wxThreadEvent();
    e->SetString(wxString::Format("MODEL_LOAD_START:%s", myItem.params.model_path));
    e->SetPayload(myItem);
    wxQueueEvent(eventHandler, e);
*/

    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_LOAD_START, myItem);

    bool model_exists = this->ModelManager->exists(myItem.params.model_path);
    if (!model_exists)
    {
        this->ModelManager->addModel(myItem.params.model_path, sd_gui_utils::DirTypes::CHECKPOINT, myItem.model);
    }
    // check if the model have a sha256
    if (model_exists)
    {
        auto model = this->ModelManager->getInfo(myItem.params.model_path);
        if (model.sha256.empty())
        {
            QM::QueueItem *item(&myItem);
            item->hash_fullsize = model.size;
            sd_gui_utils::VoidHolder *holder = new sd_gui_utils::VoidHolder;
            holder->p1 = (void *)eventHandler;
            holder->p2 = (void *)item;
            holder->p3 = (void *)this->ModelManager->getIntoPtr(model.path);
            auto hash = sd_gui_utils::sha256_file_openssl(model.path.c_str(), (void *)holder, &MainWindowUI::ModelHashingCallback);
            this->ModelManager->setHash(myItem.params.model_path, hash);
        }
    }

    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_LOAD_START, myItem);

    // std::lock_guard<std::mutex> guard(this->sdMutex);
    sd_ctx_t *sd_ctx_ = new_sd_ctx(
        sd_gui_utils::repairPath(myItem.params.model_path).c_str(),      // model path
        sd_gui_utils::repairPath(myItem.params.vae_path).c_str(),        // vae path
        sd_gui_utils::repairPath(myItem.params.taesd_path).c_str(),      // taesd path
        sd_gui_utils::repairPath(myItem.params.controlnet_path).c_str(), // controlnet path
        sd_gui_utils::repairPath(myItem.params.lora_model_dir).c_str(),  // lora path
        sd_gui_utils::repairPath(myItem.params.embeddings_path).c_str(), // embedding path
        false,                                                           // vae decode only (img2img = false)
        myItem.params.vae_tiling,                                        // vae tiling
        false,                                                           // free params immediatelly
        myItem.params.n_threads,
        myItem.params.wtype,
        myItem.params.rng_type,
        myItem.params.schedule,
        myItem.params.control_net_cpu);

    if (sd_ctx_ == NULL)
    {
        this->modelLoaded = false;
        return nullptr;
    }
    else
    {
        /* wxThreadEvent *c = new wxThreadEvent();
         c->SetString(wxString::Format("MODEL_LOAD_DONE:%s", myItem.params.model_path));
         c->SetPayload(sd_ctx_);
         wxQueueEvent(eventHandler, c);*/
        myItem.status_message = myItem.params.model_path;
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_LOADED, myItem);
        this->modelLoaded = true;
        this->currentModel = myItem.params.model_path;
        this->currentVaeModel = myItem.params.vae_path;
        this->currentTaesdModel = myItem.params.taesd_path;
        this->currentwType = myItem.params.wtype;
        this->currentControlnetModel = myItem.params.controlnet_path;
    }
    return sd_ctx_;
}

upscaler_ctx_t *MainWindowUI::LoadUpscaleModel(wxEvtHandler *eventHandler, QM::QueueItem myItem)
{
    upscaler_ctx_t *u_ctx = new_upscaler_ctx(myItem.params.esrgan_path.c_str(),
                                             myItem.params.n_threads,
                                             myItem.params.wtype);
    if (u_ctx == NULL)
    {
        this->upscaleModelLoaded = false;
        return NULL;
    }
    else
    {
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_LOADED, myItem);
        this->currentUpscalerModel = myItem.params.esrgan_path;
        this->upscaleModelLoaded = true;
    }
    return u_ctx;
}

void MainWindowUI::LoadPresets()
{
    this->LoadFileList(sd_gui_utils::DirTypes::PRESETS);
}

void MainWindowUI::ChangeModelByName(wxString ModelName)
{
    sd_gui_utils::ModelFileInfo minfo = this->ModelManager->findInfoByName(ModelName.ToStdString());

    for (unsigned int _z = 0; _z < this->m_model->GetCount(); _z++)
    {
        sd_gui_utils::ModelFileInfo *m = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(this->m_model->GetClientData(_z));

        if (m == nullptr)
        {
            continue;
        }
        if (m->path == minfo.path)
        {
            this->m_model->SetSelection(_z);
            this->m_generate2->Enable();
            if (this->m_open_image->GetPath().length() > 0)
            {
                this->m_generate1->Enable();
            }
            return;
        }
    }
}

void MainWindowUI::ChangeModelByInfo(sd_gui_utils::ModelFileInfo &info)
{
    for (unsigned int _z = 0; _z < this->m_model->GetCount(); _z++)
    {
        sd_gui_utils::ModelFileInfo *m = reinterpret_cast<sd_gui_utils::ModelFileInfo *>(this->m_model->GetClientData(_z));
        // maybe this is the none
        if (m == nullptr)
        {
            continue;
        }
        if (info == m)
        {
            this->m_model->SetSelection(_z);
            return;
        }
    }
}

void MainWindowUI::loadModelList()
{
    std::string oldSelection = this->m_model->GetStringSelection().ToStdString();

    this->LoadFileList(sd_gui_utils::DirTypes::CHECKPOINT);

    if (this->ModelFilesIndex.find(oldSelection) != this->ModelFilesIndex.end())
    {
        this->m_model->SetSelection(this->ModelFilesIndex[oldSelection]);
    }
}

void MainWindowUI::initConfig()
{
    this->fileConfig = new wxFileConfig("sd.cpp.ui", wxEmptyString, this->ini_path);
    wxConfigBase::Set(fileConfig);

    wxString datapath = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + "sd_ui_data" + wxFileName::GetPathSeparator();
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

    wxString controlnet_path = datapath;
    controlnet_path.append("controlnet");

    wxString esrgan_path = datapath;
    esrgan_path.append("esrgan");

    this->cfg->datapath = datapath;

    this->cfg->lora = this->fileConfig->Read("/paths/lora", lora_path).ToStdString();
    this->cfg->model = this->fileConfig->Read("/paths/model", model_path).ToStdString();
    this->cfg->vae = this->fileConfig->Read("/paths/vae", vae_path).ToStdString();
    this->cfg->embedding = this->fileConfig->Read("/paths/embedding", embedding_path).ToStdString();
    this->cfg->taesd = this->fileConfig->Read("/paths/taesd", taesd_path).ToStdString();
    this->cfg->esrgan = this->fileConfig->Read("/paths/esrgan", esrgan_path).ToStdString();
    this->cfg->controlnet = this->fileConfig->Read("/paths/controlnet", controlnet_path).ToStdString();
    this->cfg->presets = this->fileConfig->Read("/paths/presets", presets_path).ToStdString();

    this->cfg->jobs = this->fileConfig->Read("/paths/jobs", jobs_path).ToStdString();

    this->cfg->output = this->fileConfig->Read("/paths/output", imagespath).ToStdString();
    this->cfg->keep_model_in_memory = this->fileConfig->Read("/keep_model_in_memory", this->cfg->keep_model_in_memory);
    this->cfg->save_all_image = this->fileConfig->Read("/save_all_image", this->cfg->save_all_image);
    this->cfg->n_threads = this->fileConfig->Read("/n_threads", cores());

    this->cfg->show_notifications = this->fileConfig->ReadBool("/show_notification", this->cfg->show_notifications);
    this->cfg->notification_timeout = this->fileConfig->Read("/notification_timeout", this->cfg->notification_timeout);

    this->cfg->image_quality = this->fileConfig->Read("/image_quality", this->cfg->image_quality);

    int idx = 0;
    auto saved_image_type = this->fileConfig->Read("/image_type", "JPG");

    for (auto type : sd_gui_utils::image_types_str)
    {
        if (saved_image_type == type)
        {
            this->cfg->image_type = (sd_gui_utils::imageTypes)idx;
            break;
        }
        idx++;
    }
    // populate data from sd_params as default...

    if (!this->modelLoaded && this->firstCfgInit)
    {
        this->m_cfg->SetValue(static_cast<double>(this->sd_params->cfg_scale));
        this->m_seed->SetValue(static_cast<int>(this->sd_params->seed));
        this->m_clip_skip->SetValue(this->sd_params->clip_skip);
        this->m_steps->SetValue(this->sd_params->sample_steps);
        this->m_width->SetValue(this->sd_params->width);
        this->m_height->SetValue(this->sd_params->height);
        this->m_batch_count->SetValue(this->sd_params->batch_count);
        this->loadSamplerList();
        this->loadTypeList();

        // check if directories exists...
        if (!std::filesystem::exists(model_path.ToStdString()))
        {
            std::filesystem::create_directories(model_path.ToStdString());
        }
        if (!std::filesystem::exists(lora_path.ToStdString()))
        {
            std::filesystem::create_directories(lora_path.ToStdString());
        }
        if (!std::filesystem::exists(vae_path.ToStdString()))
        {
            std::filesystem::create_directories(vae_path.ToStdString());
        }
        if (!std::filesystem::exists(embedding_path.ToStdString()))
        {
            std::filesystem::create_directories(embedding_path.ToStdString());
        }
        if (!std::filesystem::exists(taesd_path.ToStdString()))
        {
            std::filesystem::create_directories(taesd_path.ToStdString());
        }
        if (!std::filesystem::exists(esrgan_path.ToStdString()))
        {
            std::filesystem::create_directories(esrgan_path.ToStdString());
        }
        if (!std::filesystem::exists(presets_path.ToStdString()))
        {
            std::filesystem::create_directories(presets_path.ToStdString());
        }
        if (!std::filesystem::exists(jobs_path.ToStdString()))
        {
            std::filesystem::create_directories(jobs_path.ToStdString());
        }
        if (!std::filesystem::exists(controlnet_path.ToStdString()))
        {
            std::filesystem::create_directories(controlnet_path.ToStdString());
        }
        if (!std::filesystem::exists(imagespath.ToStdString()))
        {
            std::filesystem::create_directories(imagespath.ToStdString());
        }
    }
    this->firstCfgInit = false;
}

void MainWindowUI::OnExit(wxEvent &event)
{
    this->Close();
}

void MainWindowUI::threadedModelHashCalc(wxEvtHandler *eventHandler, sd_gui_utils::ModelFileInfo *modelinfo)
{

    modelinfo->hash_fullsize = modelinfo->size;

    sd_gui_utils::VoidHolder *holder = new sd_gui_utils::VoidHolder;
    holder->p1 = (void *)eventHandler;
    holder->p2 = (void *)modelinfo;

    modelinfo->sha256 = sd_gui_utils::sha256_file_openssl(modelinfo->path.c_str(), (void *)holder, &MainWindowUI::ModelStandaloneHashingCallback);
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::STANDALONE_HASHING_DONE, modelinfo);
}

void MainWindowUI::threadedModelInfoDownload(wxEvtHandler *eventHandler, sd_gui_utils::ModelFileInfo *modelinfo)
{

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_START, modelinfo);

    std::list<std::string> headers;
    headers.push_back("Content-Type: text/json;");
    std::string url = "https://civitai.com/api/v1/model-versions/by-hash/" + modelinfo->sha256.substr(0, 10);
    std::cerr << "Getting info from " << url << std::endl;
    std::ostringstream response;
    try
    {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy request;
        request.setOpt(new curlpp::options::HttpHeader(headers));
        request.setOpt(new curlpp::options::WriteStream(&response));
        request.setOpt<curlpp::options::Url>(url);
        request.perform();
        std::cerr << std::string(response.str()) << std::endl;
        modelinfo->civitaiPlainJson = response.str();
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FINISHED, modelinfo);
        return;
    }

    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
        wxMessageDialog dialog(this, e.what());
        modelinfo->civitaiPlainJson = response.str();
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FAILED, modelinfo);
        return;
    }

    catch (curlpp::LogicError &e)
    {
        wxMessageDialog dialog(this, e.what());
        modelinfo->civitaiPlainJson = response.str();
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FAILED, modelinfo);
        return;
    }
    std::cerr << std::string(response.str()) << std::endl;
    modelinfo->civitaiPlainJson = response.str();
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_FAILED, modelinfo);
}

void MainWindowUI::threadedModelInfoImageDownload(wxEvtHandler *eventHandler, sd_gui_utils::ModelFileInfo *modelinfo)
{
    if (modelinfo->civitaiPlainJson.empty())
    {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, std::string("No model info found."));
        return;
    }

    if (modelinfo->CivitAiInfo.images.empty())
    {
        MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, "No images found.");
        return;
    }

    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGES_START, modelinfo);
    int index = 0;
    for (auto img : modelinfo->CivitAiInfo.images)
    {
        /// download
        std::list<std::string> headers;
        headers.push_back("Content-Type: text/json;");

        std::ostringstream response(std::stringstream::binary);
        try
        {
            curlpp::Cleanup myCleanup;

            // Our request to be sent.
            curlpp::Easy request;
            // request.setOpt(new curlpp::options::HttpHeader(headers));
            request.setOpt(new curlpp::options::WriteStream(&response));
            request.setOpt<curlpp::options::Url>(img.url);
            request.perform();
            std::string target_path = std::filesystem::path(this->cfg->datapath + "/" + modelinfo->sha256 + "_" + std::to_string(index) + ".tmp").generic_string();
            std::ofstream file(target_path, std::ios::binary);
            file << response.str();
            file.close();

            wxImage _tmpImg;
            _tmpImg.SetLoadFlags(_tmpImg.GetLoadFlags() & ~wxImage::Load_Verbose);

            if (_tmpImg.LoadFile(target_path))
            {

                if (_tmpImg.GetType() == wxBITMAP_TYPE_JPEG)
                {
                    std::string new_path = std::filesystem::path(target_path).replace_extension(".jpg").generic_string();
                    std::filesystem::rename(target_path, new_path);
                    modelinfo->preview_images.emplace_back(new_path);
                    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGES_PROGRESS, modelinfo, img.url);
                    index++;
                }
                if (_tmpImg.GetType() == wxBITMAP_TYPE_PNG)
                {
                    std::string new_path = std::filesystem::path(target_path).replace_extension(".png").generic_string();
                    std::filesystem::rename(target_path, new_path);
                    modelinfo->preview_images.emplace_back(new_path);
                    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGES_PROGRESS, modelinfo, img.url);
                    index++;
                }
            }
            _tmpImg.Destroy();
            continue;
        }

        catch (curlpp::RuntimeError &e)
        {
            std::cout << e.what() << std::endl;
            MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, img.url);
            continue;
        }

        catch (curlpp::LogicError &e)
        {
            MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, img.url);
            continue;
        }
        /// download
    }
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGES_DONE, modelinfo);
}

void MainWindowUI::GenerateTxt2img(wxEvtHandler *eventHandler, QM::QueueItem myItem)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    sd_gui_utils::VoidHolder *vparams = new sd_gui_utils::VoidHolder;
    vparams->p1 = (void *)this->GetEventHandler();
    vparams->p2 = (void *)&myItem;

    sd_set_progress_callback(MainWindowUI::HandleSDProgress, (void *)vparams);

    if (!this->modelLoaded)
    {
        this->txt2img_sd_ctx = this->LoadModelv2(eventHandler, myItem);
        this->currentModel = myItem.params.model_path;
        this->currentVaeModel = myItem.params.vae_path;
        this->currentTaesdModel = myItem.params.taesd_path;
        this->currentwType = myItem.params.wtype;
        this->currentControlnetModel = myItem.params.controlnet_path;
    }
    else
    { // the model must be reloaded when: models is changed, vae is changed, taesd is changed
        if (myItem.params.model_path != this->currentModel ||
            this->currentVaeModel != myItem.params.vae_path ||
            this->currentTaesdModel != myItem.params.taesd_path ||
            this->currentwType != myItem.params.wtype ||
            this->currentControlnetModel != myItem.params.controlnet_path)
        {
            free_sd_ctx(this->txt2img_sd_ctx);
            this->txt2img_sd_ctx = this->LoadModelv2(eventHandler, myItem);
        }
        else
        {
            // it's a bug in sd.cpp, can not generate new image with controlnet, if already generated one...
            // they don't known, because the example sd.exe is just a one - time runner app... sadly...
            // so we need to destroy the ctx and re init it
            /* if (myItem.params.controlnet_path.length() > 0)
             {
                 free_sd_ctx(this->txt2img_sd_ctx);
                 this->txt2img_sd_ctx = this->LoadModelv2(eventHandler, myItem);
             }
             */
        }
    }

    if (!this->modelLoaded || this->txt2img_sd_ctx == nullptr)
    {
        //       wxThreadEvent *f = new wxThreadEvent();
        //        f->SetString("GENERATION_ERROR:Model load failed...");
        //        f->SetPayload(myItem);
        //        wxQueueEvent(eventHandler, f);
        myItem.status_message = fmt::format("Model load failed: {}", myItem.model);
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_FAILED, myItem);
        return;
    }

    auto start = std::chrono::system_clock::now();

    /* wxThreadEvent *e = new wxThreadEvent();
     e->SetString(wxString::Format("GENERATION_START:%s", this->sd_params->model_path));
     e->SetPayload(myItem);
     wxQueueEvent(eventHandler, e);*/
    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_GENERATION_STARTED, myItem);
    sd_image_t *control_image = NULL;
    sd_image_t *results;
    // prepare controlnet image, if have
    // if we have, but no model, the myItem will not contains any image
    if (myItem.params.control_image_path.length() > 0)
    {
        if (std::filesystem::exists(myItem.params.control_image_path))
        {
            int c = 0;
            int w, h;
            stbi_uc *input_image_buffer = stbi_load(myItem.params.control_image_path.c_str(), &w, &h, &c, 3);
            control_image = new sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
            input_image_buffer = NULL;
            delete input_image_buffer;
        }
    }
    // std::lock_guard<std::mutex> guard(this->sdMutex);

    results = txt2img(this->txt2img_sd_ctx,
                      myItem.params.prompt.c_str(),
                      myItem.params.negative_prompt.c_str(),
                      myItem.params.clip_skip,
                      myItem.params.cfg_scale,
                      myItem.params.width,
                      myItem.params.height,
                      myItem.params.sample_method,
                      myItem.params.sample_steps,
                      myItem.params.seed,
                      myItem.params.batch_count,
                      control_image,
                      myItem.params.control_strength);

    control_image = NULL;
    delete control_image;
    if (results == NULL)
    {
        /*  wxThreadEvent *f = new wxThreadEvent();
          f->SetString("GENERATION_ERROR:Something wrong happened at image generation...");
          f->SetPayload(myItem);
          wxQueueEvent(eventHandler, f);*/

        myItem.status_message = "Empty results after txt2img!";
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, myItem);
        delete results;
        return;
    }
    /* save image(s) */

    const auto p1 = std::chrono::system_clock::now();
    auto ctime = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    for (int i = 0; i < myItem.params.batch_count; i++)
    {
        if (results[i].data == NULL)
        {
            continue;
        }
        myItem = this->handleSdImage(results[i], myItem, eventHandler);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    // send to notify the user...
    wxThreadEvent *h = new wxThreadEvent();
    auto msg = fmt::format("Image generation done in {}s. Saved into {}", elapsed_seconds.count(), this->cfg->output);
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MESSAGE, NULL, msg);
    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FINISHED, myItem);

    // send to the queue manager
    /* wxThreadEvent *j = new wxThreadEvent();
     j->SetString(wxString::Format("QUEUE:%d", QM::QueueEvents::ITEM_FINISHED));
     j->SetPayload(myItem);
     wxQueueEvent(eventHandler, j);*/
}

void MainWindowUI::GenerateImg2img(wxEvtHandler *eventHandler, QM::QueueItem myItem)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    sd_gui_utils::VoidHolder *vparams = new sd_gui_utils::VoidHolder;
    vparams->p1 = (void *)this->GetEventHandler();
    vparams->p2 = (void *)&myItem;

    sd_set_progress_callback(MainWindowUI::HandleSDProgress, (void *)vparams);

    if (!this->modelLoaded)
    {
        this->txt2img_sd_ctx = this->LoadModelv2(eventHandler, myItem);
        this->currentModel = myItem.params.model_path;
        this->currentVaeModel = myItem.params.vae_path;
        this->currentTaesdModel = myItem.params.taesd_path;
        this->currentwType = myItem.params.wtype;
        this->currentControlnetModel = myItem.params.controlnet_path;
    }
    else
    { // the model must be reloaded when: models is changed, vae is changed, taesd is changed
        if (myItem.params.model_path != this->currentModel ||
            this->currentVaeModel != myItem.params.vae_path ||
            this->currentTaesdModel != myItem.params.taesd_path ||
            this->currentwType != myItem.params.wtype)
        {
            free_sd_ctx(this->txt2img_sd_ctx);
            this->txt2img_sd_ctx = this->LoadModelv2(eventHandler, myItem);
        }
    }

    if (!this->modelLoaded)
    {
        myItem.status_message = fmt::format("Model load failed: {}", myItem.model);
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_FAILED, myItem);
        return;
    }

    auto start = std::chrono::system_clock::now();

    sd_image_t *control_image = NULL;
    sd_image_t *results;

    if (std::filesystem::exists(myItem.initial_image))
    {
        int c = 0;
        int w, h;
        stbi_uc *input_image_buffer = stbi_load(myItem.initial_image.c_str(), &w, &h, &c, 3);
        control_image = new sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
        input_image_buffer = NULL;
        delete input_image_buffer;
    }
    else
    {
        /* wxThreadEvent *e = new wxThreadEvent();
         e->SetString(wxString::Format("GENERATION_ERROR:Can not open initial image: %s", myItem.initial_image));
         e->SetPayload(myItem);
         wxQueueEvent(eventHandler, e);
         */
        myItem.status_message = fmt::format("Can not open initial image: {}", myItem.initial_image);
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, myItem);
        return;
    }

    /* wxThreadEvent *e = new wxThreadEvent();
     e->SetString(wxString::Format("GENERATION_START:%s", this->sd_params->model_path));
     e->SetPayload(myItem);
     wxQueueEvent(eventHandler, e);
     */
    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_GENERATION_STARTED, myItem);
    results = img2img(this->txt2img_sd_ctx,
                      *control_image,
                      myItem.params.prompt.c_str(),
                      myItem.params.negative_prompt.c_str(),
                      myItem.params.clip_skip,
                      myItem.params.cfg_scale,
                      myItem.params.width,
                      myItem.params.height,
                      myItem.params.sample_method,
                      myItem.params.sample_steps,
                      myItem.params.strength,
                      myItem.params.seed,
                      myItem.params.batch_count);

    control_image = NULL;
    delete control_image;

    if (results == NULL)
    {
        myItem.status_message = "Empty results after img2img!";
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, myItem);
        delete results;
        return;
    }

    /* save image(s) */

    const auto p1 = std::chrono::system_clock::now();
    auto ctime = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();

    for (int i = 0; i < myItem.params.batch_count; i++)
    {
        if (results[i].data == NULL)
        {
            continue;
        }

        myItem = this->handleSdImage(results[i], myItem, eventHandler);
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    auto msg = fmt::format("Image generation done in {}s. Saved into {}", elapsed_seconds.count(), myItem.params.output_path);
    MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::ThreadEvents::MESSAGE, NULL, msg);
    MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FINISHED, myItem);
}

void MainWindowUI::GenerateUpscale(wxEvtHandler *eventHandler, QM::QueueItem myItem)
{

    std::lock_guard<std::mutex> lock(this->mutex);
    sd_gui_utils::VoidHolder *vparams = new sd_gui_utils::VoidHolder;
    vparams->p1 = (void *)this->GetEventHandler();
    vparams->p2 = (void *)&myItem;

    sd_set_progress_callback(MainWindowUI::HandleSDProgress, (void *)vparams);

    if (this->currentUpscalerModel != myItem.params.esrgan_path)
    {
        if (this->upscaleModelLoaded == true)
        {
            free_upscaler_ctx(this->upscaler_sd_ctx);
            this->upscaleModelLoaded = false;
        }
    }

    if (this->m_keep_other_models_in_memory->GetValue() == false && this->modelLoaded == true)
    {
        free_sd_ctx(this->txt2img_sd_ctx);
        this->modelLoaded = false;
    }

    if (this->upscaleModelLoaded == false)
    {
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_LOAD_START, myItem);
        this->upscaler_sd_ctx = this->LoadUpscaleModel(eventHandler, myItem);

        if (!this->upscaleModelLoaded || this->upscaler_sd_ctx == NULL)
        {
            myItem.status_message = fmt::format("Model load failed: {}", myItem.params.esrgan_path);
            MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_MODEL_FAILED, myItem);
            this->upscaleModelLoaded = false;
            return;
        }
        this->upscaleModelLoaded = true;
    }

    if (std::filesystem::exists(myItem.initial_image))
    {
        int c = 0;
        int w, h;
        stbi_uc *input_image_buffer = stbi_load(myItem.initial_image.c_str(), &w, &h, &c, 3);
        sd_image_t control_image = sd_image_t{(uint32_t)w, (uint32_t)h, 3, input_image_buffer};
        input_image_buffer = NULL;
        delete input_image_buffer;

        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_GENERATION_STARTED, myItem);
        sd_image_t upscaled_image = upscale(this->upscaler_sd_ctx, control_image, myItem.upscale_factor);

        // save img
        wxImage *img = new wxImage(upscaled_image.width, upscaled_image.height, upscaled_image.data);
        std::string filename = this->cfg->output;
        std::string extension = ".jpg";
        auto imgHandler = wxBITMAP_TYPE_JPEG;

        if (this->cfg->image_type == sd_gui_utils::imageTypes::PNG)
        {
            extension = ".png";
            imgHandler = wxBITMAP_TYPE_PNG;
        }
        if (this->cfg->image_type == sd_gui_utils::imageTypes::JPG)
        {
            extension = ".jpg";
            imgHandler = wxBITMAP_TYPE_JPEG;
            img->SetOption("quality", this->cfg->image_quality);
        }

        std::string filename_without_extension;
        filename = filename + wxFileName::GetPathSeparator();
        filename = filename + std::to_string(myItem.id);
        filename = filename + "_";
        filename = filename + std::to_string(myItem.params.seed);
        filename = filename + "_upscaled";
        filename = filename + "_";

        filename_without_extension = filename;
        filename = filename + extension;

        if (img->SaveFile(filename, imgHandler))
        {
            myItem.images.emplace_back(filename);
            MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FINISHED, myItem);
        }
        else
        {
            myItem.status_message = fmt::format("Failed to save image into {}", filename);
            MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, myItem);
        }

        // save img
        if (!this->m_keep_upscaler_in_memory->GetValue())
        {
            this->upscaleModelLoaded = false;
            free_upscaler_ctx(this->upscaler_sd_ctx);
        }
    }
    else
    {
        myItem.status_message = fmt::format("Can not open initial image: {}", myItem.initial_image);
        MainWindowUI::SendThreadEvent(eventHandler, QM::QueueEvents::ITEM_FAILED, myItem);
        return;
    }
}
