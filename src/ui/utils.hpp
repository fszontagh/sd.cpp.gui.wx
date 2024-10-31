#ifndef __MAINFRAME_HPP__UTILS
#define __MAINFRAME_HPP__UTILS
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>

#include <wx/image.h>
#include <wx/textctrl.h>

#include <exiv2/exiv2.hpp>
#include "../helpers/sd.hpp"

#include <openssl/evp.h>

#include "../helpers/civitai.hpp"
#include "../libs/bitmask_operators.h"
#include "../libs/json.hpp"


namespace sd_gui_utils {
    inline std::string repairPath(std::string path) {
#if defined(WIN32) || defined(_WIN32) || \
    defined(__WIN32) && !defined(__CYGWIN__)
        std::replace(path.begin(), path.end(), '\\', '/');
#endif
        return path;
    }

    inline std::string UnicodeToUTF8(const char* str) {
#ifdef WIN32
        return wxString(str).utf8_string();
#endif
        return std::string(str);
    }
    inline std::string UnicodeToUTF8(wxString str) {
#ifdef WIN32
        return str.utf8_string();
#endif
        return str.ToStdString();
    }
    inline std::string UTF8ToUnicode(wxString str) {
#ifdef WIN32
        return wxString::FromUTF8(str).ToStdString();
#endif
        return str.ToStdString();
    }
    inline std::string UTF8ToUnicode(const char* str) {
#ifdef WIN32
        return wxString::FromUTF8(str).ToStdString();
#endif
        return wxString(str).ToStdString();
    }

    typedef struct stringformat {
        size_t start        = -1;
        size_t end          = -1;
        wxTextAttr textAttr = wxTextAttr();
        std::string string  = "";
    } stringformat;

    typedef struct VoidHolder {
        void* p1;  // eventhandler
        void* p2;  // QM::QueueItem
        void* p3;  // Others...
    } VoidHolder;

    enum class DirTypes : int {
        EMPTY       = 0,        // If no option is set
        LORA        = 1 << 0,   // The LORA option represents the 0th bit
        CHECKPOINT  = 1 << 1,   // The CHECKPOINT option represents the 1st bit
        VAE         = 1 << 2,   // The VAE option represents the 2nd bit
        PRESETS     = 1 << 3,   // The PRESETS option represents the 3rd bit
        PROMPTS     = 1 << 4,   // The PROMPTS option represents the 4th bit
        NEG_PROMPTS = 1 << 5,   // The NEG_PROMPTS option represents the 5th bit
        TAESD       = 1 << 6,   // The TAESD option represents the 6th bit
        ESRGAN      = 1 << 7,   // The ESRGAN option represents the 7th bit
        CONTROLNET  = 1 << 8,   // The CONTROLNET option represents the 8th bit
        UPSCALER    = 1 << 9,   // The UPSCALER option represents the 9th bit
        EMBEDDING   = 1 << 10,  // The EMBEDDING option represents the 10th bit
        ALL         = -1,       // All options are set
        UNKNOWN     = -2,       // The unknown option
    };

    template <>
    struct enable_bitmask_operators<sd_gui_utils::DirTypes> {
        static constexpr bool enable = true;
    };

