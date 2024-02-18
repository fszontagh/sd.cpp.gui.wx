#ifndef __SD_GUI_MODEL_INFO__
#define __SD_GUI_MODEL_INFO__
#include <filesystem>
#include <vector>
#include <fmt/format.h>
#include "utils.hpp"

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

    public:
        Manager();
        ~Manager();
        void addModel(std::string model_path, sd_gui_utils::DirTypes type, std::string name);
        bool exists(std::string model_path);
        std::vector<sd_gui_utils::ModelFileInfo> getList();
        std::string pathByName(std::string model_name);
        std::string getName(std::string model_path);
        void setHash(std::string model_path, std::string hash);
        sd_gui_utils::ModelFileInfo getInfo(std::string path);
        sd_gui_utils::ModelFileInfo *getIntoPtr(std::string path);
        sd_gui_utils::ModelFileInfo getInfoByName(std::string model_name);
        static std::string GenerateName(std::string model_path, std::string basepath);
    };
};
#endif