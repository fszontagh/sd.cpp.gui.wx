#include "ModelInfo.h"

ModelInfo::Manager::Manager(const wxString& meta_base_path) {
    this->MetaStorePath = meta_base_path + wxFileName::GetPathSeparators() + "modelinfos";

    if (!wxFileName::Exists(this->MetaStorePath)) {
        wxFileName::Mkdir(this->MetaStorePath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
}

ModelInfo::Manager::~Manager() {
    for (auto& pair : ModelInfos) {
        delete pair.second;
    }
    ModelInfos.clear();
}

void ModelInfo::Manager::addModel(wxString mpath, sd_gui_utils::DirTypes type, wxString name) {
    auto meta_path  = wxFileName(this->GetMetaPath(mpath));
    auto model_path = wxFileName(mpath);

    if (meta_path.FileExists()) {
        wxFile input(meta_path.GetAbsolutePath(), wxFile::read);
        if (!input.IsOpened()) {
            wxLogError("Could not open meta file for reading: %s", meta_path.GetAbsolutePath());
            return;
        }
        try {
            nlohmann::json data;
            wxString data_str;
            input.ReadAll(&data_str);
            data = nlohmann::json::parse(data_str.utf8_string());

            sd_gui_utils::ModelFileInfo _z;
            sd_gui_utils::from_json(data, _z);

            if (meta_path.GetModificationTime() < model_path.GetModificationTime()) {
                this->ModelInfos[model_path.GetAbsolutePath().utf8_string()] = this->GenerateMeta(model_path, type, name, _z);
            } else {
                this->ModelInfos[model_path.GetAbsolutePath().utf8_string()] = new sd_gui_utils::ModelFileInfo(_z);
            }

            this->ModelInfos[model_path.GetAbsolutePath().utf8_string()]->meta_file  = meta_path.GetAbsolutePath().utf8_string();
            this->ModelInfos[model_path.GetAbsolutePath().utf8_string()]->model_type = type;
            this->ModelInfos[model_path.GetAbsolutePath().utf8_string()]->name       = name;
            this->ModelInfos[model_path.GetAbsolutePath().utf8_string()]->path       = model_path.GetAbsolutePath().utf8_string();
            input.Close();
        } catch (const std::exception& e) {
            wxLogError("Exception: %s file: %s", e.what(), model_path.GetAbsolutePath());
        }
    } else {
        // no meta found, create a new one
        sd_gui_utils::ModelFileInfo* meta                            = this->GenerateMeta(model_path, type, name);
        this->ModelInfos[model_path.GetAbsolutePath().utf8_string()] = meta;

        //        wxFile output(model_path.GetAbsolutePath(), wxFile::write);
        //        if (!output.IsOpened()) {
        //            wxLogError("Could not open meta file for writing: %s", meta->meta_file);
        //            return;
        //        }
        this->WriteIntoMeta(meta);
    }
}

bool ModelInfo::Manager::exists(std::string model_path) {
    if (this->ModelInfos.find(model_path) != this->ModelInfos.end()) {
        return true;
    }
    return false;
}

std::vector<sd_gui_utils::ModelFileInfo> ModelInfo::Manager::getList() {
    std::vector<sd_gui_utils::ModelFileInfo> list;
    for (std::map<std::string, sd_gui_utils::ModelFileInfo*>::iterator itr = this->ModelInfos.begin(); itr != this->ModelInfos.end(); itr++) {
        list.emplace_back(*(itr)->second);
    }
    return list;
}

std::string ModelInfo::Manager::pathByName(std::string model_name) {
    return this->getInfoByName(model_name).path;
}

void ModelInfo::Manager::setHash(std::string model_path, std::string hash) {
    if (this->exists(model_path)) {
        this->ModelInfos[model_path]->sha256 = hash;
        this->WriteIntoMeta(this->ModelInfos[model_path]);
    }
}

sd_gui_utils::ModelFileInfo ModelInfo::Manager::getByHash(std::string hash) {
    if (hash.length() == 10) {
        for (auto model : this->ModelInfos) {
            if (model.second->sha256.substr(0, 10) == hash) {
                return *model.second;
            }
        }
        return sd_gui_utils::ModelFileInfo();
    }
    for (auto model : this->ModelInfos) {
        if (model.second->sha256 == hash) {
            return *model.second;
        }
    }

    return sd_gui_utils::ModelFileInfo();
}

sd_gui_utils::ModelFileInfo ModelInfo::Manager::getInfo(std::string path) {
    if (this->ModelInfos.find(path) != this->ModelInfos.end()) {
        return *this->ModelInfos[path];
    }
    return sd_gui_utils::ModelFileInfo();
}

sd_gui_utils::ModelFileInfo* ModelInfo::Manager::getIntoPtr(std::string path) {
    if (this->exists(path)) {
        return this->ModelInfos[path];
    }
    return nullptr;
}
sd_gui_utils::ModelFileInfo* ModelInfo::Manager::getIntoPtrByHash(std::string hash) {
    if (hash.length() == 10) {
        for (auto model : this->ModelInfos) {
            if (model.second->sha256.substr(0, 10) == hash) {
                return model.second;
            }
        }
        return nullptr;
    }
    for (auto model : this->ModelInfos) {
        if (model.second->sha256 == hash) {
            return model.second;
        }
    }
    return nullptr;
}
// @brief Get a modelinfo by it's name
sd_gui_utils::ModelFileInfo ModelInfo::Manager::getInfoByName(std::string model_name) {
    for (std::map<std::string, sd_gui_utils::ModelFileInfo*>::iterator itr = this->ModelInfos.begin(); itr != this->ModelInfos.end(); itr++) {
        if ((itr)->second->name == model_name) {
            return *(itr)->second;
        }
    }

    return sd_gui_utils::ModelFileInfo();
}
// @brief Find a similar named modelinfo
sd_gui_utils::ModelFileInfo ModelInfo::Manager::findInfoByName(std::string model_name) {
    for (auto model : this->ModelInfos) {
        if (model.second->name.find(model_name) != std::string::npos) {
            return *model.second;
        }
    }

    return sd_gui_utils::ModelFileInfo();
}

std::string ModelInfo::Manager::GenerateName(std::string model_path, std::string basepath) {
    auto path        = std::filesystem::path(model_path);
    auto ext         = path.extension().string();
    std::string name = path.filename().replace_extension("").string();
    // prepend the subdirectory to the modelname
    // // wxFileName::GetPathSeparator()
    auto path_name = path.string();
    sd_gui_utils::replace(path_name, basepath, "");
    sd_gui_utils::replace(path_name, "//", "");
    sd_gui_utils::replace(path_name, "\\\\", "");
    sd_gui_utils::replace(path_name, ext, "");

    name = path_name.substr(1);
    return name;
}

sd_gui_utils::ModelFileInfo ModelInfo::Manager::updateCivitAiInfo(std::string model_path, std::string info) {
    sd_gui_utils::ModelFileInfo* model = this->getIntoPtr(model_path);
    return this->updateCivitAiInfo(model);
}
sd_gui_utils::ModelFileInfo ModelInfo::Manager::updateCivitAiInfo(sd_gui_utils::ModelFileInfo* modelinfo) {
    if (modelinfo->civitaiPlainJson.empty()) {
        return sd_gui_utils::ModelFileInfo();
    }
    sd_gui_utils::ModelFileInfo* m = this->getIntoPtr(modelinfo->path);
    m->civitaiPlainJson            = modelinfo->civitaiPlainJson;
    this->ParseCivitAiInfo(m);
    sd_gui_utils::ModelFileInfo newInfo = this->getInfo(m->path);
    this->WriteIntoMeta(newInfo);
    return newInfo;
}

void ModelInfo::Manager::UpdateInfo(sd_gui_utils::ModelFileInfo* modelinfo) {
    this->WriteIntoMeta(modelinfo);
}

sd_gui_utils::ModelFileInfo* ModelInfo::Manager::GenerateMeta(const wxFileName& model_path, sd_gui_utils::DirTypes type, const wxString& name, sd_gui_utils::ModelFileInfo copyMeta) {
    auto path = model_path.GetFullPath().ToStdString();

    sd_gui_utils::ModelFileInfo* mi = new sd_gui_utils::ModelFileInfo(copyMeta);
    mi->size                        = model_path.GetSize().GetValue();
    auto s                          = sd_gui_utils::humanReadableFileSize(mi->size);
    mi->size_f                      = wxString::Format("%.1f%s", s.first, s.second);
    mi->hash_fullsize               = 0;
    mi->hash_progress_size          = 0;
    mi->model_type                  = type;

    mi->meta_file = this->GetMetaPath(model_path.GetAbsolutePath());
    mi->name      = name;
    mi->path      = path;
    return mi;
}

void ModelInfo::Manager::WriteIntoMeta(const std::string& model_path) {
    if (this->exists(model_path)) {
        this->WriteIntoMeta(this->ModelInfos[model_path]);
    }
}

void ModelInfo::Manager::WriteIntoMeta(const sd_gui_utils::ModelFileInfo& modelinfo) {
    nlohmann::json j(modelinfo);
    std::ofstream file(modelinfo.meta_file);
    file << j;
    file.close();
}

void ModelInfo::Manager::WriteIntoMeta(sd_gui_utils::ModelFileInfo* modelinfo) {
    this->WriteIntoMeta(*modelinfo);
}

void ModelInfo::Manager::ParseCivitAiInfo(sd_gui_utils::ModelFileInfo* modelinfo) {
    if (modelinfo->civitaiPlainJson.empty()) {
        return;
    }

    try {
        nlohmann::json data = nlohmann::json::parse(modelinfo->civitaiPlainJson);
        if (data.contains("error")) {
            modelinfo->state = sd_gui_utils::CivitAiState::NOT_FOUND;
            return;
        }
        auto civitaiinfo       = data.template get<CivitAi::ModelInfo>();
        modelinfo->CivitAiInfo = civitaiinfo;
        modelinfo->state       = sd_gui_utils::CivitAiState::OK;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "message: " << e.what() << '\n'
                  << "exception id: " << e.id << '\n'
                  << "byte position of error: " << e.byte << std::endl;
        modelinfo->state = sd_gui_utils::CivitAiState::ERR;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << e.what() << '\n';
        modelinfo->state = sd_gui_utils::CivitAiState::ERR;
    }
}

wxString ModelInfo::Manager::GetMetaPath(const wxString& model_path) {
    if (model_path.IsEmpty()) {
        throw std::invalid_argument("Model path is empty");
    }

    wxFileName path(model_path);
    if (!path.Exists()) {
        throw std::invalid_argument("Model path does not exist");
    }

    path.SetExt("json");
    wxFileName meta_path(this->MetaStorePath, path.GetFullName());

    if (!wxDirExists(meta_path.GetPath())) {
        throw std::runtime_error("Meta path parent directory does not exist");
    }

    if (!wxFileName::DirExists(meta_path.GetPath())) {
        throw std::runtime_error("Meta path parent is not a directory");
    }

    return meta_path.GetFullPath(wxPATH_UNIX);
}