    inline bool filterByModelType(const sd_gui_utils::DirTypes modelType,
                                  sd_gui_utils::DirTypes filterType) {
        return static_cast<bool>(modelType & filterType);
    }

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
        {sd_gui_utils::DirTypes::ALL, "ALL"},
        {sd_gui_utils::DirTypes::UNKNOWN, "UNKNOWN"}};

    enum CivitAiState { OK,
                        NOT_FOUND,
                        ERR,
                        NOT_CHECKED };
    inline const char* civitai_state_str[] = {
        "Ok",
        "Not found",
        "Parse error",
        "",
    };
    struct ModelFileInfo {
        std::string name;
        std::string path;
        std::string url;
        std::string poster;
        std::string sha256;
        uintmax_t size;
        std::string size_f;
        std::string meta_file;
        size_t hash_progress_size         = 0;
        size_t hash_fullsize              = 0;
        sd_gui_utils::DirTypes model_type = sd_gui_utils::DirTypes::UNKNOWN;
        std::string civitaiPlainJson;
        CivitAi::ModelInfo CivitAiInfo;
        sd_gui_utils::CivitAiState state = sd_gui_utils::CivitAiState::NOT_CHECKED;
        std::vector<std::string> preview_images;

        ModelFileInfo() = default;
        // Copy konstruktor
        ModelFileInfo(const sd_gui_utils::ModelFileInfo& other)
            : name(other.name), path(other.path), url(other.url), poster(other.poster), sha256(other.sha256), size(other.size), size_f(other.size_f), meta_file(other.meta_file), hash_progress_size(other.hash_progress_size), hash_fullsize(other.hash_fullsize), model_type(other.model_type), civitaiPlainJson(other.civitaiPlainJson), CivitAiInfo(other.CivitAiInfo), state(other.state), preview_images(other.preview_images) {}
        // Copy assignment operator
        ModelFileInfo& operator=(const sd_gui_utils::ModelFileInfo& other) {
            if (this != &other) {
                name               = other.name;
                path               = other.path;
                url                = other.url;
                poster             = other.poster;
                sha256             = other.sha256;
                size               = other.size;
                size_f             = other.size_f;
                meta_file          = other.meta_file;
                hash_progress_size = other.hash_progress_size;
                hash_fullsize      = other.hash_fullsize;
                model_type         = other.model_type;
                civitaiPlainJson   = other.civitaiPlainJson;
                CivitAiInfo        = other.CivitAiInfo;
                state              = other.state;
                preview_images     = other.preview_images;
            }
            return *this;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo& rh) const {
            return path == rh.path;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo rh) const {
            return path == rh.path;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo* rh) const {
            return path == rh->path;
        }
        inline bool operator==(const std::string& otherPath) const {
            return path == otherPath;
        }
    };

    inline void to_json(nlohmann::json& j, const sd_gui_utils::ModelFileInfo& p) {
        j = nlohmann::json{{"name", sd_gui_utils::UnicodeToUTF8(p.name)},
                           {"path", sd_gui_utils::UnicodeToUTF8(p.path)},
                           {"url", p.url},
                           {"poster", sd_gui_utils::UnicodeToUTF8(p.poster)},
                           {"sha256", p.sha256},
                           {"size", p.size},
                           {"size_f", p.size_f},
                           {"meta_file", sd_gui_utils::UnicodeToUTF8(p.meta_file)},
                           {"model_type", (int)p.model_type},
                           {"civitaiPlainJson", p.civitaiPlainJson},
                           {"CivitAiInfo", p.CivitAiInfo},
                           {"state", (int)p.state},
                           {"preview_images", p.preview_images}

        };
    }

    inline void from_json(const nlohmann::json& j, ModelFileInfo& p) {
        if (j.contains("name")) {
            p.name = sd_gui_utils::UTF8ToUnicode(j.at("name").get<std::string>());
        }

        if (j.contains("path")) {
            p.path = sd_gui_utils::UTF8ToUnicode(j.at("path").get<std::string>());
        }

        if (j.contains("url")) {
            j.at("url").get_to(p.url);
        }

        if (j.contains("poster")) {
            p.poster = sd_gui_utils::UTF8ToUnicode(j.at("poster").get<std::string>());
        }

        if (j.contains("sha256")) {
            j.at("sha256").get_to(p.sha256);
        }

        if (j.contains("size")) {
            j.at("size").get_to(p.size);
        }

        if (j.contains("size_f")) {
            j.at("size_f").get_to(p.size_f);
        }

        if (j.contains("meta_file")) {
            p.meta_file =
                sd_gui_utils::UTF8ToUnicode(j.at("meta_file").get<std::string>());
        }

        if (j.contains("model_type")) {
            p.model_type = j.at("model_type").get<sd_gui_utils::DirTypes>();
        }

        if (j.contains("civitaiPlainJson")) {
            p.civitaiPlainJson = j.at("civitaiPlainJson").get<std::string>();
        }

        if (j.contains("CivitAiInfo")) {
            p.CivitAiInfo = j.at("CivitAiInfo").get<CivitAi::ModelInfo>();
        }
        if (j.contains("state")) {
            p.state = j.at("state").get<sd_gui_utils::CivitAiState>();
        }
        if (j.contains("preview_images")) {
            p.preview_images = j.at("preview_images").get<std::vector<std::string>>();
        }
    }
    inline std::string to_hex(std::array<uint8_t, 32> data) {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (uint8_t val : data) {
            oss << std::setw(2) << (unsigned int)val;
        }
        return oss.str();
    }
    inline std::string sha256_file_openssl(const char* path, void* custom_pointer, void (*callback)(size_t, std::string, void* custom_pointer)) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return "";
        }

        std::streamsize bufferSize = 1 * 1024 * 1024;
        char* buffer               = new char[bufferSize];

        std::string hashResult;
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hashLength;
        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        const EVP_MD* md  = EVP_sha256();
        EVP_DigestInit_ex(mdctx, md, NULL);
        callback(file.tellg(), hashResult, custom_pointer);

        while (file.good()) {
            file.read(buffer, bufferSize);
            std::streamsize bytesRead = file.gcount();
            EVP_DigestUpdate(mdctx, buffer, bytesRead);

            if (file.tellg() % (20 * 1024 * 1024) == 0) {
                callback(file.tellg(), "", custom_pointer);
            }
        }

        EVP_DigestFinal_ex(mdctx, hash, &hashLength);
        EVP_MD_CTX_free(mdctx);

        std::stringstream ss;
        for (unsigned int i = 0; i < hashLength; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        callback(std::filesystem::file_size(path), ss.str(), custom_pointer);

        delete[] buffer;
        return ss.str();
    }
    inline unsigned long long Bytes2Megabytes(size_t bytes) {
        return bytes / (1024 * 1024);  // 1 MB = 1024 * 1024 bájt
    }

    struct cout_redirect {
        cout_redirect(std::streambuf* new_buffer)
            : old(std::cout.rdbuf(new_buffer)) {}

        ~cout_redirect() { std::cout.rdbuf(old); }

    private:
        std::streambuf* old;
    };
    enum imageTypes { JPG,
                      PNG };
    inline const char* image_types_str[] = {"JPG", "PNG"};
    struct config {
        std::string model          = "";
        std::string vae            = "";
        std::string lora           = "";
        std::string embedding      = "";
        std::string taesd          = "";
        std::string esrgan         = "";
        std::string presets        = "";
        std::string output         = "";
        std::string jobs           = "";
        std::string controlnet     = "";
        std::string datapath       = "";
        std::string tmppath        = "";
        std::string thumbs_path    = "";
        bool keep_model_in_memory  = true;
        bool save_all_image        = true;
        int n_threads              = 2;
        imageTypes image_type      = imageTypes::JPG;
        unsigned int image_quality = 90;
        bool show_notifications    = true;
        int notification_timeout   = 60;
    };
    inline std::string formatUnixTimestampToDate(long timestamp) {
        std::time_t time  = static_cast<std::time_t>(timestamp);
        std::tm* timeinfo = std::localtime(&time);

        std::stringstream ss;
        ss << (timeinfo->tm_year + 1900) << "-" << std::setw(2) << std::setfill('0')
           << (timeinfo->tm_mon + 1) << "-" << std::setw(2) << std::setfill('0')
           << timeinfo->tm_mday << " " << std::setw(2) << std::setfill('0')
           << timeinfo->tm_hour << ":" << std::setw(2) << std::setfill('0')
           << timeinfo->tm_min;

        return ss.str();
    }

    struct generator_preset {
        double cfg;
        int seed;
        int clip_skip;
        int steps;
        int width, height;
        sample_method_t sampler;
        std::string name;
        std::string mode;  // modes_str
        int batch;
        std::string type;  // types_str
        std::string path;
    };

    inline void to_json(nlohmann::json& j, const generator_preset& p) {
        j = nlohmann::json{
            {"cfg", p.cfg},
            {"seed", p.seed},
            {"clip_skip", p.clip_skip},
            {"steps", p.steps},
            {"width", p.width},
            {"height", p.height},
            {"name", p.name},
            {"mode", p.mode},
            {"type", p.type},
            {"sampler", (int)p.sampler},
            {"batch", p.batch},
        };
    }

    inline void from_json(const nlohmann::json& j, generator_preset& p) {
        j.at("cfg").get_to(p.cfg);
        j.at("seed").get_to(p.seed);
        j.at("clip_skip").get_to(p.clip_skip);
        j.at("steps").get_to(p.steps);
        j.at("width").get_to(p.width);
        j.at("height").get_to(p.height);
        j.at("name").get_to(p.name);
        j.at("mode").get_to(p.mode);
        if (j.contains("type")) {
            j.at("type").get_to(p.type);
        }
        j.at("batch").get_to(p.batch);
        p.sampler = j.at("sampler").get<sample_method_t>();
    }

    inline int GetCurrentUnixTimestamp() {
        const auto p1 = std::chrono::system_clock::now();
        return static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch())
                .count());
    }
    inline bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    inline std::string normalizePath(const std::string& messyPath) {
        std::filesystem::path path(messyPath);
        std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
        std::string npath                   = canonicalPath.make_preferred().string();

        char toReplace   = '\\';
        char replacement = '/';

        std::replace(npath.begin(), npath.end(), toReplace, replacement);
        return npath;
    }
    // sd c++
    inline const char* sample_method_str[] = {
        "euler_a",
        "euler",
        "heun",
        "dpm2",
        "dpm++2s_a",
        "dpm++2m",
        "dpm++2mv2",
        "ipndm",
        "ipndm_v",
        "lcm",
    };
    inline const char* schedule_str[] = {
        "default",
        "discrete",
        "karras",
        "exponential",
        "ays",
        "gits"};

    inline const char* modes_str[] = {"txt2img", "img2img", "convert", "upscale"};

    enum SDMode { TXT2IMG,
                  IMG2IMG,
                  CONVERT,
                  MODE_COUNT };

    // f32, f16, q4_0, q4_1, q5_0, q5_1, q8_0
    inline std::map<int, std::string> sd_type_gui_names = {
        {32, "Count"},
        {0, "F32"},
        {1, "F16"},
        {2, "Q4_0"},
        {3, "Q4_1"},
        {6, "Q5_0"},
        {7, "Q5_1"},
        {8, "Q8_0"},
        {28, "F64"},
        {30, "BF16"},
        {31, "Q4_0_4_4"},
        {32, "Q4_1_4_4"}

    };

    inline std::map<int, std::string> sd_scheduler_gui_names = {
        {0, "Default"},
        {1, "Discrete"},
        {2, "Karras"},
        {3, "Exponential"},
        {4, "Ays"},
        {5, "Gits"}

    };

    // from sd_type_t
    inline const char* sd_type_names[] = {"F32",
                                          "F16",
                                          "Q4_0",
                                          "Q4_1",
                                          "Q4_2 - not supported",  // Q4_2
                                          "Q4_5 - not supported",  // Q4_5
                                          "Q5_0",
                                          "Q5_1",
                                          "Q8_0",
                                          "Q8_1",
                                          "Q2_K",
                                          "Q3_K",
                                          "Q4_K",
                                          "Q5_K",
                                          "Q6_K",
                                          "Q8_K",
                                          "IQ2_XXS",
                                          "IQ2_XS",
                                          "IQ3_XXS",
                                          "IQ1_S",
                                          "IQ4_NL",
                                          "IQ3_S",
                                          "IQ2_S",
                                          "IQ4_XS",
                                          "I8",
                                          "I16",
                                          "I32",
                                          "I64",
                                          "F64",
                                          "IQ1_M",
                                          "BF16",
                                          "Q4_0_4_4",
                                          "Q4_0_4_8",
                                          "Q4_0_8_8",
                                          "Default"};

    struct SDParams {
        int n_threads = -1;
        SDMode mode   = TXT2IMG;

        std::string model_path;
        std::string clip_l_path;
        std::string clip_g_path;
        std::string t5xxl_path;
        std::string diffusion_model_path;
        std::string vae_path;
        std::string taesd_path;
        std::string esrgan_path;
        std::string controlnet_path;
        std::string embeddings_path;
        std::string stacked_id_embeddings_path;
        std::string input_id_images_path;
        sd_type_t wtype = SD_TYPE_COUNT;
        std::string lora_model_dir;
        std::string output_path = "output.png";
        std::string input_path;
        std::string control_image_path;

        std::string prompt;
        std::string negative_prompt;
        float min_cfg     = 1.0f;
        float cfg_scale   = 7.0f;
        float guidance    = 3.5f;
        float style_ratio = 20.f;
        int clip_skip     = -1;  // <= 0 represents unspecified
        int width         = 512;
        int height        = 512;
        int batch_count   = 1;

        int video_frames         = 6;
        int motion_bucket_id     = 127;
        int fps                  = 6;
        float augmentation_level = 0.f;

        sample_method_t sample_method = EULER_A;
        schedule_t schedule           = DEFAULT;
        int sample_steps              = 20;
        float strength                = 0.75f;
        float control_strength        = 0.9f;
        rng_type_t rng_type           = CUDA_RNG;
        int64_t seed                  = 42;
        bool verbose                  = false;
        bool vae_tiling               = false;
        bool control_net_cpu          = false;
        bool normalize_input          = false;
        bool clip_on_cpu              = false;
        bool vae_on_cpu               = false;
        bool canny_preprocess         = false;
        bool color                    = false;
        int upscale_repeats           = 1;

        SDParams() = default;

        SDParams(const SDParams& other) = default;

        SDParams& operator=(const SDParams& other) = default;
    };

    /* JSONize SD Params*/
    inline void to_json(nlohmann ::json& nlohmann_json_j,
                        const SDParams& nlohmann_json_t) {
        nlohmann_json_j["n_threads"]   = nlohmann_json_t.n_threads;
        nlohmann_json_j["mode"]        = nlohmann_json_t.mode;
        nlohmann_json_j["model_path"]  = nlohmann_json_t.model_path;
        nlohmann_json_j["clip_l_path"] = nlohmann_json_t.clip_l_path;
        nlohmann_json_j["t5xxl_path"]  = nlohmann_json_t.t5xxl_path;
        nlohmann_json_j["diffusion_model_path"] =
            nlohmann_json_t.diffusion_model_path;
        nlohmann_json_j["vae_path"]        = nlohmann_json_t.vae_path;
        nlohmann_json_j["taesd_path"]      = nlohmann_json_t.taesd_path;
        nlohmann_json_j["esrgan_path"]     = nlohmann_json_t.esrgan_path;
        nlohmann_json_j["controlnet_path"] = nlohmann_json_t.controlnet_path;
        nlohmann_json_j["embeddings_path"] = nlohmann_json_t.embeddings_path;
        nlohmann_json_j["stacked_id_embeddings_path"] =
            nlohmann_json_t.stacked_id_embeddings_path;
        nlohmann_json_j["input_id_images_path"] =
            nlohmann_json_t.input_id_images_path;
        nlohmann_json_j["wtype"]              = nlohmann_json_t.wtype;
        nlohmann_json_j["lora_model_dir"]     = nlohmann_json_t.lora_model_dir;
        nlohmann_json_j["output_path"]        = nlohmann_json_t.output_path;
        nlohmann_json_j["input_path"]         = nlohmann_json_t.input_path;
        nlohmann_json_j["control_image_path"] = nlohmann_json_t.control_image_path;
        nlohmann_json_j["prompt"]             = nlohmann_json_t.prompt;
        nlohmann_json_j["negative_prompt"]    = nlohmann_json_t.negative_prompt;
        nlohmann_json_j["min_cfg"]            = nlohmann_json_t.min_cfg;
        nlohmann_json_j["cfg_scale"]          = nlohmann_json_t.cfg_scale;
        nlohmann_json_j["guidance"]           = nlohmann_json_t.guidance;
        nlohmann_json_j["style_ratio"]        = nlohmann_json_t.style_ratio;
        nlohmann_json_j["clip_skip"]          = nlohmann_json_t.clip_skip;
        nlohmann_json_j["width"]              = nlohmann_json_t.width;
        nlohmann_json_j["height"]             = nlohmann_json_t.height;
        nlohmann_json_j["batch_count"]        = nlohmann_json_t.batch_count;
        nlohmann_json_j["video_frames"]       = nlohmann_json_t.video_frames;
        nlohmann_json_j["motion_bucket_id"]   = nlohmann_json_t.motion_bucket_id;
        nlohmann_json_j["fps"]                = nlohmann_json_t.fps;
        nlohmann_json_j["augmentation_level"] = nlohmann_json_t.augmentation_level;
        nlohmann_json_j["sample_method"]      = nlohmann_json_t.sample_method;
        nlohmann_json_j["schedule"]           = nlohmann_json_t.schedule;
        nlohmann_json_j["sample_steps"]       = nlohmann_json_t.sample_steps;
        nlohmann_json_j["strength"]           = nlohmann_json_t.strength;
        nlohmann_json_j["control_strength"]   = nlohmann_json_t.control_strength;
        nlohmann_json_j["rng_type"]           = nlohmann_json_t.rng_type;
        nlohmann_json_j["seed"]               = nlohmann_json_t.seed;
        nlohmann_json_j["verbose"]            = nlohmann_json_t.verbose;
        nlohmann_json_j["vae_tiling"]         = nlohmann_json_t.vae_tiling;
        nlohmann_json_j["control_net_cpu"]    = nlohmann_json_t.control_net_cpu;
        nlohmann_json_j["normalize_input"]    = nlohmann_json_t.normalize_input;
        nlohmann_json_j["clip_on_cpu"]        = nlohmann_json_t.clip_on_cpu;
        nlohmann_json_j["vae_on_cpu"]         = nlohmann_json_t.vae_on_cpu;
        nlohmann_json_j["canny_preprocess"]   = nlohmann_json_t.canny_preprocess;
        nlohmann_json_j["color"]              = nlohmann_json_t.color;
        nlohmann_json_j["upscale_repeats"]    = nlohmann_json_t.upscale_repeats;
    }
    inline void from_json(const nlohmann ::json& nlohmann_json_j,
                          SDParams& nlohmann_json_t){
        {auto iter = nlohmann_json_j.find("n_threads");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.n_threads);
}  // namespace sd_gui_utils
{
    auto iter = nlohmann_json_j.find("mode");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.mode);
}
{
    auto iter = nlohmann_json_j.find("model_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.model_path);
}
{
    auto iter = nlohmann_json_j.find("clip_l_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.clip_l_path);
}
{
    auto iter = nlohmann_json_j.find("t5xxl_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.t5xxl_path);
}
{
    auto iter = nlohmann_json_j.find("diffusion_model_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.diffusion_model_path);
}
{
    auto iter = nlohmann_json_j.find("vae_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.vae_path);
}
{
    auto iter = nlohmann_json_j.find("taesd_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.taesd_path);
}
{
    auto iter = nlohmann_json_j.find("esrgan_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.esrgan_path);
}
{
    auto iter = nlohmann_json_j.find("controlnet_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.controlnet_path);
}
{
    auto iter = nlohmann_json_j.find("embeddings_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.embeddings_path);
}
{
    auto iter = nlohmann_json_j.find("stacked_id_embeddings_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.stacked_id_embeddings_path);
}
{
    auto iter = nlohmann_json_j.find("input_id_images_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.input_id_images_path);
}
{
    auto iter = nlohmann_json_j.find("wtype");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.wtype);
}
{
    auto iter = nlohmann_json_j.find("lora_model_dir");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.lora_model_dir);
}
{
    auto iter = nlohmann_json_j.find("output_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.output_path);
}
{
    auto iter = nlohmann_json_j.find("input_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.input_path);
}
{
    auto iter = nlohmann_json_j.find("control_image_path");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.control_image_path);
}
{
    auto iter = nlohmann_json_j.find("prompt");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.prompt);
}
{
    auto iter = nlohmann_json_j.find("negative_prompt");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.negative_prompt);
}
{
    auto iter = nlohmann_json_j.find("min_cfg");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.min_cfg);
}
{
    auto iter = nlohmann_json_j.find("cfg_scale");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.cfg_scale);
}
{
    auto iter = nlohmann_json_j.find("guidance");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.guidance);
}
{
    auto iter = nlohmann_json_j.find("style_ratio");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.style_ratio);
}
{
    auto iter = nlohmann_json_j.find("clip_skip");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.clip_skip);
}
{
    auto iter = nlohmann_json_j.find("width");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.width);
}
{
    auto iter = nlohmann_json_j.find("height");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.height);
}
{
    auto iter = nlohmann_json_j.find("batch_count");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.batch_count);
}
{
    auto iter = nlohmann_json_j.find("video_frames");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.video_frames);
}
{
    auto iter = nlohmann_json_j.find("motion_bucket_id");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.motion_bucket_id);
}
{
    auto iter = nlohmann_json_j.find("fps");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.fps);
}
{
    auto iter = nlohmann_json_j.find("augmentation_level");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.augmentation_level);
}
{
    auto iter = nlohmann_json_j.find("sample_method");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.sample_method);
}
{
    auto iter = nlohmann_json_j.find("schedule");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.schedule);
}
{
    auto iter = nlohmann_json_j.find("sample_steps");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.sample_steps);
}
{
    auto iter = nlohmann_json_j.find("strength");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.strength);
}
{
    auto iter = nlohmann_json_j.find("control_strength");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.control_strength);
}
{
    auto iter = nlohmann_json_j.find("rng_type");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.rng_type);
}
{
    auto iter = nlohmann_json_j.find("seed");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.seed);
}
{
    auto iter = nlohmann_json_j.find("verbose");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.verbose);
}
{
    auto iter = nlohmann_json_j.find("vae_tiling");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.vae_tiling);
}
{
    auto iter = nlohmann_json_j.find("control_net_cpu");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.control_net_cpu);
}
{
    auto iter = nlohmann_json_j.find("normalize_input");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.normalize_input);
}
{
    auto iter = nlohmann_json_j.find("clip_on_cpu");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.clip_on_cpu);
}
{
    auto iter = nlohmann_json_j.find("vae_on_cpu");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.vae_on_cpu);
}
{
    auto iter = nlohmann_json_j.find("canny_preprocess");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.canny_preprocess);
}
{
    auto iter = nlohmann_json_j.find("color");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.color);
}
{
    auto iter = nlohmann_json_j.find("upscale_repeats");
    if (iter != nlohmann_json_j.end())
        if (!iter->is_null())
            iter->get_to(nlohmann_json_t.upscale_repeats);
}
}
;

