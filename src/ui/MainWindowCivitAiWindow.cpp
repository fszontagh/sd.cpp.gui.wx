#include "MainWindowCivitAiWindow.h"
#include "../helpers/simplecurl.h"
#include "ver.hpp"
#include "wx/string.h"

MainWindowCivitAiWindow::MainWindowCivitAiWindow(wxWindow* parent)
    : CivitAiWindow(parent) {
    Bind(wxEVT_THREAD, &MainWindowCivitAiWindow::OnThreadMessage, this);
}

void MainWindowCivitAiWindow::m_civitai_searchOnTextEnter(wxCommandEvent& event) {
    if (this->isLoading) {
        return;
    }
    this->OnSearch(this->m_civitai_search->GetValue().ToStdString());
    this->m_model_download->Disable();
}

void MainWindowCivitAiWindow::m_searchOnButtonClick(wxCommandEvent& event) {
    if (this->isLoading) {
        return;
    }
    this->OnSearch(this->m_civitai_search->GetValue().ToStdString());
    this->m_model_download->Disable();
}

void MainWindowCivitAiWindow::m_dataViewListCtrl5OnDataViewListCtrlSelectionChanged(wxDataViewEvent& event) {
    auto selected = this->m_dataViewListCtrl5->GetSelectedRow();

    this->m_model_details->DeleteAllItems();
    this->m_model_filelist->DeleteAllItems();
    this->m_model_description->SetPage("");
    this->m_model_version_description->SetPage("");
    for (auto& _d : this->bitmaps) {
        _d->Destroy();
    }
    this->bitmaps.clear();

    this->previewImagesMap.clear();

    if (selected == wxNOT_FOUND) {
        return;
    }
    auto item                 = this->m_dataViewListCtrl5->GetCurrentItem();
    nlohmann::json* json_data = reinterpret_cast<nlohmann::json*>(this->m_dataViewListCtrl5->GetItemData(item));
    nlohmann::json js(*json_data);

    this->populateVersions(js);
}

void MainWindowCivitAiWindow::OnHtmlLinkClicked(wxHtmlLinkEvent& event) {
    wxString question = wxString::Format(_("Do you want to open the URL?\n%s"), event.GetLinkInfo().GetHref());

    wxMessageDialog dialog(
        this,
        question,
        _("Open URL"),
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);

    if (dialog.ShowModal() == wxID_YES) {
        wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
    }
}

void MainWindowCivitAiWindow::m_model_detailsOnDataViewListCtrlSelectionChanged(wxDataViewEvent& event) {
    this->m_model_filelist->DeleteAllItems();
    if (this->m_model_details->GetSelectedRow() == wxNOT_FOUND) {
        return;
    }

    auto item = this->m_model_details->GetCurrentItem();

    nlohmann::json* data = reinterpret_cast<nlohmann::json*>(this->m_model_details->GetItemData(item));
    if (data == nullptr) {
        return;
    }
    nlohmann::json js(*data);
    this->populateFiles(js);
    this->loadImages(js);
}

