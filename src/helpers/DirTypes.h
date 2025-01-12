#ifndef _HELPERS_DIR_TYPES_H_
#define _HELPERS_DIR_TYPES_H_

namespace sd_gui_utils {

    enum class ModelInfoTag {
        None      = 0,       // No specific tag
        Deletable = 1 << 0,  // Indicates the item can be deleted
        Favorite  = 1 << 1   // Indicates the item is marked as favorite
    };

    // Allow bitwise operations for ModelInfoTag.
    inline ModelInfoTag operator|(ModelInfoTag lhs, ModelInfoTag rhs) {
        return static_cast<ModelInfoTag>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline ModelInfoTag& operator|=(ModelInfoTag& lhs, ModelInfoTag rhs) {
        lhs = lhs | rhs;
        return lhs;
    }
    inline ModelInfoTag operator~(ModelInfoTag tag) {
        return static_cast<ModelInfoTag>(~static_cast<int>(tag));
    }

    inline ModelInfoTag operator&(ModelInfoTag lhs, ModelInfoTag rhs) {
        return static_cast<ModelInfoTag>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    inline bool HasTag(ModelInfoTag tags, ModelInfoTag tag) {
        return (tags & tag) != ModelInfoTag::None;
    }

    enum class DirTypes : int {
        EMPTY            = 0,     // If no option is set
        LORA             = 1,     // The LORA option represents the 0th bit
        CHECKPOINT       = 2,     // The CHECKPOINT option represents the 1st bit
        VAE              = 4,     // The VAE option represents the 2nd bit
        PRESETS          = 8,     // The PRESETS option represents the 3rd bit
        PROMPTS          = 16,    // The PROMPTS option represents the 4th bit
        NEG_PROMPTS      = 32,    // The NEG_PROMPTS option represents the 5th bit
        TAESD            = 64,    // The TAESD option represents the 6th bit
        ESRGAN           = 128,   // The ESRGAN option represents the 7th bit
        CONTROLNET       = 256,   // The CONTROLNET option represents the 8th bit
        UPSCALER         = 512,   // The UPSCALER option represents the 9th bit
        EMBEDDING        = 1024,  // The EMBEDDING option represents the 10th bit
        PROMPT_TEMPLATES = 2048,  // The PROMPT_TEMPLATES option represents the 11th bit
        ALL              = -1,    // All options are set
        UNKNOWN          = -2     // The unknown option
    };

    enum class DirTypesOld : int {
        EMPTY            = 0,        // If no option is set
        LORA             = 1 << 0,   // The LORA option represents the 0th bit
        CHECKPOINT       = 1 << 1,   // The CHECKPOINT option represents the 1st bit
        VAE              = 1 << 2,   // The VAE option represents the 2nd bit
        PRESETS          = 1 << 3,   // The PRESETS option represents the 3rd bit
        PROMPTS          = 1 << 4,   // The PROMPTS option represents the 4th bit
        NEG_PROMPTS      = 1 << 5,   // The NEG_PROMPTS option represents the 5th bit
        TAESD            = 1 << 6,   // The TAESD option represents the 6th bit
        ESRGAN           = 1 << 7,   // The ESRGAN option represents the 7th bit
        CONTROLNET       = 1 << 8,   // The CONTROLNET option represents the 8th bit
        UPSCALER         = 1 << 9,   // The UPSCALER option represents the 9th bit
        EMBEDDING        = 1 << 10,  // The EMBEDDING option represents the 10th bit
        PROMPT_TEMPLATES = 1 << 11,  // The PROMPT_TEMPLATES option represents the 11th bit
        ALL              = -1,       // All options are set
        UNKNOWN          = -2,       // The unknown option
    };

        inline std::unordered_map<DirTypes, std::string> dirtypes_str = {
            {sd_gui_utils::DirTypes::LORA, "LORA"},
            {sd_gui_utils::DirTypes::CHECKPOINT, "CHECKPOINT"},
            {sd_gui_utils::DirTypes::VAE, "VAE"},
            {sd_gui_utils::DirTypes::PRESETS, "PRESETS"},
            {sd_gui_utils::DirTypes::PROMPTS, "PROMPTS"},
            {sd_gui_utils::DirTypes::NEG_PROMPTS, "NEG_PROMPTS"},
            {sd_gui_utils::DirTypes::TAESD, "TAESD"},
            {sd_gui_utils::DirTypes::ESRGAN, "ESRGAN"},
            {sd_gui_utils::DirTypes::CONTROLNET, "CONTROLNET"},
            {sd_gui_utils::DirTypes::UPSCALER, "UPSCALER"},
            {sd_gui_utils::DirTypes::EMBEDDING, "EMBEDDING"},
            {sd_gui_utils::DirTypes::PROMPT_TEMPLATES, "PROMPT_TEMPLATES"},
            {sd_gui_utils::DirTypes::ALL, "ALL"},
            {sd_gui_utils::DirTypes::UNKNOWN, "UNKNOWN"}};

        inline std::unordered_map<wxString, DirTypes> dirtypes_wxstr = {{wxT("LORA"), sd_gui_utils::DirTypes::LORA}, {wxT("CHECKPOINT"), sd_gui_utils::DirTypes::CHECKPOINT}, {wxT("VAE"), sd_gui_utils::DirTypes::VAE}, {wxT("PRESETS"), sd_gui_utils::DirTypes::PRESETS}, {wxT("PROMPTS"), sd_gui_utils::DirTypes::PROMPTS}, {wxT("NEG_PROMPTS"), sd_gui_utils::DirTypes::NEG_PROMPTS}, {wxT("TAESD"), sd_gui_utils::DirTypes::TAESD}, {wxT("ESRGAN"), sd_gui_utils::DirTypes::ESRGAN}, {wxT("CONTROLNET"), sd_gui_utils::DirTypes::CONTROLNET}, {wxT("UPSCALER"), sd_gui_utils::DirTypes::UPSCALER}, {wxT("EMBEDDING"), sd_gui_utils::DirTypes::EMBEDDING}, {wxT("PROMPT_TEMPLATES"), sd_gui_utils::DirTypes::PROMPT_TEMPLATES}, {wxT("ALL"), sd_gui_utils::DirTypes::ALL}, {wxT("UNKNOWN"), sd_gui_utils::DirTypes::UNKNOWN}};
    }  // namespace sd_gui_utils
#endif  // _HELPERS_DIR_TYPES_H_