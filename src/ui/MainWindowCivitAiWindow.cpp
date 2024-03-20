#include "MainWindowCivitAiWindow.h"

MainWindowCivitAiWindow::MainWindowCivitAiWindow(wxWindow *parent)
    : CivitAiWindow(parent)
{
    Bind(wxEVT_THREAD, &MainWindowCivitAiWindow::OnThreadMessage, this);
}

void MainWindowCivitAiWindow::m_civitai_searchOnTextEnter(wxCommandEvent &event)
{
    if (this->isLoading)
    {
        return;
    }
    this->OnSearch(this->m_civitai_search->GetValue().ToStdString());
}

void MainWindowCivitAiWindow::m_searchOnButtonClick(wxCommandEvent &event)
{
    if (this->isLoading)
    {
        return;
    }
    this->OnSearch(this->m_civitai_search->GetValue().ToStdString());
}

void MainWindowCivitAiWindow::m_dataViewListCtrl5OnDataViewListCtrlSelectionChanged(wxDataViewEvent &event)
{
    auto selected = this->m_dataViewListCtrl5->GetSelectedRow();

    this->m_model_details->DeleteAllItems();
    this->m_model_filelist->DeleteAllItems();
    this->m_model_description->SetPage("");
    this->m_model_version_description->SetPage("");
    for (auto &_d : this->bitmaps)
    {
        _d->Destroy();
    }
    this->bitmaps.clear();
    for (auto &_pi : this->previewImages)
    {
        delete _pi.second;
    }
    this->previewImages.clear();

    if (selected == wxNOT_FOUND)
    {
        return;
    }
    auto item = this->m_dataViewListCtrl5->GetCurrentItem();
    nlohmann::json *json_data = reinterpret_cast<nlohmann::json *>(this->m_dataViewListCtrl5->GetItemData(item));
    nlohmann::json js(*json_data);

    this->populateVersions(js);
}

void MainWindowCivitAiWindow::OnHtmlLinkClicked(wxHtmlLinkEvent &event)
{
    wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
}

void MainWindowCivitAiWindow::m_model_detailsOnDataViewListCtrlSelectionChanged(wxDataViewEvent &event)
{
    this->m_model_filelist->DeleteAllItems();
    if (this->m_model_details->GetSelectedRow() == wxNOT_FOUND)
    {
        return;
    }

    auto item = this->m_model_details->GetCurrentItem();

    nlohmann::json *data = reinterpret_cast<nlohmann::json *>(this->m_model_details->GetItemData(item));
    if (data == nullptr || data == NULL)
    {
        return;
    }
    nlohmann::json js(*data);
    this->populateFiles(js);
    this->loadImages(js);
}

