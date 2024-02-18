#include "MainWindowModelinfo.h"

MainWindowModelinfo::MainWindowModelinfo(wxWindow *parent)
    : Modelinfo(parent)
{

    auto bitmap = app_png_to_wx_bitmap();

    wxIcon icon;
    icon.CopyFromBitmap(bitmap);
    this->SetIcon(icon);
    wxPersistentRegisterAndRestore(this, this->GetName());

    Bind(wxEVT_SHOW,
         [this](wxShowEvent &)
         {
             this->data2Gui();
         });
}

void MainWindowModelinfo::data2Gui()
{
    if (!std::filesystem::exists(this->infofile))
    {
        return;
    }
    std::ifstream ifs(this->infofile);
    nlohmann::json jf = nlohmann::json::parse(ifs);
    if (jf.contains("model"))
    {

        /// prepare images
        if (jf.contains("images") && this->images.size() == 0)
        {
            this->loadImages(jf);
        }

        if (jf["model"]["name"].is_string())
        {
            std::string full_name = fmt::format("{} {}",
                                                jf["model"]["name"].get<std::string>(),
                                                jf["name"]
                                                    .get<std::string>());
            this->SetTitle(fmt::format("Model details - {}", full_name));

            this->m_static_model_name->SetLabel(full_name);
        }
        else
        {
            this->m_static_model_name->SetLabel(this->ModelInfo->name);
        }

        this->m_static_model_type->SetLabel(jf["model"]["type"].get<std::string>());
        this->m_static_model_nsfw->SetLabel(jf["model"]["nsfw"].get<bool>() == true ? "NSFW" : "SFW");
        this->m_static_model_created_at->SetLabel(jf["createdAt"].get<std::string>());
        this->m_static_model_basemodel->SetLabel(jf["baseModel"].get<std::string>());
        this->m_static_model_basemodeltype->SetLabel(jf["baseModelType"].get<std::string>());
        if (jf["description"].is_null())
        {
            this->m_htmlWin1->SetPage(wxString("<b><i>No description...</i></b>"));
        }
        else
        {
            this->m_htmlWin1->SetPage(jf["description"].get<std::string>());
        }
        if (this->images.size() > 0)
        {
            int maxWidth = 0;
            for (auto img : this->images)
            {
                wxStaticBitmap *bmap = new wxStaticBitmap(this->m_scrolledWindow3, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);
                bmap->SetSize(img.GetSize());
                bmap->SetBitmap(img);
                this->poster_holder->Add(bmap, 0, wxALIGN_LEFT | wxALIGN_TOP, 0);
                maxWidth = maxWidth < img.GetWidth() ? img.GetWidth() : maxWidth;
            }
            this->m_scrolledWindow3->SetSize(maxWidth, this->m_scrolledWindow3->GetSize().GetHeight());
            this->m_scrolledWindow3->Refresh();
            this->Refresh();
            this->Layout();
        }
    }
}

void MainWindowModelinfo::loadImages(nlohmann::json jf)
{
    /// get images ... if there
    int index = 0;
    // get and set images
    if (jf.contains("images"))
    {
        for (auto it = jf["images"].begin(); it != jf["images"].end(); ++it)
        {
            if ((*it).contains("url") && (*it).contains("type"))
            {
                if ((*it)["type"].get<std::string>() == "image")
                {
                    std::string filename = this->datapath + "/" + this->ModelInfo->sha256 + "_" + std::to_string(index) + ".jpeg";
                    if (!std::filesystem::exists(filename))
                    {
                        this->images.emplace_back(this->FetchImage((*it)["url"].get<std::string>(), filename));
                    }
                    else
                    {
                        wxImage img;
                        if (img.LoadFile(filename))
                        {
                            this->images.emplace_back(img);
                        }
                    }
                    index++;
                }
            }
        }
    }
}

std::string MainWindowModelinfo::FetchData(std::string hash)
{
    std::list<std::string> headers;
    headers.push_back("Content-Type: text/json;");
    std::string url = this->url_civitai_base + this->url_model_info + hash;
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
        return std::string(response.str());
    }

    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
        wxMessageDialog dialog(this, e.what());
    }

    catch (curlpp::LogicError &e)
    {
        wxMessageDialog dialog(this, e.what());
    }
    return std::string(response.str());
}

wxImage MainWindowModelinfo::FetchImage(std::string url, std::string target)
{

    // std::list<std::string> headers;
    // headers.push_back("Content-Type: text/json;");
    std::ostringstream response(std::stringstream::binary);
    try
    {
        // That's all that is needed to do cleanup of used resources (RAII style).
        curlpp::Cleanup myCleanup;

        // Our request to be sent.
        curlpp::Easy request;
        // request.setOpt(new curlpp::options::HttpHeader(headers));
        request.setOpt(new curlpp::options::WriteStream(&response));
        request.setOpt<curlpp::options::Url>(url);
        request.perform();

        std::ofstream file(target, std::ios::binary);
        file << response.str();
        file.close();
        wxImage img;
        img.LoadFile(target);
        return img;
    }

    catch (curlpp::RuntimeError &e)
    {
        std::cout << e.what() << std::endl;
        wxMessageDialog dialog(this, e.what());
        return wxImage();
    }

    catch (curlpp::LogicError &e)
    {
        wxMessageDialog dialog(this, e.what());
        return wxImage();
    }
    return wxImage();
}

bool MainWindowModelinfo::SetData(ModelInfo::Manager *ModelManager, std::string model_path, std::string datapath)
{
    this->ModelInfo = ModelManager->getIntoPtr(model_path);
    this->datapath = datapath;
    if (!std::filesystem::exists(this->datapath))
    {
        std::filesystem::create_directories(this->datapath);
    }

    this->infofile = std::filesystem::path(this->datapath + "/" + this->ModelInfo->sha256 + ".json").string();

    if (std::filesystem::exists(this->infofile))
    {
        return true;
    }
    std::string plain_json = this->FetchData(this->ModelInfo->sha256);
    nlohmann::json jf = nlohmann::json::parse(plain_json);
    if (jf.contains("error"))
    {
        wxMessageBox(jf.at("error").get<std::string>());

        return false;
    }
    else
    {
        std::ofstream file(this->infofile);
        file << jf;
        file.close();
        this->loadImages(jf);
    }
    return true;
}