void MainWindowCivitAiWindow::m_model_downloadOnButtonClick(wxCommandEvent& event) {
    auto selection = this->m_model_filelist->GetSelectedRow();
    if (selection == wxNOT_FOUND) {
        this->m_model_download->Disable();
        return;
    }
    auto item             = this->m_model_filelist->RowToItem(selection);
    nlohmann::json* jsptr = reinterpret_cast<nlohmann::json*>(this->m_model_filelist->GetItemData(item));
    if (jsptr == nullptr) {
        return;
    }
    nlohmann::json js(*jsptr);

    CivitAi::DownloadItem* ditem = new CivitAi::DownloadItem();
    // get the model type...
    auto parentSelection = this->m_dataViewListCtrl5->GetSelectedRow();
    if (parentSelection == wxNOT_FOUND) {
        return;
    }
    auto parentItem              = this->m_dataViewListCtrl5->RowToItem(parentSelection);
    nlohmann::json* parent_jsptr = reinterpret_cast<nlohmann::json*>(this->m_dataViewListCtrl5->GetItemData(parentItem));
    nlohmann::json parent_js(*parent_jsptr);

    wxFileName path = wxFileName(this->config->model + wxFileName::GetPathSeparators() + js["name"].get<std::string>());

    if (parent_js.contains("type") && !parent_js["type"].is_null()) {
        std::string type = parent_js["type"].get<std::string>();
        if (type == CivitAi::ModelTypesNames[CivitAi::ModelTypes::LORA]) {
            ditem->type = CivitAi::ModelTypes::LORA;
            path        = wxFileName(this->config->lora + wxFileName::GetPathSeparators() + js["name"].get<std::string>());
        } else if (type == CivitAi::ModelTypesNames[CivitAi::ModelTypes::Checkpoint]) {
            ditem->type = CivitAi::ModelTypes::Checkpoint;
            path        = wxFileName(this->config->model + wxFileName::GetPathSeparators() + js["name"].get<std::string>());
        } else if (type == CivitAi::ModelTypesNames[CivitAi::ModelTypes::TextualInversion]) {
            ditem->type = CivitAi::ModelTypes::TextualInversion;
            path        = wxFileName(this->config->embedding + wxFileName::GetPathSeparators() + js["name"].get<std::string>());
        } else {
            // no type
            delete ditem;
            return;
        }
    } else {
        delete ditem;
        return;
    }
    /// this is missing from the documentation
    /// @url https://github.com/civitai/civitai/wiki/REST-API-Reference#get-apiv1modelsmodelid
    // example: https://civitai.com/api/v1/models/1102
    if (js.contains("type")) {
        if (js["type"].get<std::string>() == "LORA") {
            path = wxFileName(this->config->lora + wxFileName::GetPathSeparators() + js["name"].get<std::string>());
        }
        if (js["type"].get<std::string>() == "VAE") {
            path = wxFileName(this->config->vae + wxFileName::GetPathSeparators() + js["name"].get<std::string>());
        }
    }

    ditem->state = CivitAi::DownloadItemState::PENDING;
    if (js.contains("downloadUrl") && !js["downloadUrl"].is_null()) {
        ditem->url = js["downloadUrl"].get<std::string>();
    } else {
        delete ditem;
        return;
    }

    ditem->local_file = path.GetAbsolutePath().utf8_string();
    ditem->tmp_name   = std::filesystem::path(ditem->local_file).replace_extension(".download").string();
    ditem->targetSize = js["sizeKB"].get<double>() * 1024;
    if (std::filesystem::exists(ditem->local_file) || std::filesystem::exists(ditem->tmp_name)) {
        wxMessageDialog dialog(this, wxString::Format(_("The file \n'%s'\n already exists.\nDo you want to overwrite it?"), ditem->local_file), _("Download Model"), wxYES_NO | wxICON_QUESTION);
        if (dialog.ShowModal() == wxID_YES) {
            this->SendThreadEvent("DOWNLOAD_ADD", ditem);
            return;
        } else {
            delete ditem;
            return;
        }
    }
    this->SendThreadEvent("DOWNLOAD_ADD", ditem);
}

void MainWindowCivitAiWindow::m_model_filelistOnDataViewListCtrlSelectionChanged(wxDataViewEvent& event) {
    if (this->m_model_filelist->GetSelectedRow() >= 0) {
        this->m_model_download->Enable();
    } else {
        this->m_model_download->Disable();
    }
}

