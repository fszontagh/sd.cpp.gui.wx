#ifndef __SD_GUI_CPP_CIVITAI_HTTP_HELPER__
#define __SD_GUI_CPP_CIVITAI_HTTP_HELPER__

#include <iomanip>
#include <iostream>
#include <sstream>
#include "wx/translation.h"

namespace CivitAi {
    inline std::string urlEncode(const std::string& value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (unsigned char c : value)  // használd unsigned char típust a helyes értékek miatt
        {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << int(c);
                escaped << std::nouppercase;
            }
        }

        return escaped.str();
    }
    enum ModelTypes {
        Checkpoint,
        TextualInversion,
        Hypernetwork,
        AestheticGradient,
        LORA,
        Controlnet,
        Poses
    };

    inline const char* ModelTypesNames[] = {
        "Checkpoint",
        "TextualInversion",  // aka embedding
        "Hypernetwork",
        "AestheticGradient",
        "LORA",
        "Controlnet",
        "Poses"};

    struct PreviewImage {
        std::string localpath;
        std::string url;
        int id;
        bool downloaded = false;
        int width, height;
        bool visible = false;
    };
    enum DownloadItemState {
        PENDING,
        DOWNLOADING,
        FINISHED,
        DOWNLOAD_ERROR
    };
    inline const char* DownloadItemStateNames[] = {
        _("Pending"),
        _("Downloading"),
        _("Finished"),
        _("Error")};

    struct DownloadItem {
        size_t targetSize;
        size_t downloadedSize;
        std::string url;
        std::string local_file;
        std::string tmp_name;
        std::string error;
        std::ofstream* file;
        CivitAi::DownloadItemState state;
        CivitAi::ModelTypes type;
    };
}  // namespace CivitAi

#endif