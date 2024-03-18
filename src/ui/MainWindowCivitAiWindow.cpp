#include "MainWindowCivitAiWindow.h"

MainWindowCivitAiWindow::MainWindowCivitAiWindow(wxWindow *parent)
    : CivitAiWindow(parent)
{

    Bind(wxEVT_THREAD, &MainWindowCivitAiWindow::OnThreadMessage, this);
}

void MainWindowCivitAiWindow::m_civitai_searchOnTextEnter(wxCommandEvent &event)
{
    this->OnSearch(this->m_civitai_search->GetValue().ToStdString());
}

void MainWindowCivitAiWindow::m_searchOnButtonClick(wxCommandEvent &event)
{
    // TODO: Implement m_searchOnButtonClick
    this->OnSearch(this->m_civitai_search->GetValue().ToStdString());
}

void MainWindowCivitAiWindow::m_dataViewListCtrl5OnDataViewListCtrlSelectionChanged(wxDataViewEvent &event)
{
    auto selected = this->m_dataViewListCtrl5->GetSelectedRow();

    this->m_model_details->DeleteAllItems();
    this->m_model_filelist->DeleteAllItems();

    if (selected == wxNOT_FOUND)
    {
        return;
    }

    for (auto item : this->loadedJsonDatas)
    {
        nlohmann::json js = *item;
        if (js.contains("index") && js["index"] == selected)
        {
            if (!js.contains("modelVersions"))
            {
                continue;
            }
            wxVector<wxVariant> data;
            for (auto modelVersion : js["modelVersions"])
            {

                data.push_back(modelVersion["name"].get<std::string>());
                data.push_back(modelVersion["baseModel"].get<std::string>());
                data.push_back(modelVersion["baseModelType"].get<std::string>());
                this->m_model_details->AppendItem(data);
                data.clear();
            }
            // populate filelist if selected one
            break;
        }
    }
}

void MainWindowCivitAiWindow::m_model_detailsOnDataViewListCtrlSelectionChanged(wxDataViewEvent &event)
{
    // TODO: Implement m_model_detailsOnDataViewListCtrlSelectionChanged
}

void MainWindowCivitAiWindow::m_model_filelistOnDataViewListCtrlSelectionChanged(wxDataViewEvent &event)
{
    // TODO: Implement m_model_filelistOnDataViewListCtrlSelectionChanged
}

MainWindowCivitAiWindow::~MainWindowCivitAiWindow()
{
    for (auto thread : this->infoDownloadThread)
    {
        if (thread->joinable())
        {
            thread->join();
        }
        thread = NULL;
        delete thread;
    }
    for (auto json : this->loadedJsonDatas)
    {

        json = NULL;
        delete json;
    }
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
    this->m_statusBar2->SetStatusText(_("Loading, please wait..."));
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

        wxDataViewIconText icont;
        data.push_back(wxVariant(icont));

        if ((*it).contains("name") && !(*it).is_null())
        {
            data.push_back((*it)["name"].get<std::string>());
        }
        else
        {
            data.push_back("");
        }
        data.push_back(wxVariant(""));

        store->AppendItem(data);

        data.clear();
        (*it)["index"] = index;
        this->loadedJsonDatas.emplace_back(new nlohmann::json((*it)));
        index++;
    }
    this->m_statusBar2->SetStatusText("");
}