/* JSONize SD Params*/
enum ThreadEvents {
    QUEUE,
    HASHING_PROGRESS,
    MODEL_LOAD_DONE,
    GENERATION_DONE,
    STANDALONE_HASHING_PROGRESS,
    STANDALONE_HASHING_DONE,
    HASHING_DONE,
    GENERATION_PROGRESS,
    SD_MESSAGE,
    MESSAGE,
    MODEL_INFO_DOWNLOAD_START,
    MODEL_INFO_DOWNLOAD_FAILED,
    MODEL_INFO_DOWNLOAD_FINISHED,
    MODEL_INFO_DOWNLOAD_IMAGES_START,
    MODEL_INFO_DOWNLOAD_IMAGES_PROGRESS,
    MODEL_INFO_DOWNLOAD_IMAGES_DONE,
    MODEL_INFO_DOWNLOAD_IMAGE_FAILED,
};
// sd c++
inline wxImage ResizeImageToMaxSize(const wxImage& image, int maxWidth, int maxHeight) {
    int newWidth  = image.GetWidth();
    int newHeight = image.GetHeight();

    // Az új méreteket úgy határozzuk meg, hogy megtartsuk a képarányt
    if (newWidth > maxWidth || newHeight > maxHeight) {
        double aspectRatio =
            static_cast<double>(newWidth) / static_cast<double>(newHeight);
        if (aspectRatio > 1.0) {
            // Szélesség korlátozó dimenzió, a magasságot arányosan beállítjuk
            newWidth  = maxWidth;
            newHeight = static_cast<int>(maxWidth / aspectRatio);
        } else {
            // Magasság korlátozó dimenzió, a szélességet arányosan beállítjuk
            newHeight = maxHeight;
            newWidth  = static_cast<int>(maxHeight * aspectRatio);
        }
    }

    // Méretezés az új méretekre
    return image.Scale(newWidth, newHeight);
};

