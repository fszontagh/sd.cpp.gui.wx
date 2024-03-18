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

    if (selected == wxNOT_FOUND)
    {
        return;
    }
    auto item = this->m_dataViewListCtrl5->GetCurrentItem();
    nlohmann::json *json_data = reinterpret_cast<nlohmann::json *>(this->m_dataViewListCtrl5->GetItemData(item));
    nlohmann::json js(*json_data);

    if (!js.contains("modelVersions"))
    {
        return;
    }
    wxVector<wxVariant> data;
    auto store = this->m_model_details->GetStore();
    int currentRow = 0;
    for (auto modelVersion : js["modelVersions"])
    {
        data.push_back(modelVersion["name"].get<std::string>());
        data.push_back(modelVersion["baseModel"].get<std::string>());
        data.push_back(modelVersion["baseModelType"].get<std::string>());
        this->m_model_details->AppendItem(data);
        auto item = store->GetItem(currentRow);
        auto _mdJson = new nlohmann::json(modelVersion);
        this->loadedJsonDatas.emplace_back(_mdJson);
        store->SetItemData(item, (wxUIntPtr)_mdJson);
        data.clear();
        currentRow++;
    }

    if (this->m_model_details->GetItemCount() > 0)
    {
        this->m_model_details->SelectRow(0);
    }

    if (js.contains("description") && !js["description"].is_null())
    {
        this->m_model_description->SetPage(js["description"].get<std::string>());
    }
    else
    {
        this->m_model_description->SetPage("");
    }
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
    nlohmann::json js(*data);
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
            this->m_model_filelist->AppendItem(data);
            data.clear();
        }
        if (this->m_model_filelist->GetItemCount() > 0)
        {
            this->m_model_filelist->SelectRow(0);
        }
    }
}

void MainWindowCivitAiWindow::m_model_filelistOnDataViewListCtrlSelectionChanged(wxDataViewEvent &event)
{
    // TODO: Implement m_model_filelistOnDataViewListCtrlSelectionChanged
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
}

void MainWindowCivitAiWindow::civitSearchThread(std::string query)
{
    // get http
    std::list<std::string> headers;
    headers.push_back("Content-Type: text/json;");
    headers.push_back("User-Agent: " + std::string(SD_CURL_USER_AGENT));
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

        wxThreadEvent *event = new wxThreadEvent();
        event->SetString(wxString::Format("JSON:%s", response.str()));
        wxQueueEvent(this->GetEventHandler(), event);
        return;
    }

    catch (curlpp::RuntimeError &e)
    {
        std::cerr << e.what() << std::endl;

        wxThreadEvent *event2 = new wxThreadEvent();
        event2->SetString(wxString::Format("ERROR:%s", e.what()));
        wxQueueEvent(this->GetEventHandler(), event2);

        return;
    }

    catch (curlpp::LogicError &e2)
    {
        std::cerr << e2.what() << std::endl;

        wxThreadEvent *event2 = new wxThreadEvent();
        event2->SetString(wxString::Format("ERROR:%s", e2.what()));
        wxQueueEvent(this->GetEventHandler(), event2);

        return;
    }
    // get http
}

void MainWindowCivitAiWindow::SetModelManager(ModelInfo::Manager *manager)
{
    this->ModelInfoManager = manager;
}

void MainWindowCivitAiWindow::OnSearch(std::string query)
{
    if (query.empty())
    {
        return;
    }
    std::string types = "&types=";
    auto selected = this->m_model_type->GetSelection();

    switch (selected)
    {
    case 0: // CHECKPOINTS
        types = types + std::string(CivitAi::ModelTypesNames[0]);
        break;
    case 1: // LORA
        types = types + std::string(CivitAi::ModelTypesNames[4]);
        break;
    case 3: // EMBEDD - TextualInversion
        types = types + std::string(CivitAi::ModelTypesNames[1]);
        break;
    default:
        types = types + std::string(CivitAi::ModelTypesNames[0]);
        break;
    }
    query = query + types;
    this->infoDownloadThread.emplace_back(new std::thread(&MainWindowCivitAiWindow::civitSearchThread, this, query));
    this->m_search->Enable(false);
    this->isLoading = true;
    this->m_statusBar2->SetStatusText(_("Loading, please wait..."));
}

bool MainWindowCivitAiWindow::CheckIfModelDownloaded(nlohmann::json item)
{

    if (item.contains("modelVersions") && !item["modelVersions"].is_array())
    {
        for (auto versions : item["modelVersions"])
        {
            if (versions.contains("files"))
            {
                for (auto file : versions["files"])
                {
                    if (file.contains("hashes") && !file["hashes"].is_null())
                    {
                        for (auto hash : file["hashes"])
                        {
                            if (hash.contains("SHA256") && hash["SHA256"].is_string())
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
                    }
                }
            }
        }
    }
    return false;
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
    if ((token == "JSON"))
    {
        this->m_statusBar2->SetStatusText(_("Loading response, please wait..."));
        this->JsonToTable(content);
    }

    this->m_search->Enable(true);
    this->isLoading = false;
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
    for (auto it = json_data["items"].begin(); it != json_data["items"].end(); ++it)
    {

        nlohmann::json *_json = new nlohmann::json((*it));
        bool localAvailable = this->CheckIfModelDownloaded(*_json);
        wxDataViewIconText icont;
        data.push_back(wxVariant(icont));

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
            data.push_back(wxVariant(""));
        }

        store->AppendItem(data, (wxUIntPtr)_json);
        this->modelDetailData.emplace_back(_json);
        data.clear();
        index++;
    }
    this->m_statusBar2->SetStatusText("");
}