void MainWindowCivitAiWindow::m_model_downloadOnButtonClick(wxCommandEvent &event)
{
    auto selection = this->m_model_filelist->GetSelectedRow();
    if (selection == wxNOT_FOUND)
    {
        this->m_model_download->Disable();
        return;
    }
    auto item = this->m_model_filelist->RowToItem(selection);
    nlohmann::json *jsptr = reinterpret_cast<nlohmann::json *>(this->m_model_filelist->GetItemData(item));
    if (jsptr == NULL || jsptr == nullptr)
    {
        return;
    }
    nlohmann::json js(*jsptr);

    CivitAi::DownloadItem *ditem = new CivitAi::DownloadItem();
    // get the model type...
    auto parentSelection = this->m_dataViewListCtrl5->GetSelectedRow();
    if (parentSelection == wxNOT_FOUND)
    {
        return;
    }
    auto parentItem = this->m_dataViewListCtrl5->RowToItem(parentSelection);
    nlohmann::json *parent_jsptr = reinterpret_cast<nlohmann::json *>(this->m_dataViewListCtrl5->GetItemData(parentItem));
    nlohmann::json parent_js(*parent_jsptr);
    auto path = sd_gui_utils::normalizePath(this->config->model + "/" + js["name"].get<std::string>());

    if (parent_js.contains("type") && !parent_js["type"].is_null())
    {
        std::string type = parent_js["type"].get<std::string>();
        if (type == CivitAi::ModelTypesNames[CivitAi::ModelTypes::LORA])
        {
            ditem->type = CivitAi::ModelTypes::LORA;
            path = sd_gui_utils::normalizePath(this->config->lora + "/" + js["name"].get<std::string>());
        }
        else if (type == CivitAi::ModelTypesNames[CivitAi::ModelTypes::Checkpoint])
        {
            ditem->type = CivitAi::ModelTypes::Checkpoint;
            path = sd_gui_utils::normalizePath(this->config->model + "/" + js["name"].get<std::string>());
        }
        else if (type == CivitAi::ModelTypesNames[CivitAi::ModelTypes::TextualInversion])
        {
            ditem->type = CivitAi::ModelTypes::TextualInversion;
            path = sd_gui_utils::normalizePath(this->config->embedding + "/" + js["name"].get<std::string>());
        }
        else
        {
            // no type
            delete ditem;
            return;
        }
    }
    else
    {
        delete ditem;
        return;
    }

    ditem->state = CivitAi::DownloadItemState::PENDING;
    if (js.contains("downloadUrl") && !js["downloadUrl"].is_null())
    {
        ditem->url = js["downloadUrl"].get<std::string>();
    }
    else
    {
        delete ditem;
        return;
    }
    if (js.contains("downloadUrl") && !js["downloadUrl"].is_null())
    {
        ditem->url = js["downloadUrl"].get<std::string>();
    }
    else
    {
        delete ditem;
        return;
    }

    ditem->local_file = path;
    ditem->tmp_name = std::filesystem::path(path).replace_extension(".download").string();
    ditem->targetSize = js["sizeKB"].get<double>() * 1024;
    this->SendThreadEvent("DOWNLOAD_ADD", ditem);
}

void MainWindowCivitAiWindow::m_model_filelistOnDataViewListCtrlSelectionChanged(wxDataViewEvent &event)
{
    if (this->m_model_filelist->GetSelectedRow() >= 0)
    {
        this->m_model_download->Enable();
    }
    else
    {
        this->m_model_download->Disable();
    }
}

