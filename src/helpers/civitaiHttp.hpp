#ifndef __SD_GUI_CPP_CIVITAI_HTTP_HELPER__
#define __SD_GUI_CPP_CIVITAI_HTTP_HELPER__
namespace CivitAi
{
    enum ModelTypes
    {
        Checkpoint,
        TextualInversion,
        Hypernetwork,
        AestheticGradient,
        LORA,
        Controlnet,
        Poses
    };

    inline const char *ModelTypesNames[] = {
        "Checkpoint",
        "TextualInversion",
        "Hypernetwork",
        "AestheticGradient",
        "LORA",
        "Controlnet",
        "Poses"};

    enum DownloadType
    {
        IMAGE,
        MODELFILE
    };

    struct PreviewImage
    {
        std::string localpath;
        std::string url;
        int id;
        bool downloaded = false;
        int width, height;
        bool visible = false;
    };
    class DownloadItem
    {
        size_t targetSize;
        size_t downloadedSize;
        std::string url;
        CivitAi::DownloadType type;
    };
} // namespace CivitAi

#endif;