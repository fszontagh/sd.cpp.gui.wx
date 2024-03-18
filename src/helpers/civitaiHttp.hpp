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
        "Poses"
    };
} // namespace CivitAi

#endif;