void MainWindowCivitAiWindow::OnThreadMessage(wxThreadEvent &e)
{
    if (e.GetSkipped() == false)
    {
        e.Skip();
    }
    auto msg = e.GetString().ToStdString();
    std::string token = msg.substr(0, msg.find(":"));
    std::string content = msg.substr(msg.find(":") + 1);

    if (token == "ERROR")
    {
        wxMessageBox(wxString::Format(_("Error on receiving list: %s"), content));
        this->m_statusBar2->SetStatusText(wxString::Format(_("Error on receiving list: %s"), content));
    }
    if (token == "JSON")
    {
        this->m_statusBar2->SetStatusText(_("Loading response, please wait..."));
        this->current_json_text = content;
        this->JsonToTable(content);
    }
    if (token == "IMGDONE")
    {
        this->showImages(e.GetInt(), true);
        return;
    }
    if (token == "DOWNLOAD_ADD")
    {
        auto item = e.GetPayload<CivitAi::DownloadItem *>();
        bool start = true;
        for (auto job : this->downloads)
        {
            if (job->state == CivitAi::DownloadItemState::DOWNLOADING)
            {
                start = false;
            }
        }
        if (start)
        {
            item->state = CivitAi::DownloadItemState::DOWNLOADING;
        }
        else
        {
            item->state = CivitAi::DownloadItemState::PENDING;
        }

        this->downloads.emplace_back(item);
        // add to the table
        auto store = this->m_downloads->GetStore();
        wxVector<wxVariant> data;

        data.push_back(wxVariant(std::filesystem::path(item->local_file).filename().string()));
        auto s = sd_gui_utils::humanReadableFileSize(item->targetSize);
        data.push_back(wxString::Format("%.2f %s", s.first, s.second));
        data.push_back(wxVariant(CivitAi::DownloadItemStateNames[item->state]));
        data.push_back(wxVariant(0));

        this->m_downloads->AppendItem(data, (wxUIntPtr)item);
        this->m_downloads->Refresh();
        if (start)
        {
            // if we start, because no other
            item->timer = new wxTimer();
            item->timer->SetClientData((void *)item);
            item->timer->Bind(wxEVT_TIMER, &MainWindowCivitAiWindow::OnTimer, this);
            item->timer->Start(500);
            this->modelDownloadThreads.emplace_back(new std::thread(&MainWindowCivitAiWindow::modelDownloadThread, this, item));
        }
        return;
    }
    if (token == "DOWNLOAD_FINISH")
    {
        auto payload = e.GetPayload<CivitAi::DownloadItem *>();
        payload->state = CivitAi::DownloadItemState::FINISHED;
        payload->timer->Stop();
        delete payload->timer;
        int progressCol = this->m_downloads->GetColumnCount() - 1;
        auto store = this->m_downloads->GetStore();
        for (int _i = 0; _i < this->m_downloads->GetItemCount(); ++_i)
        {
            auto item = this->m_downloads->RowToItem(_i);
            if (item == nullptr)
            {
                continue;
            }

            CivitAi::DownloadItem *itemData = reinterpret_cast<CivitAi::DownloadItem *>(this->m_downloads->GetItemData(item));
            if (itemData->local_file == payload->local_file)
            {

                store->SetValueByRow(100, _i, progressCol);
                store->SetValueByRow(CivitAi::DownloadItemStateNames[payload->state], _i, progressCol - 1);
                this->m_downloads->Refresh();
                break;
            }
        }
        this->JsonToTable(this->current_json_text);

        // start the next if we have in the list
        for (auto &job : this->downloads)
        {
            if (job->state == CivitAi::DownloadItemState::PENDING)
            {
                job->timer = new wxTimer();
                job->timer->SetClientData((void *)job);
                job->timer->Bind(wxEVT_TIMER, &MainWindowCivitAiWindow::OnTimer, this);
                job->timer->Start(500);
                job->state = CivitAi::DownloadItemState::DOWNLOADING;
                this->modelDownloadThreads.emplace_back(new std::thread(&MainWindowCivitAiWindow::modelDownloadThread, this, job));
                break;
            }
        }

        return; // start new one if pending
    }

    // update table
    if (token == "DOWNLOAD_UPDATE")
    {
        auto payload = e.GetPayload<CivitAi::DownloadItem *>();
        int progressCol = this->m_downloads->GetColumnCount() - 1;
        auto store = this->m_downloads->GetStore();
        for (int _i = 0; _i < this->m_downloads->GetItemCount(); ++_i)
        {
            auto item = this->m_downloads->RowToItem(_i);
            if (item == nullptr)
            {
                continue;
            }

            CivitAi::DownloadItem *itemData = reinterpret_cast<CivitAi::DownloadItem *>(this->m_downloads->GetItemData(item));
            if (itemData->local_file == payload->local_file)
            {
                int current_progress = static_cast<int>((payload->downloadedSize * 100) / payload->targetSize);
                wxString size;
                auto target_s = sd_gui_utils::humanReadableFileSize(payload->targetSize);
                auto current_s = sd_gui_utils::humanReadableFileSize(payload->downloadedSize);
                size = wxString::Format("%.2f %s / %.2f %s", target_s.first, target_s.second, current_s.first, current_s.second);
                store->SetValueByRow(size, _i, progressCol - 2);                                            // size
                store->SetValueByRow(CivitAi::DownloadItemStateNames[payload->state], _i, progressCol - 1); // state
                store->SetValueByRow(current_progress, _i, progressCol);                                    // progress
                this->m_downloads->Refresh();
                break;
            }
        }

        //
        return;
    }
    if (token == "DOWNLOAD_ERROR")
    {
        auto payload = e.GetPayload<CivitAi::DownloadItem *>();
        payload->timer->Stop();
        payload->state = CivitAi::DownloadItemState::DOWNLOAD_ERROR;

        // update info in table
        int last_col = this->m_downloads->GetColumnCount() - 1;
        auto store = this->m_downloads->GetStore();
        for (int _i = 0; _i < this->m_downloads->GetItemCount(); ++_i)
        {
            auto item = this->m_downloads->RowToItem(_i);
            if (item == nullptr)
            {
                continue;
            }

            CivitAi::DownloadItem *itemData = reinterpret_cast<CivitAi::DownloadItem *>(this->m_downloads->GetItemData(item));
            if (itemData->local_file == payload->local_file)
            {
                int current_progress = static_cast<int>((payload->downloadedSize * 100) / payload->targetSize);
                store->SetValueByRow(current_progress, _i, last_col);
                store->SetValueByRow(payload->error, _i, last_col - 1);
                break;
            }
        }

        return;
    }

    this->m_search->Enable(true);
    this->isLoading = false;
}