inline const unsigned int generateRandomInt(unsigned int min,
                                            unsigned int max) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<unsigned int> dis(min, max);
    return dis(gen);
};

inline std::pair<double, std::string> humanReadableFileSize(double bytes) {
    static const char* sizes[] = {"B", "KB", "MB", "GB", "TB"};
    long unsigned int div      = 0;
    double size                = bytes;

    while (size >= 1024 && div < (sizeof(sizes) / sizeof(*sizes))) {
        size /= 1024;
        div++;
    }

    return std::make_pair(size, sizes[div]);
}

inline void write_comment(std::string file_name, const char* comment) {
    try {
        // PNG fájl metaadatainak beolvasása és módosítása
        auto image = Exiv2::ImageFactory::open(file_name);
        image->readMetadata();
        auto& exifData                     = image->exifData();
        exifData["Exif.Photo.UserComment"] = comment;
        exifData["Exif.Image.XPComment"]   = comment;

        // PNG fájl metaadatainak frissítése
        image->setExifData(exifData);
        image->writeMetadata();
    } catch (Exiv2::Error& e) {
        std::cerr << "Err: " << e.what() << std::endl;
    }
}
inline std::string removeWhitespace(std::string str) {
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
    str.erase(end_pos, str.end());
    return str;
}
inline std::string tolower(std::string data) {
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return data;
}
inline std::map<std::string, std::string> parseExifPrompts(std::string text) {
    size_t pos = text.find("charset=Unicode ");
    if (pos != std::string::npos) {
        text.erase(pos, std::string("charset=Unicode ").length());
    }

    size_t pos_ = text.find("charset=Unicode");
    if (pos_ != std::string::npos) {
        text.erase(pos_, std::string("charset=Unicode").length());
    }

    std::map<std::string, std::string> result;

    result[std::string("prompt")] = "";

    std::vector<std::string> lines_in_reverse;
    std::istringstream f(text);
    std::string line;
    while (std::getline(f, line)) {
        {
            lines_in_reverse.push_back(line);
        }
    }
    std::reverse(lines_in_reverse.begin(), lines_in_reverse.end());

    std::vector<std::string> seglist;

    // first in vector the last in text

    std::string firstline = lines_in_reverse.front();
    std::istringstream p;
    p.str(firstline);
    std::string segment;
    while (std::getline(p, segment, ',')) {
        // seglist.push_back(segment);
        size_t pos      = segment.find(':');
        std::string key = removeWhitespace(segment.substr(0, pos));
        key             = sd_gui_utils::tolower(key);
        std::string val = removeWhitespace(segment.substr(pos + 1));

        // result[segment.substr(0, pos)] = segment.substr(pos + 1);
        result[key] = val;
    }
    // remve params, we already parsed it...
    lines_in_reverse.erase(lines_in_reverse.begin());

    bool _nfound = false;
    int counter  = 0;
    std::vector<std::string> _n_prompt_lines;

    std::string nn_p("Negative prompt: ");

    for (auto _l : lines_in_reverse) {
        if (_l.substr(0, nn_p.length()) == nn_p) {
            // prompt = prompt + _l.substr(17);
            _n_prompt_lines.emplace_back(_l.substr(17));
            _nfound = true;
            counter++;
            break;
        }
        // prompt = prompt + _l;
        _n_prompt_lines.emplace_back(_l);
        counter++;
    }

    for (int z = 0; z < counter; z++) {
        lines_in_reverse.erase(lines_in_reverse.begin());
    }

    std::string prompt;  // negative or plain prompt...

    std::reverse(_n_prompt_lines.begin(), _n_prompt_lines.end());

    for (auto _n : _n_prompt_lines) {
        prompt = prompt + _n;
    }

    if (_nfound) {
        // result[std::string("negative_prompt")] = prompt;

        result[std::string("negative_prompt")] = prompt;

        std::reverse(lines_in_reverse.begin(), lines_in_reverse.end());
        for (auto _u : lines_in_reverse) {
            result[std::string("prompt")] = result[std::string("prompt")] + _u;
        }
    } else {
        result[std::string("prompt")] = prompt;
    }

    return result;
};

