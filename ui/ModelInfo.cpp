#include "ModelInfo.h"

ModelInfo::Manager::Manager()
{
}

ModelInfo::Manager::~Manager()
{
}

void ModelInfo::Manager::addModel(std::string model_path, sd_gui_utils::DirTypes type, std::string name)
{
    auto path = std::filesystem::path(model_path);

    std::string meta_path = path.replace_extension(".json").string();

    if (std::filesystem::exists(meta_path))
    {
        std::ifstream f(meta_path);
        try
        {
            nlohmann::json data = nlohmann::json::parse(f);
            sd_gui_utils::ModelFileInfo _z;
            sd_gui_utils::from_json(data, _z);

            this->ModelInfos[model_path] = new sd_gui_utils::ModelFileInfo(_z);
            this->ModelInfos[model_path]->meta_file = meta_path;
            this->ModelInfos[model_path]->model_type = type;
            this->ModelInfos[model_path]->name = name;
            this->ModelInfos[model_path]->path = model_path;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << " file: " << path.string() << '\n';
        }
    }
    else
    {
        // no meta found, create a new one
        sd_gui_utils::ModelFileInfo *meta = this->GenerateMeta(model_path, type, name);
        this->ModelInfos[model_path] = meta;
        /// save info file

        std::ofstream file(meta->meta_file);
        this->WriteIntoMeta(meta);
    }
}

bool ModelInfo::Manager::exists(std::string model_path)
{
    if (this->ModelInfos.find(model_path) != this->ModelInfos.end())
    {
        return true;
    }
    return false;
}

std::vector<sd_gui_utils::ModelFileInfo> ModelInfo::Manager::getList()
{

    std::vector<sd_gui_utils::ModelFileInfo> list;
    for (std::map<std::string, sd_gui_utils::ModelFileInfo *>::iterator itr = this->ModelInfos.begin(); itr != this->ModelInfos.end(); itr++)
    {
        list.emplace_back(*(itr)->second);
    }
    return list;
}

std::string ModelInfo::Manager::pathByName(std::string model_name)
{
    return this->getInfoByName(model_name).path;
}

void ModelInfo::Manager::setHash(std::string model_path, std::string hash)
{
    if (this->exists(model_path))
    {
        this->ModelInfos[model_path]->sha256 = hash;
        this->WriteIntoMeta(this->ModelInfos[model_path]);
    }
}

sd_gui_utils::ModelFileInfo ModelInfo::Manager::getInfo(std::string path)
{
    if (this->ModelInfos.find(path) != this->ModelInfos.end())
    {
        return *this->ModelInfos[path];
    }
    return sd_gui_utils::ModelFileInfo();
}

sd_gui_utils::ModelFileInfo *ModelInfo::Manager::getIntoPtr(std::string path)
{
    if (this->exists(path))
    {
        return this->ModelInfos[path];
    }
    return nullptr;
}

sd_gui_utils::ModelFileInfo ModelInfo::Manager::getInfoByName(std::string model_name)
{
    for (std::map<std::string, sd_gui_utils::ModelFileInfo *>::iterator itr = this->ModelInfos.begin(); itr != this->ModelInfos.end(); itr++)
    {
        if ((itr)->second->name == model_name)
        {
            return *(itr)->second;
        }
    }

    return sd_gui_utils::ModelFileInfo();
}

std::string ModelInfo::Manager::GenerateName(std::string model_path, std::string basepath)
{
    auto path = std::filesystem::path(model_path);
    auto ext = path.extension().string();
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

sd_gui_utils::ModelFileInfo *ModelInfo::Manager::GenerateMeta(std::string model_path, sd_gui_utils::DirTypes type, std::string name)
{
    auto path = std::filesystem::path(model_path);
    sd_gui_utils::ModelFileInfo *mi = new sd_gui_utils::ModelFileInfo();
    mi->size = std::filesystem::file_size(path);
    auto s = sd_gui_utils::humanReadableFileSize(mi->size);
    mi->size_f = wxString::Format("%.4f%s", s.first, s.second);
    mi->hash_fullsize = 0;
    mi->hash_progress_size = 0;
    mi->meta_file = path.replace_extension(".json").string();
    mi->name = name;
    mi->path = path.string();
    return mi;
}

void ModelInfo::Manager::WriteIntoMeta(std::string model_path)
{
    if (this->exists(model_path))
    {
        this->WriteIntoMeta(this->ModelInfos[model_path]);
    }
}

void ModelInfo::Manager::WriteIntoMeta(sd_gui_utils::ModelFileInfo modelinfo)
{
    nlohmann::json j(modelinfo);
    std::ofstream file(modelinfo.meta_file);
    file << j;
    file.close();
}

void ModelInfo::Manager::WriteIntoMeta(sd_gui_utils::ModelFileInfo *modelinfo)
{
    this->WriteIntoMeta(*modelinfo);
}