void MainWindowCivitAiWindow::SendThreadEvent(wxString payload)
{
    wxThreadEvent *event = new wxThreadEvent();
    event->SetString(payload);
    wxQueueEvent(this->GetEventHandler(), event);
}

void MainWindowCivitAiWindow::showImages(int version_id, bool from_thread)
{

    // only updat, if already selected and got thread info about download finish

    if (version_id == this->current_version_id && from_thread == true)
    {
        for (auto &bm : this->bitmaps)
        {
            auto img = static_cast<CivitAi::PreviewImage *>(bm->GetClientData());
            if (img->downloaded && std::filesystem::exists(img->localpath))
            {
                auto resized = sd_gui_utils::cropResizeImage(img->localpath, 200, 200, wxColour(0, 0, 0), this->config->thumbs_path);
                bm->SetBitmap(resized);
            }
        }
    }
    // repopulate with downloaded images  selection changed
    if (version_id != this->current_version_id && from_thread == false)
    {
        for (auto &bm : this->bitmaps)
        {
            bm->Destroy();
        }
        this->bitmaps.clear();

        for (auto &img : this->previewImages)
        {
            if (img.second->downloaded && std::filesystem::exists(img.second->localpath))
            {
                auto resized = sd_gui_utils::cropResizeImage(img.second->localpath, 200, 200, wxColour(0, 0, 0), this->config->thumbs_path);
                wxStaticBitmap *bitmap = new wxStaticBitmap(m_scrolledWindow4, wxID_ANY, resized, wxDefaultPosition, wxSize(200, 200), 0);
                bitmap->SetClientData((void *)img.second);
                image_container->Add(bitmap, 0, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 1);
                this->bitmaps.emplace_back(bitmap);
            }
            else
            {

                wxStaticBitmap *bitmap = new wxStaticBitmap(m_scrolledWindow4, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(200, 200), 0);
                bitmap->SetClientData((void *)img.second);
                image_container->Add(bitmap, 0, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 1);
                this->bitmaps.emplace_back(bitmap);
            }
        }
        this->current_version_id = version_id;
    }

    this->image_container->Layout();
    this->m_scrolledWindow4->Layout();
    this->m_scrolledWindow4->FitInside();
}

void MainWindowCivitAiWindow::SendThreadEvent(std::string payload)
{
    this->SendThreadEvent(wxString(payload));
}

void MainWindowCivitAiWindow::loadImages(nlohmann::json js)
{
    for (auto &del : this->previewImages)
    {
        delete del.second;
    }
    this->previewImages.clear();
    if (js.contains("images") && !js["images"].is_null())
    {

        for (auto img : js["images"])
        {
            int img_id = img["id"].get<int>();
            bool downloaded = false;
            std::filesystem::path path(this->config->tmppath + "/" + std::to_string(img_id));

            auto png = path.replace_extension("png").string();
            auto jpg = path.replace_extension("jpg").string();
            std::string local_path = path.string();

            if (std::filesystem::exists(png))
            {
                local_path = png;
                downloaded = true;
            }
            if (std::filesystem::exists(jpg))
            {
                local_path = jpg;
                downloaded = true;
            }

            CivitAi::PreviewImage *pmage = new CivitAi::PreviewImage();
            pmage->downloaded = downloaded;
            pmage->id = img_id;
            pmage->localpath = local_path;
            pmage->url = img["url"].get<std::string>();

            this->previewImages[pmage->id] = pmage;
            if (pmage->downloaded == false)
            {
                this->imgDownloadThreads.emplace_back(new std::thread(&MainWindowCivitAiWindow::imgDownloadThread, this, pmage, js["id"].get<int>()));
            }
        }
    }
    this->showImages(js["id"].get<int>());
}