inline wxImage cropResizeImage(const wxImage& originalImage, int targetWidth, int targetHeight) {
    int originalWidth  = originalImage.GetWidth();
    int originalHeight = originalImage.GetHeight();

    double aspectRatio =
        static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
    int newWidth  = targetWidth;
    int newHeight = targetHeight;

    // Kiszámítjuk az új méreteket, hogy megtartsuk a képarányt
    if (originalWidth > targetWidth || originalHeight > targetHeight) {
        if (aspectRatio > 1.0) {
            // Szélesség alapján skálázzuk az új méretet
            newWidth  = targetWidth;
            newHeight = static_cast<int>(targetWidth / aspectRatio);
        } else {
            // Magasság alapján skálázzuk az új méretet
            newHeight = targetHeight;
            newWidth  = static_cast<int>(targetHeight * aspectRatio);
        }
    }

    // Méretezzük az eredeti képet az új méretekre
    wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

    // Üres terület hozzáadása és transzparens töltése
    if (newWidth < targetWidth || newHeight < targetHeight) {
        wxImage finalImage(targetWidth, targetHeight);
        finalImage.SetAlpha();

        finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2,
                         (targetHeight - newHeight) / 2);
        return finalImage;
    }

    return resizedImage;
}
inline std::string cropResizeCacheName(int targetWidth, int targetHeight, const std::string orig_filename, const std::string cache_path) {
    auto cache_name_path     = std::filesystem::path(orig_filename);
    std::string ext          = cache_name_path.extension().string();
    auto filename            = cache_name_path.filename().replace_extension();
    std::string new_filename = filename.string() + "_" +
                               std::to_string(targetWidth) + "x" +
                               std::to_string(targetHeight);
    filename = filename.replace_filename(new_filename);
    filename = filename.replace_extension(ext);

    return sd_gui_utils::normalizePath(cache_path + "/" + filename.string());
}
inline bool cropResizeImageCacheExists(int targetWidth, int targetHeight, const std::string orig_filename = "", const std::string cache_path = "") {
    return std::filesystem::exists(sd_gui_utils::cropResizeCacheName(
        targetWidth, targetHeight, orig_filename, cache_path));
}
inline wxImage cropResizeImageCachedImage(int targetWidth, int targetHeight, const std::string orig_filename, const std::string cache_path) {
    wxImage img;
    img.LoadFile(sd_gui_utils::cropResizeCacheName(targetWidth, targetHeight,
                                                   orig_filename, cache_path));
    return img;
}
inline wxImage cropResizeImage(const wxImage& originalImage, int targetWidth, int targetHeight, const wxColour& backgroundColor, const std::string orig_filename = "", const std::string cache_path = "") {
    std::string cache_name;
    if (!orig_filename.empty() && !cache_path.empty()) {
        if (std::filesystem::exists(cache_name)) {
            wxImage cached_img;
            cached_img.LoadFile(cache_name);
            return cached_img;
        }
    }

    int originalWidth  = originalImage.GetWidth();
    int originalHeight = originalImage.GetHeight();

    double aspectRatio =
        static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
    int newWidth  = targetWidth;
    int newHeight = targetHeight;

    // Kiszámítjuk az új méreteket, hogy megtartsuk a képarányt
    if (originalWidth > targetWidth || originalHeight > targetHeight) {
        if (aspectRatio > 1.0) {
            // Szélesség alapján skálázzuk az új méretet
            newWidth  = targetWidth;
            newHeight = static_cast<int>(targetWidth / aspectRatio);
        } else {
            // Magasság alapján skálázzuk az új méretet
            newHeight = targetHeight;
            newWidth  = static_cast<int>(targetHeight * aspectRatio);
        }
    }

    // Méretezzük az eredeti képet az új méretekre
    wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

    // Üres terület hozzáadása és háttérszínnel való töltése
    if (newWidth < targetWidth || newHeight < targetHeight) {
        wxImage finalImage(targetWidth, targetHeight);
        finalImage.SetRGB(wxRect(0, 0, targetWidth, targetHeight),
                          backgroundColor.Red(), backgroundColor.Green(),
                          backgroundColor.Blue());

        finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2,
                         (targetHeight - newHeight) / 2);
        if (!cache_name.empty()) {
            finalImage.SaveFile(cache_name);
        }
        return finalImage;
    }
    if (!cache_name.empty()) {
        resizedImage.SaveFile(cache_name);
    }
    return resizedImage;
}
inline wxImage cropResizeImage(const std::string image_path, int targetWidth, int targetHeight, const wxColour& backgroundColor, const std::string cache_path = "") {
    std::string cache_name;
    if (!cache_path.empty()) {
        cache_name = sd_gui_utils::cropResizeCacheName(targetWidth, targetHeight,
                                                       image_path, cache_path);
        if (std::filesystem::exists(cache_name)) {
            wxImage cached_img;
            cached_img.LoadFile(cache_name);
            return cached_img;
        }
    }
    auto originalImage = wxImage();

    if (!originalImage.LoadFile(image_path)) {
        return wxImage();
    }

    int originalWidth  = originalImage.GetWidth();
    int originalHeight = originalImage.GetHeight();

    double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
    int newWidth       = targetWidth;
    int newHeight      = targetHeight;

    // Kiszámítjuk az új méreteket, hogy megtartsuk a képarányt
    if (originalWidth > targetWidth || originalHeight > targetHeight) {
        if (aspectRatio > 1.0) {
            // Szélesség alapján skálázzuk az új méretet
            newWidth  = targetWidth;
            newHeight = static_cast<int>(targetWidth / aspectRatio);
        } else {
            // Magasság alapján skálázzuk az új méretet
            newHeight = targetHeight;
            newWidth  = static_cast<int>(targetHeight * aspectRatio);
        }
    }

    // Méretezzük az eredeti képet az új méretekre
    wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

    // Üres terület hozzáadása és háttérszínnel való töltése
    if (newWidth < targetWidth || newHeight < targetHeight) {
        wxImage finalImage(targetWidth, targetHeight);
        finalImage.SetRGB(wxRect(0, 0, targetWidth, targetHeight),
                          backgroundColor.Red(), backgroundColor.Green(),
                          backgroundColor.Blue());

        finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2,
                         (targetHeight - newHeight) / 2);
        if (!cache_name.empty()) {
            finalImage.SaveFile(cache_name);
        }
        return finalImage;
    }
    if (!cache_name.empty()) {
        resizedImage.SaveFile(cache_name);
    }
    return resizedImage;
}
}  // namespace sd_gui_utils
#endif
