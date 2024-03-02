#ifndef __SD_GUI_MODEL_INFO__
#define __SD_GUI_MODEL_INFO__
#include <filesystem>
#include <vector>
#include <fmt/format.h>
#include "utils.hpp"

#include "../helpers/civitai.hpp"

namespace ModelInfo
{
    class Manager
    {

    private:
        std::map<std::string, sd_gui_utils::ModelFileInfo *> ModelInfos;
        sd_gui_utils::ModelFileInfo *GenerateMeta(std::string model_path, sd_gui_utils::DirTypes type, std::string name);
        void WriteIntoMeta(std::string model_path);
        void WriteIntoMeta(sd_gui_utils::ModelFileInfo modelinfo);
        void WriteIntoMeta(sd_gui_utils::ModelFileInfo *modelinfo);
        void ParseCivitAiInfo(sd_gui_utils::ModelFileInfo *modelinfo);
        std::string GetMetaPath(std::string model_path);
        std::string MetaStorePath;

    public:
        Manager(std::string meta_base_path);
        ~Manager();
        void addModel(std::string model_path, sd_gui_utils::DirTypes type, std::string name);
        bool exists(std::string model_path);
        std::vector<sd_gui_utils::ModelFileInfo> getList();
        std::string pathByName(std::string model_name);
        std::string getName(std::string model_path);
        void setHash(std::string model_path, std::string hash);
        sd_gui_utils::ModelFileInfo getByHash(std::string hash);
        sd_gui_utils::ModelFileInfo getInfo(std::string path);
        sd_gui_utils::ModelFileInfo *getIntoPtr(std::string path);
        sd_gui_utils::ModelFileInfo getInfoByName(std::string model_name);
        sd_gui_utils::ModelFileInfo findInfoByName(std::string model_name);
        static std::string GenerateName(std::string model_path, std::string basepath);
        sd_gui_utils::ModelFileInfo updateCivitAiInfo(std::string model_path, std::string info);
        sd_gui_utils::ModelFileInfo updateCivitAiInfo(sd_gui_utils::ModelFileInfo *modelinfo);
        void UpdateInfo(sd_gui_utils::ModelFileInfo *modelinfo);
        
    };
};
#endif