void MainWindowCivitAiWindow::populateVersions(nlohmann::json js)
{
    if (!js.contains("modelVersions"))
    {
        return;
    }
    wxVector<wxVariant> data;
    auto store = this->m_model_details->GetStore();
    int currentRow = 0;

    for (auto modelVersion : js["modelVersions"])
    {

        if (modelVersion.contains("name") && !modelVersion["name"].is_null())
        {
            data.push_back(modelVersion["name"].get<std::string>());
        }
        else
        {
            data.push_back("");
        }

        if (modelVersion.contains("baseModel") && !modelVersion["baseModel"].is_null())
        {
            data.push_back(modelVersion["baseModel"].get<std::string>());
        }
        else
        {
            data.push_back("");
        }

        if (modelVersion.contains("baseModelType") && !modelVersion["baseModelType"].is_null())
        {
            data.push_back(modelVersion["baseModelType"].get<std::string>());
        }
        else
        {
            data.push_back("");
        }

        if (modelVersion.contains("publishedAt") && !modelVersion["publishedAt"].is_null())
        {
            data.push_back(modelVersion["publishedAt"].get<std::string>());
        }
        else
        {
            data.push_back("");
        }
        if (modelVersion.contains("stats"))
        {
            auto stats = modelVersion["stats"];
            if (stats.contains("downloadCount") && !stats["downloadCount"].is_null())
            {
                data.push_back(wxString::Format("%d", stats["downloadCount"].get<int>()));
            }
            else
            {
                data.push_back("");
            }
        }
        else
        {
            data.push_back("");
        }

        if (this->CheckIfModelDownloaded(modelVersion))
        {
            data.push_back(_("Local available"));
        }
        else
        {
            data.push_back("");
        }

        this->m_model_details->AppendItem(data);
        auto item = store->GetItem(currentRow);
        auto _mdJson = new nlohmann::json(modelVersion);
        this->loadedJsonDatas.emplace_back(_mdJson);
        store->SetItemData(item, (wxUIntPtr)_mdJson);
        data.clear();
        currentRow++;
    }
    this->m_model_details->Refresh();

    if (js.contains("description") && !js["description"].is_null())
    {
        this->m_model_description->SetPage(js["description"].get<std::string>());
    }
    else
    {
        this->m_model_description->SetPage("");
    }
}

void MainWindowCivitAiWindow::imgDownloadThread(CivitAi::PreviewImage *previewImage, int versionId)
{

    // get apikey from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.Load(SD_GUI_HOMEPAGE, username, password))
    {
        apikey = password.GetAsString();
    }

    // get http
    std::list<std::string> headers;
    headers.push_back("User-Agent: " + std::string(SD_CURL_USER_AGENT));

    if (!apikey.empty())
    {
        headers.push_back("Authorization: Bearer " + apikey.ToStdString());
    }

    std::ostringstream response(std::stringstream::binary);
    try
    {
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy request;
        // request.setOpt(new curlpp::options::HttpHeader(headers));
        request.setOpt(new curlpp::options::WriteStream(&response));
        request.setOpt<curlpp::options::Url>(previewImage->url);
        request.perform();
        std::string target_path = std::filesystem::path(previewImage->localpath).replace_extension("tmp").generic_string();
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
                previewImage->localpath = new_path;
            }
            if (_tmpImg.GetType() == wxBITMAP_TYPE_PNG)
            {
                std::string new_path = std::filesystem::path(target_path).replace_extension(".png").generic_string();
                std::filesystem::rename(target_path, new_path);
                previewImage->localpath = new_path;
            }
            previewImage->downloaded = true;
            wxSleep(2);
            this->SendThreadEvent("IMGDONE", versionId, previewImage);
        }
        _tmpImg.Destroy();
    }

    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
        // MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, img.url);
    }

    catch (curlpp::LogicError &f)
    {
        std::cout << f.what() << std::endl;
        // MainWindowUI::SendThreadEvent(eventHandler, sd_gui_utils::MODEL_INFO_DOWNLOAD_IMAGE_FAILED, modelinfo, img.url);
    }
}