void MainWindowCivitAiWindow::OnThreadMessage(wxThreadEvent& e) {
    if (e.GetSkipped() == false) {
        e.Skip();
    }

    auto msg            = e.GetString().utf8_string();
    std::string token   = msg.substr(0, msg.find(":"));
    std::string content = msg.substr(msg.find(":") + 1);

    if (token == "ERROR") {
        wxMessageBox(wxString::Format(_("Error on receiving list: %s"), content));
        this->m_statusBar2->SetStatusText(wxString::Format(_("Error on receiving list: %s"), content));
    }
    if (token == "JSON") {
        this->m_statusBar2->SetStatusText(_("Loading response, please wait..."));
        this->current_json_text = content;
        this->JsonToTable(content);
    }
    if (token == "IMGDONE") {
        this->showImages(e.GetInt(), true);
        return;
    }
    if (token == "DOWNLOAD_ADD") {
        auto item  = e.GetPayload<CivitAi::DownloadItem*>();
        bool start = true;
        for (auto job : this->downloads) {
            if (job->state == CivitAi::DownloadItemState::DOWNLOADING) {
                start = false;
            }
        }
        if (start) {
            item->state = CivitAi::DownloadItemState::DOWNLOADING;
        } else {
            item->state = CivitAi::DownloadItemState::PENDING;
        }

        this->downloads.emplace_back(item);
        // add to the table
        wxVector<wxVariant> data;

        data.push_back(wxVariant(wxString::FromUTF8Unchecked(std::filesystem::path(item->local_file).filename().string())));
        auto s = sd_gui_utils::humanReadableFileSize(item->targetSize);
        data.push_back(wxString::Format("%.2f %s", s.first, s.second));
        data.push_back(wxVariant(wxGetTranslation(CivitAi::DownloadItemStateNames[item->state])));
        data.push_back(wxVariant(0));

        this->m_downloads->AppendItem(data, (wxUIntPtr)item);
        this->m_downloads->Refresh();
        if (start) {
            this->modelDownloadThreads.emplace_back(new std::thread(&MainWindowCivitAiWindow::modelDownloadThread, this, item));
        }
        return;
    }
    if (token == "DOWNLOAD_FINISH") {
        auto payload    = e.GetPayload<CivitAi::DownloadItem*>();
        payload->state  = CivitAi::DownloadItemState::FINISHED;
        int progressCol = this->m_downloads->GetColumnCount() - 1;
        auto store      = this->m_downloads->GetStore();
        for (int _i = 0; _i < this->m_downloads->GetItemCount(); ++_i) {
            auto item = this->m_downloads->RowToItem(_i);
            if (item == nullptr) {
                continue;
            }

            CivitAi::DownloadItem* itemData = reinterpret_cast<CivitAi::DownloadItem*>(this->m_downloads->GetItemData(item));
            if (itemData->local_file == payload->local_file) {
                store->SetValueByRow(100, _i, progressCol);
                store->SetValueByRow(wxGetTranslation(CivitAi::DownloadItemStateNames[payload->state]), _i, progressCol - 1);
                store->RowValueChanged(_i, progressCol - 1);
                this->m_downloads->Refresh();
                break;
            }
        }
        this->JsonToTable(this->current_json_text);

        // start the next if we have in the list
        for (auto& job : this->downloads) {
            if (job->state == CivitAi::DownloadItemState::PENDING) {
                job->state = CivitAi::DownloadItemState::DOWNLOADING;
                this->modelDownloadThreads.emplace_back(new std::thread(&MainWindowCivitAiWindow::modelDownloadThread, this, job));
                break;
            }
        }

        return;  // start new one if pending
    }

    // update table
    if (token == "DOWNLOAD_UPDATE") {
        auto payload    = e.GetPayload<CivitAi::DownloadItem*>();
        int progressCol = this->m_downloads->GetColumnCount() - 1;
        auto store      = this->m_downloads->GetStore();
        for (int _i = 0; _i < this->m_downloads->GetItemCount(); ++_i) {
            auto item = this->m_downloads->RowToItem(_i);
            if (item == nullptr) {
                continue;
            }

            CivitAi::DownloadItem* itemData = reinterpret_cast<CivitAi::DownloadItem*>(this->m_downloads->GetItemData(item));
            if (itemData->local_file == payload->local_file) {
                if (payload->downloadedSize == 0 || payload->targetSize == 0) {
                    store->SetValueByRow(0, _i, progressCol);                                                    // progress
                    store->SetValueByRow(wxGetTranslation(CivitAi::DownloadItemStateNames[payload->state]), _i, progressCol - 1);  // state
                    store->SetValueByRow("", _i, progressCol - 2);                                               // size
                    store->RowValueChanged(_i, progressCol - 2);
                    store->RowValueChanged(_i, progressCol - 1);
                    store->RowValueChanged(_i, progressCol);
                    this->m_downloads->Refresh();
                    break;
                }
                int current_progress = static_cast<int>((payload->downloadedSize * 100) / payload->targetSize);
                wxString size;
                auto target_s  = sd_gui_utils::humanReadableFileSize(payload->targetSize);
                auto current_s = sd_gui_utils::humanReadableFileSize(payload->downloadedSize);
                size           = wxString::Format("%.2f %s / %.2f %s", target_s.first, target_s.second, current_s.first, current_s.second);
                store->SetValueByRow(size, _i, progressCol - 2);                                             // size
                store->SetValueByRow(wxGetTranslation(CivitAi::DownloadItemStateNames[payload->state]), _i, progressCol - 1);  // state
                store->SetValueByRow(current_progress, _i, progressCol);                                     // progress

                store->RowValueChanged(_i, progressCol - 2);
                store->RowValueChanged(_i, progressCol - 1);
                store->RowValueChanged(_i, progressCol);
                this->m_downloads->Refresh();
                break;
            }
        }

        //
        return;
    }
    if (token == "DOWNLOAD_ERROR") {
        auto payload   = e.GetPayload<CivitAi::DownloadItem*>();
        payload->state = CivitAi::DownloadItemState::DOWNLOAD_ERROR;

        // update info in table
        int last_col = this->m_downloads->GetColumnCount() - 1;
        auto store   = this->m_downloads->GetStore();
        for (int _i = 0; _i < this->m_downloads->GetItemCount(); ++_i) {
            auto item = this->m_downloads->RowToItem(_i);
            if (item == nullptr) {
                continue;
            }

            CivitAi::DownloadItem* itemData = reinterpret_cast<CivitAi::DownloadItem*>(this->m_downloads->GetItemData(item));
            if (itemData->local_file == payload->local_file) {
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

void MainWindowCivitAiWindow::SendThreadEvent(wxString payload) {
    wxThreadEvent* event = new wxThreadEvent();
    event->SetString(payload);
    wxQueueEvent(this->GetEventHandler(), event);
}

void MainWindowCivitAiWindow::showImages(int version_id, bool from_thread) {
    // only updat, if already selected and got thread info about download finish

    if (version_id == this->current_version_id && from_thread == true) {
        for (auto& bm : this->bitmaps) {
            auto img = static_cast<CivitAi::PreviewImage*>(bm->GetClientData());
            if (img->downloaded && std::filesystem::exists(img->localpath)) {
                auto resized = sd_gui_utils::cropResizeImage(wxString::FromUTF8Unchecked(img->localpath), 200, 200, wxColour(51, 51, 51, wxALPHA_TRANSPARENT), wxString::FromUTF8Unchecked(this->config->thumbs_path));
                bm->SetBitmap(resized);
            }
        }
    }
    // repopulate with downloaded images  selection changed
    if (version_id != this->current_version_id && from_thread == false) {
        for (auto& bm : this->bitmaps) {
            bm->Destroy();
        }
        this->bitmaps.clear();

        for (auto& img : this->previewImagesMap) {
            if (img.second->downloaded && std::filesystem::exists(img.second->localpath)) {
                auto resized           = sd_gui_utils::cropResizeImage(wxString::FromUTF8Unchecked(img.second->localpath), 200, 200, wxColour(51, 51, 51, wxALPHA_TRANSPARENT), wxString::FromUTF8Unchecked(this->config->thumbs_path));
                wxStaticBitmap* bitmap = new wxStaticBitmap(m_scrolledWindow4, wxID_ANY, resized, wxDefaultPosition, wxSize(200, 200), 0);
                bitmap->SetClientData((void*)img.second.get());
                image_container->Add(bitmap, 0, wxALL | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 1);
                this->bitmaps.emplace_back(bitmap);
            } else {
                wxStaticBitmap* bitmap = new wxStaticBitmap(m_scrolledWindow4, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(200, 200), 0);
                bitmap->SetClientData((void*)img.second.get());
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

void MainWindowCivitAiWindow::SendThreadEvent(std::string payload) {
    wxThreadEvent* event = new wxThreadEvent();
    event->SetString(wxString::FromUTF8(payload));
    wxQueueEvent(this->GetEventHandler(), event);
}

void MainWindowCivitAiWindow::loadImages(nlohmann::json js) {
    this->previewImagesMap.clear();
    if (js.contains("images") && !js["images"].is_null()) {
        for (auto img : js["images"]) {
            int img_id      = img["id"].get<int>();
            bool downloaded = false;

            auto _p = this->config->tmppath;  //+ wxFileName::GetPathSeparator().GetValue() << std::to_string(img_id);
            _p.Append(wxFileName::GetPathSeparators() + std::to_string(img_id));
            std::filesystem::path path(_p.utf8_string());

            auto png               = path.replace_extension("png").string();
            auto jpg               = path.replace_extension("jpg").string();
            std::string local_path = path.string();

            if (std::filesystem::exists(png)) {
                local_path = png;
                downloaded = true;
            }
            if (std::filesystem::exists(jpg)) {
                local_path = jpg;
                downloaded = true;
            }

            auto pmage        = std::make_shared<CivitAi::PreviewImage>();
            pmage->downloaded = downloaded;
            pmage->id         = img_id;
            pmage->localpath  = local_path;
            pmage->url        = img["url"].get<std::string>();

            this->previewImagesMap[pmage->id] = pmage;
            if (pmage->downloaded == false) {
                this->imgDownloadThreads.emplace_back(new std::thread(&MainWindowCivitAiWindow::imgDownloadThread, this, pmage, js["id"].get<int>()));
            }
        }
    }
    this->showImages(js["id"].get<int>());
}

void MainWindowCivitAiWindow::populateVersions(nlohmann::json js) {
    if (!js.contains("modelVersions")) {
        return;
    }
    wxVector<wxVariant> data;
    auto store     = this->m_model_details->GetStore();
    int currentRow = 0;

    for (auto modelVersion : js["modelVersions"]) {
        if (modelVersion.contains("name") && !modelVersion["name"].is_null()) {
            data.push_back(wxString::FromUTF8Unchecked(modelVersion["name"].get<std::string>()));
        } else {
            data.push_back("");
        }

        if (modelVersion.contains("baseModel") && !modelVersion["baseModel"].is_null()) {
            data.push_back(modelVersion["baseModel"].get<std::string>());
        } else {
            data.push_back("");
        }

        if (modelVersion.contains("baseModelType") && !modelVersion["baseModelType"].is_null()) {
            data.push_back(modelVersion["baseModelType"].get<std::string>());
        } else {
            data.push_back("");
        }

        if (modelVersion.contains("publishedAt") && !modelVersion["publishedAt"].is_null()) {
            data.push_back(modelVersion["publishedAt"].get<std::string>());
        } else {
            data.push_back("");
        }
        if (modelVersion.contains("stats")) {
            auto stats = modelVersion["stats"];
            if (stats.contains("downloadCount") && !stats["downloadCount"].is_null()) {
                data.push_back(wxString::Format("%d", stats["downloadCount"].get<int>()));
            } else {
                data.push_back("");
            }
        } else {
            data.push_back("");
        }

        if (this->CheckIfModelDownloaded(modelVersion)) {
            data.push_back(_("Local available"));
        } else {
            data.push_back("");
        }

        this->m_model_details->AppendItem(data);
        auto item    = store->GetItem(currentRow);
        auto _mdJson = new nlohmann::json(modelVersion);
        this->loadedJsonDatas.emplace_back(_mdJson);
        store->SetItemData(item, (wxUIntPtr)_mdJson);
        data.clear();
        currentRow++;
    }
    this->m_model_details->Refresh();

    if (js.contains("description") && !js["description"].is_null()) {
        this->m_model_description->SetPage(wxString::FromUTF8Unchecked(js["description"].get<std::string>()));
    } else {
        this->m_model_description->SetPage("");
    }
}
void MainWindowCivitAiWindow::imgDownloadThread(std::shared_ptr<CivitAi::PreviewImage> previewImage, int versionId) {
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
    headers.push_back("User-Agent: " + std::string(SD_CURL_USER_AGENT));

    if (!apikey.empty()) {
        headers.push_back("Authorization: Bearer " + apikey.ToStdString());
    }

    try {
        sd_gui_utils::SimpleCurl curl;

        // Set callback to write data to file
        std::string target_path = std::filesystem::path(previewImage->localpath).replace_extension("tmp").generic_string();

        curl.getFile(previewImage->url, headers, target_path);
        if (curl.getLastResponseCode() != 200) {
            std::cerr << "Error: " << curl.errorCodeToString() << std::endl;
            return;
        }
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
                previewImage->localpath  = new_path;
                previewImage->downloaded = true;
                wxSleep(2);
                this->SendThreadEvent("IMGDONE", versionId, previewImage);
            }
        }
        _tmpImg.Destroy();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void MainWindowCivitAiWindow::modelDownloadThread(CivitAi::DownloadItem* item) {
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
    headers.push_back("User-Agent: " + std::string(SD_CURL_USER_AGENT));

    if (!apikey.empty()) {
        headers.push_back("Authorization: Bearer " + apikey.ToStdString());
    }

    // Set up paths
    std::string target_path = std::filesystem::path(item->tmp_name).generic_string();
    item->tmp_name          = target_path;
    item->local_file        = std::filesystem::path(item->local_file).generic_string();

    try {
        sd_gui_utils::SimpleCurl curl;

        curl.getFile(item->url, headers, target_path, [this, &item](curl_off_t bytes, curl_off_t total) {
            // item->targetSize     = total;
            item->downloadedSize = bytes;
            this->SendThreadEvent("DOWNLOAD_UPDATE", item);
        });

        // TODO: check response header, if it is json, then we got error (ef the author not allowing to download without login)
        // {"error":"Unauthorized","message":"The creator of this asset requires you to be logged in to download it"}
        if (curl.getLastResponseCode() != 200) {
            item->error = wxString::Format(_("Download failed: %s"), curl.errorCodeToString());
            this->SendThreadEvent("DOWNLOAD_ERROR", item);
            return;
        }
        if (item->targetSize != item->downloadedSize) {
            item->error = _("Download failed");
            this->SendThreadEvent("DOWNLOAD_ERROR", item);
            return;
        }
        // Rename file after download
        std::filesystem::rename(target_path, item->local_file);

        this->SendThreadEvent("DOWNLOAD_FINISH", item);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        item->error = e.what();
        this->SendThreadEvent("DOWNLOAD_ERROR", item);
    }
}

bool MainWindowCivitAiWindow::CheckIfModelDownloaded(nlohmann::json item) {
    if (item.contains("modelVersions") && item["modelVersions"].is_array()) {
        for (auto versions : item["modelVersions"]) {
            if (this->CheckIfModelDownloaded(versions)) {
                return true;
            }
        }
        return false;
    }

    if (item.contains("files")) {
        for (auto file : item["files"]) {
            if (this->CheckIfModelDownloaded(file)) {
                return true;
            }
        }
        return false;
    }
    if (item.contains("hashes") && !item["hashes"].is_null()) {
        auto hash = item["hashes"];
        if (hash.contains("SHA256") && !hash["SHA256"].is_null()) {
            auto model = this->ModelInfoManager->getIntoPtrByHash(sd_gui_utils::tolower(hash["SHA256"].get<std::string>()));
            if (model != nullptr) {
                if (std::find(this->localAvailableFiles.begin(), this->localAvailableFiles.end(), model) == this->localAvailableFiles.end()) {
                    this->localAvailableFiles.emplace_back(model);
                }
                return true;
            }
        }
    }
    return false;
}

void MainWindowCivitAiWindow::JsonToTable(std::string json_str) {
    try {
        nlohmann::json json_data = nlohmann::json::parse(json_str);
        if (!json_data.contains("items")) {
            wxMessageBox(_("Error on parsing response!"));
            this->m_statusBar2->SetStatusText(_("Error on parsing response! (no items in json)"));
            return;
        }
        this->m_statusBar2->SetStatusText(_("Parsing items"));

        this->m_dataViewListCtrl5->DeleteAllItems();

        auto store = this->m_dataViewListCtrl5->GetStore();

        wxVector<wxVariant> data;
        int index                = 0;
        nlohmann::json firstitem = NULL;
        for (auto it = json_data["items"].begin(); it != json_data["items"].end(); ++it) {
            if (firstitem == NULL) {
                firstitem = (*it);
            }
            nlohmann::json* _json = new nlohmann::json((*it));
            bool localAvailable   = this->CheckIfModelDownloaded(*_json);
            wxDataViewIconText icont;

            if ((*it).contains("name") && !(*it).is_null()) {
                data.push_back(wxString::FromUTF8Unchecked((*it)["name"].get<std::string>()));
            } else {
                data.push_back(_("N/A"));
            }
            if (localAvailable) {
                data.push_back(wxVariant(_("Local available")));
            } else {
                data.push_back(wxVariant("--"));
            }
            if ((*it).contains("type") && (*it)["type"].is_string()) {
                data.push_back(wxVariant((*it)["type"].get<std::string>()));
            } else {
                data.push_back(wxVariant("--"));
            }
            if ((*it).contains("stats")) {
                auto stats = (*it)["stats"];
                if (stats.contains("downloadCount")) {
                    data.push_back(wxString::Format("%d", stats["downloadCount"].get<int>()));
                } else {
                    data.push_back(wxVariant("--"));
                }
            } else {
                data.push_back(wxVariant("--"));
            }

            store->AppendItem(data, (wxUIntPtr)_json);
            this->modelDetailData.emplace_back(_json);
            data.clear();
            index++;
        }
        this->m_dataViewListCtrl5->Refresh();
        this->m_statusBar2->SetStatusText("");
        if (firstitem != NULL) {
            this->m_dataViewListCtrl5->SelectRow(0);
            this->populateVersions(firstitem);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        this->m_statusBar2->SetStatusText(_("Error on parsing response: " + std::string(e.what())));
        return;
    }
}

void MainWindowCivitAiWindow::OnSearch(std::string query) {
    if (query.empty()) {
        return;
    }
    query             = CivitAi::urlEncode(query);
    std::string types = "&types=";
    auto selected     = this->m_model_type->GetSelection();

    switch (selected) {
        case 0:  // CHECKPOINTS
            types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::Checkpoint]);
            break;
        case 1:  // LORA
            types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::LORA]);
            break;
        case 2:  // EMBEDD
            types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::TextualInversion]);
            break;
        default:  // default is the checkpints
            types = types + std::string(CivitAi::ModelTypesNames[CivitAi::ModelTypes::Checkpoint]);
            break;
    }
    query = query + types;
    this->infoDownloadThread.emplace_back(new std::thread(&MainWindowCivitAiWindow::civitSearchThread, this, query));
    this->m_search->Enable(false);
    this->isLoading = true;
    this->m_statusBar2->SetStatusText(_("Loading, please wait..."));
}

void MainWindowCivitAiWindow::SendThreadEvent(std::string str, int id, CivitAi::PreviewImage* payload) {
    wxThreadEvent* event = new wxThreadEvent();
    event->SetPayload(payload);
    event->SetString(str);
    event->SetInt(id);
    wxQueueEvent(this->GetEventHandler(), event);
}
void MainWindowCivitAiWindow::SendThreadEvent(std::string str, int id, std::shared_ptr<CivitAi::PreviewImage> payload) {
    wxThreadEvent* event = new wxThreadEvent();
    event->SetPayload(payload);
    event->SetString(str);
    event->SetInt(id);
    wxQueueEvent(this->GetEventHandler(), event);
}

MainWindowCivitAiWindow::~MainWindowCivitAiWindow() {
    for (auto& thread : this->infoDownloadThread) {
        if (thread->joinable()) {
            thread->join();
        }
        delete thread;
    }
    this->infoDownloadThread.clear();

    for (auto& mdd : this->modelDetailData) {
        delete mdd;
    }
    this->modelDetailData.clear();

    for (auto& json : this->loadedJsonDatas) {
        delete json;
    }
    this->loadedJsonDatas.clear();

    for (auto& d : this->downloads) {
        delete d;
    }
    this->downloads.clear();
}

void MainWindowCivitAiWindow::civitSearchThread(const std::string& query) {
    // Get API key from store, if available
    wxString username = "civitai_api_key";
    wxSecretValue password;
    wxString apikey;
    wxSecretStore store = wxSecretStore::GetDefault();

    if (store.Load(wxString::Format(wxT("%s/CivitAiApiKey"), PROJECT_NAME), username, password)) {
        apikey = password.GetAsString();
    }

    // Set up headers
    std::vector<std::string> headers = {
        "Content-Type: application/json;",
        "User-Agent: " + std::string(SD_CURL_USER_AGENT)};

    if (!apikey.empty()) {
        headers.push_back("Authorization: Bearer " + apikey.ToStdString());
    }

    std::string url = "https://civitai.com/api/v1/models?limit=20&query=" + query;

    try {
        // Use the CurlCppWrapper for easier HTTP management
        sd_gui_utils::SimpleCurl curl;

        std::string response;

        curl.get(url, headers, response);

        if (curl.getLastResponseCode() != 200) {
            this->SendThreadEvent(wxString::Format("ERROR:HTTP request failed: %s", curl.errorCodeToString()));
            return;
        }

        // Process the JSON response
        std::string jsonResponse = "JSON:" + response;
        this->SendThreadEvent(jsonResponse);

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        this->SendThreadEvent(wxString::Format("ERROR:%s", e.what()));
    }
}

void MainWindowCivitAiWindow::SetModelManager(std::shared_ptr<ModelInfo::Manager> manager) {
    this->ModelInfoManager = manager;
}

void MainWindowCivitAiWindow::populateFiles(nlohmann::json js) {
    if (js.contains("description") && !js["description"].is_null()) {
        std::string description = js["description"].get<std::string>();
        this->m_model_version_description->SetPage(wxString::FromUTF8Unchecked(description));
    } else {
        this->m_model_version_description->SetPage("");
    }

    if (js.contains("files") && !js["files"].is_null()) {
        for (auto file : js["files"]) {
            wxVector<wxVariant> data;
            if (file.contains("name") && file["name"].is_string()) {
                data.push_back(wxString::FromUTF8Unchecked(file["name"].get<std::string>()));
            } else {
                data.push_back(_("N/A"));
            }
            if (file.contains("type") && file["type"].is_string()) {
                data.push_back(file["type"].get<std::string>());
            }
            if (file.contains("metadata") && !file["metadata"].is_null()) {
                auto metadata = file["metadata"];
                if (metadata.contains("format") && !metadata["format"].is_null()) {
                    data.push_back(metadata["format"].get<std::string>());
                } else {
                    data.push_back(metadata["format"].get<std::string>());
                }
                if (metadata.contains("size") && metadata["size"].is_string()) {
                    data.push_back(metadata["size"].get<std::string>());
                } else {
                    data.push_back(_("N/A"));
                }
                if (metadata.contains("fp") && metadata["fp"].is_string()) {
                    data.push_back(metadata["fp"].get<std::string>());
                } else {
                    data.push_back(_("N/A"));
                }
            }
            if (this->CheckIfModelDownloaded(file)) {
                data.push_back(_("Local available"));
            } else {
                data.push_back("");
            }
            if (file.contains("sizeKB") && file["sizeKB"].is_number_float()) {
                auto size = sd_gui_utils::humanReadableFileSize(file["sizeKB"].get<double>() * 1024);
                data.push_back(wxString::Format("%.2f %s", size.first, size.second));
            } else {
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

void MainWindowCivitAiWindow::SetCfg(sd_gui_utils::config* cfg) {
    this->config = cfg;
}