void MainWindowCivitAiWindow::modelDownloadThread(CivitAi::DownloadItem *item)
{

    // get apikey from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.Load(SD_GUI_HOMEPAGE, username, password))
    {
        apikey = password.GetAsString();
    }

    // get http
    std::list<std::string> headers;
    headers.push_back("User-Agent: " + std::string(SD_CURL_USER_AGENT));

    if (!apikey.empty())
    {
        headers.push_back("Authorization: Bearer " + apikey.ToStdString());
    }
    
    std::string target_path = std::filesystem::path(item->tmp_name).generic_string();
    item->tmp_name = target_path;
    item->local_file = std::filesystem::path(item->local_file).generic_string();
    item->file = new std::ofstream(target_path, std::ios::binary);

    std::ostringstream response(std::stringstream::binary);
    try
    {
        curlpp::Cleanup myCleanup;
        curlpp::Easy request;

        request.setOpt(new curlpp::options::WriteStream(&response));
        request.setOpt(new curlpp::options::FollowLocation(true));

        curlpp::options::WriteFunctionCurlFunction myFunction(&MainWindowCivitAiWindow::curlCppWriteDataToFile);
        curlpp::OptionTrait<void *, CURLOPT_WRITEDATA> myData(item);
        request.setOpt(myFunction);
        request.setOpt(myData);

        request.setOpt<curlpp::options::Url>(item->url);
        request.perform();
        item->file->close();
        std::filesystem::rename(target_path, item->local_file);
        this->SendThreadEvent("DOWNLOAD_FINISH", item);
    }

    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
        item->error = e.what();
        this->SendThreadEvent("DOWNLOAD_ERROR", item);
    }
    catch (curlpp::LogicError &f)
    {
        std::cout << f.what() << std::endl;
        item->error = f.what();
        this->SendThreadEvent("DOWNLOAD_ERROR", item);
    }
    if (item->file->is_open())
    {
        item->file->close();
    }
    delete item->file;
}

bool MainWindowCivitAiWindow::CheckIfModelDownloaded(nlohmann::json item)
{

    if (item.contains("modelVersions") && item["modelVersions"].is_array())
    {
        for (auto versions : item["modelVersions"])
        {
            if (this->CheckIfModelDownloaded(versions))
            {
                return true;
            }
        }
        return false;
    }

    if (item.contains("files"))
    {
        for (auto file : item["files"])
        {
            if (this->CheckIfModelDownloaded(file))
            {
                return true;
            }
        }
        return false;
    }
    if (item.contains("hashes") && !item["hashes"].is_null())
    {
        auto hash = item["hashes"];
        if (hash.contains("SHA256") && !hash["SHA256"].is_null())
        {
            auto model = this->ModelInfoManager->getIntoPtrByHash(sd_gui_utils::tolower(hash["SHA256"].get<std::string>()));
            if (model != nullptr)
            {
                if (std::find(this->localAvailableFiles.begin(), this->localAvailableFiles.end(), model) == this->localAvailableFiles.end())
                {
                    this->localAvailableFiles.emplace_back(model);
                }
                return true;
            }
        }
    }
    return false;
}

void MainWindowCivitAiWindow::JsonToTable(std::string json_str)
{

    nlohmann::json json_data = nlohmann::json::parse(json_str);
    if (!json_data.contains("items"))
    {
        wxMessageBox(_("Error on parsing response!"));
        return;
    }

    this->m_statusBar2->SetStatusText(_("Parsing items"));

    this->m_dataViewListCtrl5->DeleteAllItems();

    auto store = this->m_dataViewListCtrl5->GetStore();

    wxVector<wxVariant> data;
    int index = 0;
    nlohmann::json firstitem = NULL;
    for (auto it = json_data["items"].begin(); it != json_data["items"].end(); ++it)
    {

        if (firstitem == NULL)
        {
            firstitem = (*it);
        }
        nlohmann::json *_json = new nlohmann::json((*it));
        bool localAvailable = this->CheckIfModelDownloaded(*_json);
        wxDataViewIconText icont;

        if ((*it).contains("name") && !(*it).is_null())
        {
            data.push_back((*it)["name"].get<std::string>());
        }
        else
        {
            data.push_back(_("N/A"));
        }
        if (localAvailable)
        {
            data.push_back(wxVariant(_("Local available")));
        }
        else
        {
            data.push_back(wxVariant("--"));
        }
        if ((*it).contains("type") && (*it)["type"].is_string())
        {
            data.push_back(wxVariant((*it)["type"].get<std::string>()));
        }
        else
        {
            data.push_back(wxVariant("--"));
        }
        if ((*it).contains("stats"))
        {
            auto stats = (*it)["stats"];
            if (stats.contains("downloadCount"))
            {
                data.push_back(wxString::Format("%d", stats["downloadCount"].get<int>()));
            }
            else
            {
                data.push_back(wxVariant("--"));
            }
        }
        else
        {
            data.push_back(wxVariant("--"));
        }

        store->AppendItem(data, (wxUIntPtr)_json);
        this->modelDetailData.emplace_back(_json);
        data.clear();
        index++;
    }
    this->m_dataViewListCtrl5->Refresh();
    this->m_statusBar2->SetStatusText("");
    if (firstitem != NULL)
    {
        this->m_dataViewListCtrl5->SelectRow(0);
        this->populateVersions(firstitem);
    }
}

void MainWindowCivitAiWindow::OnSearch(std::string query)
{
    if (query.empty())
    {
        return;
    }
    query = CivitAi::urlEncode(query);
    std::string types = "&types=";
    auto selected = this->m_model_type->GetSelection();

    switch (selected)
    {
    case 0: // CHECKPOINTS
        types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::Checkpoint]);
        break;
    case 1: // LORA
        types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::LORA]);
        break;
    case 2: // EMBEDD
        types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::TextualInversion]);
        break;
    default: // default is the checkpints
        types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::Checkpoint]);
        break;
    }
    query = query + types;
    this->infoDownloadThread.emplace_back(new std::thread(&MainWindowCivitAiWindow::civitSearchThread, this, query));
    this->m_search->Enable(false);
    this->isLoading = true;
    this->m_statusBar2->SetStatusText(_("Loading, please wait..."));
}

void MainWindowCivitAiWindow::SendThreadEvent(std::string str, int id, CivitAi::PreviewImage *payload)
{
    wxThreadEvent *event = new wxThreadEvent();
    event->SetPayload(payload);
    event->SetString(str);
    event->SetInt(id);
    wxQueueEvent(this->GetEventHandler(), event);
}

void MainWindowCivitAiWindow::OnTimer(wxTimerEvent &e)
{
    auto timer = (wxTimer *)e.GetEventObject();
    auto item = reinterpret_cast<CivitAi::DownloadItem *>(timer->GetClientData());
    if (item->state == CivitAi::DownloadItemState::DOWNLOAD_ERROR)
    {
        item->timer->Stop();
        return;
    }
    this->SendThreadEvent("DOWNLOAD_UPDATE", item);
}

MainWindowCivitAiWindow::~MainWindowCivitAiWindow()
{
    for (auto &thread : this->infoDownloadThread)
    {
        if (thread->joinable())
        {
            thread->join();
        }
        delete thread;
    }
    this->infoDownloadThread.clear();

    for (auto &mdd : this->modelDetailData)
    {
        delete mdd;
    }
    this->modelDetailData.clear();

    for (auto &json : this->loadedJsonDatas)
    {
        delete json;
    }
    this->loadedJsonDatas.clear();

    for (auto &d : this->downloads)
    {
        delete d;
    }
    this->downloads.clear();
}

void MainWindowCivitAiWindow::civitSearchThread(std::string query)
{

    // get apikey from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;

    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.Load(SD_GUI_HOMEPAGE, username, password))
    {
        apikey = password.GetAsString();
    }

    // get http
    std::list<std::string> headers;
    headers.push_back("Content-Type: application/json;");
    headers.push_back("User-Agent: " + std::string(SD_CURL_USER_AGENT));

    if (!apikey.empty())
    {
        headers.push_back("Authorization: Bearer " + apikey.ToStdString());
    }

    std::string url = "https://civitai.com/api/v1/models?limit=20&query=" + query;
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
        this->SendThreadEvent(wxString::Format("JSON:%s", response.str()));
        return;
    }

    catch (curlpp::RuntimeError &e)
    {
        std::cerr << e.what() << std::endl;
        this->SendThreadEvent(wxString::Format("ERROR:%s", e.what()));
        return;
    }

    catch (curlpp::LogicError &e2)
    {
        std::cerr << e2.what() << std::endl;
        this->SendThreadEvent(wxString::Format("ERROR:%s", e2.what()));
        return;
    }
    // get http
}

void MainWindowCivitAiWindow::SetModelManager(ModelInfo::Manager *manager)
{
    this->ModelInfoManager = manager;
}

void MainWindowCivitAiWindow::populateFiles(nlohmann::json js)
{
    if (js.contains("description") && !js["description"].is_null())
    {
        this->m_model_version_description->SetPage(js["description"].get<std::string>());
    }
    else
    {
        this->m_model_version_description->SetPage("");
    }

    if (js.contains("files") && !js["files"].is_null())
    {
        for (auto file : js["files"])
        {
            wxVector<wxVariant> data;
            if (file.contains("name") && file["name"].is_string())
            {
                data.push_back(file["name"].get<std::string>());
            }
            else
            {
                data.push_back(_("N/A"));
            }
            if (file.contains("type") && file["type"].is_string())
            {
                data.push_back(file["type"].get<std::string>());
            }
            if (file.contains("metadata") && !file["metadata"].is_null())
            {
                auto metadata = file["metadata"];
                if (metadata.contains("format") && !metadata["format"].is_null())
                {
                    data.push_back(metadata["format"].get<std::string>());
                }
                else
                {
                    data.push_back(metadata["format"].get<std::string>());
                }
                if (metadata.contains("size") && metadata["size"].is_string())
                {
                    data.push_back(metadata["size"].get<std::string>());
                }
                else
                {
                    data.push_back(_("N/A"));
                }
                if (metadata.contains("fp") && metadata["fp"].is_string())
                {
                    data.push_back(metadata["fp"].get<std::string>());
                }
                else
                {
                    data.push_back(_("N/A"));
                }
            }
            if (this->CheckIfModelDownloaded(file))
            {
                data.push_back(_("Local available"));
            }
            else
            {
                data.push_back("");
            }
            if (file.contains("sizeKB") && file["sizeKB"].is_number_float())
            {
                auto size = sd_gui_utils::humanReadableFileSize(file["sizeKB"].get<double>() * 1024);
                data.push_back(wxString::Format("%.2f %s", size.first, size.second));
            }
            else
            {
                data.push_back("");
            }
            auto _js = new nlohmann::json(file);
            modelDetailData.emplace_back(_js);
            this->m_model_filelist->AppendItem(data, (wxUIntPtr)_js);
            data.clear();
        }
        this->m_model_filelist->Refresh();
    }
}

void MainWindowCivitAiWindow::SetCfg(sd_gui_utils::config *cfg)
{
    this->config = cfg;
}