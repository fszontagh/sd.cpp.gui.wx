#ifndef __MAINFRAME_HPP__UTILS
#define __MAINFRAME_HPP__UTILS
#include <string>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <unordered_map>

#include <wx/image.h>
#include <wx/textctrl.h>

#include "../helpers/sd.hpp"
#include <exiv2/exiv2.hpp>

#include <openssl/sha.h>

#include "../libs/json.hpp"
#include "../libs/stb_image.h"
#include "../libs/bitmask_operators.h"
#include "../helpers/civitai.hpp"

namespace sd_gui_utils
{
    inline std::string repairPath(std::string path)
    {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        std::replace(path.begin(), path.end(), '\\', '/');
#endif
        return path;
    }

    inline std::string UnicodeToUTF8(const char *str)
    {
#ifdef WIN32
        return wxString(str).utf8_string();
#endif
        return std::string(str);
    }
    inline std::string UnicodeToUTF8(wxString str)
    {
#ifdef WIN32
        return str.utf8_string();
#endif
        return str.ToStdString();
    }
    inline std::string UTF8ToUnicode(wxString str)
    {
#ifdef WIN32
        return wxString::FromUTF8(str).ToStdString();
#endif
        return str.ToStdString();
    }
    inline std::string UTF8ToUnicode(const char *str)
    {
#ifdef WIN32
        return wxString::FromUTF8(str).ToStdString();
#endif
        return wxString(str).ToStdString();
    }

    typedef struct stringformat
    {
        size_t start = -1;
        size_t end = -1;
        wxTextAttr textAttr = wxTextAttr();
        std::string string = "";
    } stringformat;

    typedef struct VoidHolder
    {
        void *p1; // eventhandler
        void *p2; // QM::QueueItem
        void *p3; // Others...
    } VoidHolder;

    enum class DirTypes : int
    {
        EMPTY = 0,            // If no option is set
        LORA = 1 << 0,        // The LORA option represents the 0th bit
        CHECKPOINT = 1 << 1,  // The CHECKPOINT option represents the 1st bit
        VAE = 1 << 2,         // The VAE option represents the 2nd bit
        PRESETS = 1 << 3,     // The PRESETS option represents the 3rd bit
        PROMPTS = 1 << 4,     // The PROMPTS option represents the 4th bit
        NEG_PROMPTS = 1 << 5, // The NEG_PROMPTS option represents the 5th bit
        TAESD = 1 << 6,       // The TAESD option represents the 6th bit
        ESRGAN = 1 << 7,      // The ESRGAN option represents the 7th bit
        CONTROLNET = 1 << 8,  // The CONTROLNET option represents the 8th bit
        UPSCALER = 1 << 9,    // The UPSCALER option represents the 9th bit
        EMBEDDING = 1 << 10,  // The EMBEDDING option represents the 10th bit
        ALL = -1,             // All options are set
        UNKNOWN = -2,         // The unknown option
    };

    template <>
    struct enable_bitmask_operators<sd_gui_utils::DirTypes>
    {
        static constexpr bool enable = true;
    };

    inline bool filterByModelType(const sd_gui_utils::DirTypes modelType, sd_gui_utils::DirTypes filterType)
    {
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

    enum CivitAiState
    {
        OK,
        NOT_FOUND,
        ERR,
        NOT_CHECKED
    };
    inline const char *civitai_state_str[] = {
        "Ok",
        "Not found",
        "Parse error",
        "",
    };
    struct ModelFileInfo
    {
        std::string name;
        std::string path;
        std::string url;
        std::string poster;
        std::string sha256;
        uintmax_t size;
        std::string size_f;
        std::string meta_file;
        size_t hash_progress_size = 0;
        size_t hash_fullsize = 0;
        sd_gui_utils::DirTypes model_type = sd_gui_utils::DirTypes::UNKNOWN;
        std::string civitaiPlainJson;
        CivitAi::ModelInfo CivitAiInfo;
        sd_gui_utils::CivitAiState state = sd_gui_utils::CivitAiState::NOT_CHECKED;
        std::vector<std::string> preview_images;

        ModelFileInfo() = default;
        // Copy konstruktor
        ModelFileInfo(const sd_gui_utils::ModelFileInfo &other)
            : name(other.name),
              path(other.path),
              url(other.url),
              poster(other.poster),
              sha256(other.sha256),
              size(other.size),
              size_f(other.size_f),
              meta_file(other.meta_file),
              hash_progress_size(other.hash_progress_size),
              hash_fullsize(other.hash_fullsize),
              model_type(other.model_type),
              civitaiPlainJson(other.civitaiPlainJson),
              CivitAiInfo(other.CivitAiInfo),
              state(other.state),
              preview_images(other.preview_images)
        {
        }
        // Copy assignment operator
        ModelFileInfo &operator=(const sd_gui_utils::ModelFileInfo &other)
        {
            if (this != &other)
            {
                name = other.name;
                path = other.path;
                url = other.url;
                poster = other.poster;
                sha256 = other.sha256;
                size = other.size;
                size_f = other.size_f;
                meta_file = other.meta_file;
                hash_progress_size = other.hash_progress_size;
                hash_fullsize = other.hash_fullsize;
                model_type = other.model_type;
                civitaiPlainJson = other.civitaiPlainJson;
                CivitAiInfo = other.CivitAiInfo;
                state = other.state;
                preview_images = other.preview_images;
            }
            return *this;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo &rh) const
        {
            return path == rh.path;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo rh) const
        {
            return path == rh.path;
        }
        inline bool operator==(const sd_gui_utils::ModelFileInfo *rh) const
        {
            return path == rh->path;
        }
        inline bool operator==(const std::string &otherPath) const
        {
            return path == otherPath;
        }
    };

    inline void to_json(nlohmann::json &j, const sd_gui_utils::ModelFileInfo &p)
    {
        j = nlohmann::json{
            {"name", sd_gui_utils::UnicodeToUTF8(p.name)},
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

    inline void from_json(const nlohmann::json &j, ModelFileInfo &p)
    {
        if (j.contains("name"))
        {
            p.name = sd_gui_utils::UTF8ToUnicode(j.at("name").get<std::string>());
        }

        if (j.contains("path"))
        {
            p.path = sd_gui_utils::UTF8ToUnicode(j.at("path").get<std::string>());
        }

        if (j.contains("url"))
        {
            j.at("url").get_to(p.url);
        }

        if (j.contains("poster"))
        {
            p.poster = sd_gui_utils::UTF8ToUnicode(j.at("poster").get<std::string>());
        }

        if (j.contains("sha256"))
        {
            j.at("sha256").get_to(p.sha256);
        }

        if (j.contains("size"))
        {
            j.at("size").get_to(p.size);
        }

        if (j.contains("size_f"))
        {
            j.at("size_f").get_to(p.size_f);
        }

        if (j.contains("meta_file"))
        {
            p.meta_file = sd_gui_utils::UTF8ToUnicode(j.at("meta_file").get<std::string>());
        }

        if (j.contains("model_type"))
        {
            p.model_type = j.at("model_type").get<sd_gui_utils::DirTypes>();
        }

        if (j.contains("civitaiPlainJson"))
        {
            p.civitaiPlainJson = j.at("civitaiPlainJson").get<std::string>();
        }

        if (j.contains("CivitAiInfo"))
        {
            p.CivitAiInfo = j.at("CivitAiInfo").get<CivitAi::ModelInfo>();
        }
        if (j.contains("state"))
        {
            p.state = j.at("state").get<sd_gui_utils::CivitAiState>();
        }
        if (j.contains("preview_images"))
        {
            p.preview_images = j.at("preview_images").get<std::vector<std::string>>();
        }
    }
    inline std::string to_hex(std::array<uint8_t, 32> data)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (uint8_t val : data)
        {
            oss << std::setw(2) << (unsigned int)val;
        }
        return oss.str();
    }
    inline std::string sha256_file_openssl(const char *path, void *custom_pointer, void (*callback)(size_t, std::string, void *custom_pointer))
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            return "";
        }

        // Buffer méretének meghatározása
        std::streamsize bufferSize = 1 * 1024 * 1024;
        char *buffer = new char[bufferSize];

        std::string hashResult;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        callback(file.tellg(), hashResult, custom_pointer);
        while (file.good())
        {
            file.read(buffer, bufferSize);
            std::streamsize bytesRead = file.gcount();

            SHA256_Update(&sha256, buffer, bytesRead);

            // Hívjuk meg a callback függvényt csak minden 20 MB után
            if (file.tellg() % (20 * 1024 * 1024) == 0)
            {
                callback(file.tellg(), "", custom_pointer);
            }
        }
        SHA256_Final(hash, &sha256);
        // Konvertáljuk a hash-t hexadecimális formátumba
        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        callback(std::filesystem::file_size(path), ss.str(), custom_pointer);

        delete[] buffer;
        return ss.str();
    }
    inline unsigned long long Bytes2Megabytes(size_t bytes)
    {
        return bytes / (1024 * 1024); // 1 MB = 1024 * 1024 bájt
    }

    struct cout_redirect
    {
        cout_redirect(std::streambuf *new_buffer)
            : old(std::cout.rdbuf(new_buffer))
        {
        }

        ~cout_redirect()
        {
            std::cout.rdbuf(old);
        }

    private:
        std::streambuf *old;
    };
    enum imageTypes
    {
        JPG,
        PNG
    };
    inline const char *image_types_str[] = {
        "JPG", "PNG"};
    struct config
    {
        std::string model = "";
        std::string vae = "";
        std::string lora = "";
        std::string embedding = "";
        std::string taesd = "";
        std::string esrgan = "";
        std::string presets = "";
        std::string output = "";
        std::string jobs = "";
        std::string controlnet = "";
        std::string datapath = "";
        std::string tmppath = "";
        std::string thumbs_path = "";
        bool keep_model_in_memory = true;
        bool save_all_image = true;
        int n_threads = 2;
        imageTypes image_type = imageTypes::JPG;
        unsigned int image_quality = 90;
        bool show_notifications = true;
        int notification_timeout = 60;
    };
    inline std::string formatUnixTimestampToDate(long timestamp)
    {
        std::time_t time = static_cast<std::time_t>(timestamp);
        std::tm *timeinfo = std::localtime(&time);

        // A dátum formázása az évszám-hónap-nap formátumba
        std::stringstream ss;
        ss << (timeinfo->tm_year + 1900) << "-" << std::setw(2) << std::setfill('0') << (timeinfo->tm_mon + 1) << "-" << std::setw(2) << std::setfill('0') << timeinfo->tm_mday
           << " " << std::setw(2) << std::setfill('0') << timeinfo->tm_hour << ":" << std::setw(2) << std::setfill('0') << timeinfo->tm_min;

        return ss.str();
    }

    struct generator_preset
    {
        double cfg;
        int seed;
        int clip_skip;
        int steps;
        int width, height;
        sample_method_t sampler;
        std::string name;
        std::string mode; // modes_str
        int batch;
        std::string type; // types_str
        std::string path;
    };

    inline void to_json(nlohmann::json &j, const generator_preset &p)
    {
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

    inline void from_json(const nlohmann::json &j, generator_preset &p)
    {
        j.at("cfg").get_to(p.cfg);
        j.at("seed").get_to(p.seed);
        j.at("clip_skip").get_to(p.clip_skip);
        j.at("steps").get_to(p.steps);
        j.at("width").get_to(p.width);
        j.at("height").get_to(p.height);
        j.at("name").get_to(p.name);
        j.at("mode").get_to(p.mode);
        if (j.contains("type"))
        {
            j.at("type").get_to(p.type);
        }
        j.at("batch").get_to(p.batch);
        p.sampler = j.at("sampler").get<sample_method_t>();
    }

    inline int GetCurrentUnixTimestamp()
    {
        const auto p1 = std::chrono::system_clock::now();
        return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count());
    }
    inline bool replace(std::string &str, const std::string &from, const std::string &to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    inline std::string normalizePath(const std::string &messyPath)
    {
        std::filesystem::path path(messyPath);
        std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
        std::string npath = canonicalPath.make_preferred().string();

        char toReplace = '\\';
        char replacement = '/';

        std::replace(npath.begin(), npath.end(), toReplace, replacement);
        return npath;
    }
    // sd c++
    inline const char *sample_method_str[] = {
        "euler_a",
        "euler",
        "heun",
        "dpm2",
        "dpm++2s_a",
        "dpm++2m",
        "dpm++2mv2",
        "lcm",
    };
    inline const char *schedule_str[] = {
        "default",
        "discrete",
        "karras",
    };

    inline const char *modes_str[] = {
        "txt2img",
        "img2img",
        "convert",
        "upscale"};

    enum SDMode
    {
        TXT2IMG,
        IMG2IMG,
        CONVERT,
        MODE_COUNT
    };

    // f32, f16, q4_0, q4_1, q5_0, q5_1, q8_0
    inline std::unordered_map<int, std::string> sd_type_gui_names = {{20, "Default"}, {0, "F32"}, {1, "F16"}, {2, "Q4_0"}, {3, "Q4_1"}, {6, "Q5_0"}, {7, "Q5_1"}, {8, "Q8_0"}};

    inline const char *sd_type_names[] = {"F32", "F16", "Q4_0", "Q4_1",
                                          "Q4_2 - not supported", // Q4_2
                                          "Q4_5 - not supported", // Q4_5
                                          "Q5_0", "Q5_1", "Q8_0",
                                          "Q8_1", "Q2_K", "Q3_K",
                                          "Q4_K", "Q5_K", "Q6_K",
                                          "Q8_K", "IQ2_XXS", "I8",
                                          "I16", "I32", "Default"};

    struct SDParams
    {
        int n_threads = -1;
        SDMode mode = TXT2IMG;

        std::string model_path;
        std::string vae_path;
        std::string taesd_path;
        std::string esrgan_path;
        std::string controlnet_path;
        std::string embeddings_path;
        std::string id_embeddings_path;
        sd_type_t wtype = SD_TYPE_COUNT;
        std::string lora_model_dir;
        std::string output_path = "output.png";
        std::string input_path;
        /// @brief Controlnet image path
        std::string control_image_path;

        std::string prompt;
        std::string negative_prompt;
        float cfg_scale = 7.0f;
        int clip_skip = -1; // <= 0 represents unspecified
        int width = 512;
        int height = 512;
        int batch_count = 1;

        sample_method_t sample_method = EULER_A;
        schedule_t schedule = DEFAULT;
        int sample_steps = 20;
        float strength = 0.75f;
        float control_strength = 0.9f;
        float style_ratio = 20.f;
        bool normalize_input = false;
        std::string input_id_images_path;
        rng_type_t rng_type = rng_type_t::STD_DEFAULT_RNG;
        int64_t seed = 42;
        bool verbose = false;
        bool vae_decode_only = true; // on img2img is false
        bool vae_tiling = false;
        bool canny_preprocess = false;

        bool keep_clip_on_cpu = false;
        bool keep_control_net_cpu = false;
        bool keep_vae_on_cpu = false;

        SDParams() = default;

        // copy constructor...
        SDParams(const SDParams &other)
            : n_threads(other.n_threads), mode(other.mode), model_path(other.model_path),
              vae_path(other.vae_path), taesd_path(other.taesd_path), esrgan_path(other.esrgan_path),
              controlnet_path(other.controlnet_path), embeddings_path(other.embeddings_path), id_embeddings_path(other.id_embeddings_path),
              wtype(other.wtype), lora_model_dir(other.lora_model_dir), output_path(other.output_path),
              input_path(other.input_path), control_image_path(other.control_image_path),
              prompt(other.prompt), negative_prompt(other.negative_prompt), cfg_scale(other.cfg_scale),
              clip_skip(other.clip_skip), width(other.width), height(other.height), batch_count(other.batch_count),
              sample_method(other.sample_method), schedule(other.schedule), sample_steps(other.sample_steps),
              strength(other.strength), control_strength(other.control_strength), style_ratio(other.style_ratio),
              normalize_input(other.normalize_input), input_id_images_path(other.input_id_images_path),
              rng_type(other.rng_type),
              seed(other.seed), verbose(other.verbose), vae_tiling(other.vae_tiling),
              keep_clip_on_cpu(other.keep_clip_on_cpu),
              keep_control_net_cpu(other.keep_control_net_cpu),
              keep_vae_on_cpu(other.keep_vae_on_cpu),
              canny_preprocess(other.canny_preprocess){
                  // Másolat létrehozása
              };
        SDParams &operator=(const SDParams &other)
        {
            if (this != &other) // Ellenőrizzük, hogy ne másoljuk önmagát
            {
                n_threads = other.n_threads;
                mode = other.mode;
                model_path = other.model_path;
                vae_path = other.vae_path;
                taesd_path = other.taesd_path;
                esrgan_path = other.esrgan_path;
                controlnet_path = other.controlnet_path;
                embeddings_path = other.embeddings_path;
                wtype = other.wtype;
                lora_model_dir = other.lora_model_dir;
                output_path = other.output_path;
                input_path = other.input_path;
                control_image_path = other.control_image_path;
                prompt = other.prompt;
                negative_prompt = other.negative_prompt;
                cfg_scale = other.cfg_scale;
                clip_skip = other.clip_skip;
                width = other.width;
                height = other.height;
                batch_count = other.batch_count;
                sample_method = other.sample_method;
                schedule = other.schedule;
                sample_steps = other.sample_steps;
                strength = other.strength;
                control_strength = other.control_strength;
                style_ratio = other.style_ratio;
                normalize_input = other.normalize_input;
                input_id_images_path = other.input_id_images_path;
                rng_type = other.rng_type;
                seed = other.seed;
                verbose = other.verbose;
                vae_tiling = other.vae_tiling;
                canny_preprocess = other.canny_preprocess;
                keep_control_net_cpu = other.keep_control_net_cpu;
                keep_clip_on_cpu = other.keep_clip_on_cpu;
                keep_vae_on_cpu = other.keep_vae_on_cpu;
            }
            return *this;
        }
    };

    /* JSONize SD Params*/

    inline void to_json(nlohmann::json &j, const SDParams &p)
    {
        j = nlohmann::json{
            {"n_threads", p.n_threads},
            {"mode", (int)p.mode},
            {"model_path", sd_gui_utils::UnicodeToUTF8(p.model_path)},
            {"vae_path", sd_gui_utils::UnicodeToUTF8(p.vae_path)},
            {"taesd_path", sd_gui_utils::UnicodeToUTF8(p.taesd_path)},
            {"esrgan_path", sd_gui_utils::UnicodeToUTF8(p.esrgan_path)},
            {"controlnet_path", sd_gui_utils::UnicodeToUTF8(p.controlnet_path)},
            {"embeddings_path", sd_gui_utils::UnicodeToUTF8(p.embeddings_path)},
            {"id_embeddings_path", sd_gui_utils::UnicodeToUTF8(p.id_embeddings_path)},
            {"wtype", (int)p.wtype},
            {"lora_model_dir", sd_gui_utils::UnicodeToUTF8(p.lora_model_dir)},
            {"output_path", sd_gui_utils::UnicodeToUTF8(p.output_path)},
            {"input_path", sd_gui_utils::UnicodeToUTF8(p.input_path)},
            {"control_image_path", sd_gui_utils::UnicodeToUTF8(p.control_image_path)},
            {"prompt", p.prompt},
            {"negative_prompt", p.negative_prompt},
            {"cfg_scale", p.cfg_scale},
            {"clip_skip", p.clip_skip},
            {"width", p.width},
            {"height", p.height},
            {"batch_count", p.batch_count},
            {"sample_method", (int)p.sample_method},
            {"schedule", (int)p.schedule},
            {"sample_steps", p.sample_steps},
            {"strength", p.strength},
            {"control_strength", p.control_strength},
            {"style_ratio", p.style_ratio},
            {"normalize_input", p.normalize_input},
            {"input_id_images_path", p.input_id_images_path},
            {"rng_type", (int)p.rng_type},
            {"seed", p.seed},
            {"verbose", p.verbose},
            {"vae_tiling", p.vae_tiling},
            {"canny_preprocess", p.canny_preprocess},
            {"keep_control_net_cpu", p.keep_control_net_cpu},
            {"keep_clip_on_cpu", p.keep_clip_on_cpu},
            {"keep_vae_on_cpu", p.keep_vae_on_cpu},

        };
    }

    inline void from_json(const nlohmann::json &j, SDParams &p)
    {
        j.at("n_threads").get_to(p.n_threads);

        p.mode = j.at("mode").get<sd_gui_utils::SDMode>();

        p.model_path = sd_gui_utils::UTF8ToUnicode(j.at("model_path").get<std::string>());
        p.vae_path = sd_gui_utils::UTF8ToUnicode(j.at("vae_path").get<std::string>());
        p.taesd_path = sd_gui_utils::UTF8ToUnicode(j.at("taesd_path").get<std::string>());
        p.esrgan_path = sd_gui_utils::UTF8ToUnicode(j.at("esrgan_path").get<std::string>());
        p.controlnet_path = sd_gui_utils::UTF8ToUnicode(j.at("controlnet_path").get<std::string>());
        p.embeddings_path = sd_gui_utils::UTF8ToUnicode(j.at("embeddings_path").get<std::string>());
        p.lora_model_dir = sd_gui_utils::UTF8ToUnicode(j.at("lora_model_dir").get<std::string>());
        p.output_path = sd_gui_utils::UTF8ToUnicode(j.at("output_path").get<std::string>());
        p.input_path = sd_gui_utils::UTF8ToUnicode(j.at("input_path").get<std::string>());
        p.control_image_path = sd_gui_utils::UTF8ToUnicode(j.at("control_image_path").get<std::string>());

        p.wtype = j.at("wtype").get<sd_type_t>();

        j.at("prompt").get_to(p.prompt);
        j.at("negative_prompt").get_to(p.negative_prompt);
        j.at("cfg_scale").get_to(p.cfg_scale);
        j.at("width").get_to(p.width);
        j.at("height").get_to(p.height);
        j.at("batch_count").get_to(p.batch_count);

        p.sample_method = j.at("sample_method").get<sample_method_t>();
        p.schedule = j.at("schedule").get<schedule_t>();

        j.at("sample_steps").get_to(p.sample_steps);
        j.at("strength").get_to(p.strength);
        j.at("control_strength").get_to(p.control_strength);

        if (j.contains("input_id_images_path"))
        {
            j.at("input_id_images_path").get_to(p.input_id_images_path);
        }
        if (j.contains("style_ratio"))
        {
            j.at("style_ratio").get_to(p.style_ratio);
        }
        if (j.contains("normalize_input"))
        {
            j.at("normalize_input").get_to(p.normalize_input);
        }

        p.rng_type = j.at("rng_type").get<rng_type_t>();

        j.at("seed").get_to(p.seed);
        j.at("verbose").get_to(p.verbose);
        j.at("vae_tiling").get_to(p.vae_tiling);

        if (j.contains("control_net_cpu"))
        {
            j.at("control_net_cpu").get_to(p.keep_control_net_cpu);
        }
        if (j.contains("keep_control_net_cpu"))
        {
            j.at("keep_control_net_cpu").get_to(p.keep_control_net_cpu);
        }
        if (j.contains("keep_vae_on_cpu"))
        {
            j.at("keep_vae_on_cpu").get_to(p.keep_vae_on_cpu);
        }
        if (j.contains("keep_clip_on_cpu"))
        {
            j.at("keep_clip_on_cpu").get_to(p.keep_clip_on_cpu);
        }
        if (j.contains("id_embeddings_path"))
        {
            j.at("id_embeddings_path").get_to(p.id_embeddings_path);
        }

        j.at("canny_preprocess").get_to(p.canny_preprocess);
    };
    /* JSONize SD Params*/
    enum ThreadEvents
    {
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
    inline wxImage ResizeImageToMaxSize(const wxImage &image, int maxWidth, int maxHeight)
    {
        int newWidth = image.GetWidth();
        int newHeight = image.GetHeight();

        // Az új méreteket úgy határozzuk meg, hogy megtartsuk a képarányt
        if (newWidth > maxWidth || newHeight > maxHeight)
        {
            double aspectRatio = static_cast<double>(newWidth) / static_cast<double>(newHeight);
            if (aspectRatio > 1.0)
            {
                // Szélesség korlátozó dimenzió, a magasságot arányosan beállítjuk
                newWidth = maxWidth;
                newHeight = static_cast<int>(maxWidth / aspectRatio);
            }
            else
            {
                // Magasság korlátozó dimenzió, a szélességet arányosan beállítjuk
                newHeight = maxHeight;
                newWidth = static_cast<int>(maxHeight * aspectRatio);
            }
        }

        // Méretezés az új méretekre
        return image.Scale(newWidth, newHeight);
    };

    inline const unsigned int generateRandomInt(unsigned int min, unsigned int max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<unsigned int> dis(min, max);
        return dis(gen);
    };

    inline std::pair<double, std::string> humanReadableFileSize(double bytes)
    {
        static const char *sizes[] = {"B", "KB", "MB", "GB", "TB"};
        long unsigned int div = 0;
        double size = bytes;

        while (size >= 1024 && div < (sizeof(sizes) / sizeof(*sizes)))
        {
            size /= 1024;
            div++;
        }

        return std::make_pair(size, sizes[div]);
    }

    inline void write_comment(std::string file_name, const char *comment)
    {
        try
        {
            // PNG fájl metaadatainak beolvasása és módosítása
            auto image = Exiv2::ImageFactory::open(file_name);
            image->readMetadata();
            auto &exifData = image->exifData();
            exifData["Exif.Photo.UserComment"] = comment;
            exifData["Exif.Image.XPComment"] = comment;

            // PNG fájl metaadatainak frissítése
            image->setExifData(exifData);
            image->writeMetadata();
        }
        catch (Exiv2::Error &e)
        {
            std::cerr << "Err: " << e.what() << std::endl;
        }
    }
    inline std::string removeWhitespace(std::string str)
    {
        std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
        str.erase(end_pos, str.end());
        return str;
    }
    inline std::string tolower(std::string data)
    {
        std::transform(data.begin(), data.end(), data.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return data;
    }
    inline std::map<std::string, std::string> parseExifPrompts(std::string text)
    {
        std::map<std::string, std::string> result;

        result[std::string("prompt")] = "";

        std::vector<std::string> lines_in_reverse;
        std::istringstream f(text);
        std::string line;
        while (std::getline(f, line))
        {
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
        while (std::getline(p, segment, ','))
        {
            // seglist.push_back(segment);
            size_t pos = segment.find(':');
            std::string key = removeWhitespace(segment.substr(0, pos));
            key = sd_gui_utils::tolower(key);
            std::string val = removeWhitespace(segment.substr(pos + 1));

            // result[segment.substr(0, pos)] = segment.substr(pos + 1);
            result[key] = val;
        }
        // remve params, we already parsed it...
        lines_in_reverse.erase(lines_in_reverse.begin());

        bool _nfound = false;
        int counter = 0;
        std::vector<std::string> _n_prompt_lines;

        std::string nn_p("Negative prompt: ");

        for (auto _l : lines_in_reverse)
        {
            if (_l.substr(0, nn_p.length()) == nn_p)
            {
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

        for (int z = 0; z < counter; z++)
        {
            lines_in_reverse.erase(lines_in_reverse.begin());
        }

        std::string prompt; // negative or plain prompt...

        std::reverse(_n_prompt_lines.begin(), _n_prompt_lines.end());

        for (auto _n : _n_prompt_lines)
        {
            prompt = prompt + _n;
        }

        if (_nfound)
        {
            // result[std::string("negative_prompt")] = prompt;

            result[std::string("negative_prompt")] = prompt;

            std::reverse(lines_in_reverse.begin(), lines_in_reverse.end());
            for (auto _u : lines_in_reverse)
            {
                result[std::string("prompt")] = result[std::string("prompt")] + _u;
            }
        }
        else
        {
            result[std::string("prompt")] = prompt;
        }

        // remove garbage
        // charset=Unicode
        std::string charset1("charset=Unicode ");
        if (result["prompt"].substr(0, 1) == " ")
        {
            result["prompt"] = result["prompt"].substr(1);
        }
        if (result["prompt"].substr(0, charset1.length()) == charset1)
        {
            result["prompt"] = result["prompt"].substr(charset1.length());
        }

        return result;
    };

    inline wxImage cropResizeImage(const wxImage &originalImage, int targetWidth, int targetHeight)
    {
        int originalWidth = originalImage.GetWidth();
        int originalHeight = originalImage.GetHeight();

        double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
        int newWidth = targetWidth;
        int newHeight = targetHeight;

        // Kiszámítjuk az új méreteket, hogy megtartsuk a képarányt
        if (originalWidth > targetWidth || originalHeight > targetHeight)
        {
            if (aspectRatio > 1.0)
            {
                // Szélesség alapján skálázzuk az új méretet
                newWidth = targetWidth;
                newHeight = static_cast<int>(targetWidth / aspectRatio);
            }
            else
            {
                // Magasság alapján skálázzuk az új méretet
                newHeight = targetHeight;
                newWidth = static_cast<int>(targetHeight * aspectRatio);
            }
        }

        // Méretezzük az eredeti képet az új méretekre
        wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

        // Üres terület hozzáadása és transzparens töltése
        if (newWidth < targetWidth || newHeight < targetHeight)
        {
            wxImage finalImage(targetWidth, targetHeight);
            finalImage.SetAlpha();

            finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2, (targetHeight - newHeight) / 2);
            return finalImage;
        }

        return resizedImage;
    }
    inline std::string cropResizeCacheName(int targetWidth, int targetHeight, const std::string orig_filename, const std::string cache_path)
    {

        auto cache_name_path = std::filesystem::path(orig_filename);
        std::string ext = cache_name_path.extension().string();
        auto filename = cache_name_path.filename().replace_extension();
        std::string new_filename = filename.string() + "_" + std::to_string(targetWidth) + "x" + std::to_string(targetHeight);
        filename = filename.replace_filename(new_filename);
        filename = filename.replace_extension(ext);

        return sd_gui_utils::normalizePath(cache_path + "/" + filename.string());
    }
    inline bool cropResizeImageCacheExists(int targetWidth, int targetHeight, const std::string orig_filename = "", const std::string cache_path = "")
    {
        return std::filesystem::exists(sd_gui_utils::cropResizeCacheName(targetWidth, targetHeight, orig_filename, cache_path));
    }
    inline wxImage cropResizeImageCachedImage(int targetWidth, int targetHeight, const std::string orig_filename, const std::string cache_path)
    {
        wxImage img;
        img.LoadFile(sd_gui_utils::cropResizeCacheName(targetWidth, targetHeight, orig_filename, cache_path));
        return img;
    }
    inline wxImage cropResizeImage(const wxImage &originalImage, int targetWidth, int targetHeight, const wxColour &backgroundColor, const std::string orig_filename = "", const std::string cache_path = "")
    {
        std::string cache_name;
        if (!orig_filename.empty() && !cache_path.empty())
        {

            if (std::filesystem::exists(cache_name))
            {
                wxImage cached_img;
                cached_img.LoadFile(cache_name);
                return cached_img;
            }
        }

        int originalWidth = originalImage.GetWidth();
        int originalHeight = originalImage.GetHeight();

        double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
        int newWidth = targetWidth;
        int newHeight = targetHeight;

        // Kiszámítjuk az új méreteket, hogy megtartsuk a képarányt
        if (originalWidth > targetWidth || originalHeight > targetHeight)
        {
            if (aspectRatio > 1.0)
            {
                // Szélesség alapján skálázzuk az új méretet
                newWidth = targetWidth;
                newHeight = static_cast<int>(targetWidth / aspectRatio);
            }
            else
            {
                // Magasság alapján skálázzuk az új méretet
                newHeight = targetHeight;
                newWidth = static_cast<int>(targetHeight * aspectRatio);
            }
        }

        // Méretezzük az eredeti képet az új méretekre
        wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

        // Üres terület hozzáadása és háttérszínnel való töltése
        if (newWidth < targetWidth || newHeight < targetHeight)
        {
            wxImage finalImage(targetWidth, targetHeight);
            finalImage.SetRGB(wxRect(0, 0, targetWidth, targetHeight), backgroundColor.Red(), backgroundColor.Green(), backgroundColor.Blue());

            finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2, (targetHeight - newHeight) / 2);
            if (!cache_name.empty())
            {
                finalImage.SaveFile(cache_name);
            }
            return finalImage;
        }
        if (!cache_name.empty())
        {
            resizedImage.SaveFile(cache_name);
        }
        return resizedImage;
    }
    inline wxImage cropResizeImage(const std::string image_path, int targetWidth, int targetHeight, const wxColour &backgroundColor, const std::string cache_path = "")
    {
        std::string cache_name;
        if (!cache_path.empty())
        {
            cache_name = sd_gui_utils::cropResizeCacheName(targetWidth, targetHeight, image_path, cache_path);
            if (std::filesystem::exists(cache_name))
            {
                wxImage cached_img;
                cached_img.LoadFile(cache_name);
                return cached_img;
            }
        }
        auto originalImage = wxImage();

        if (!originalImage.LoadFile(image_path))
        {
            return wxImage();
        }

        int originalWidth = originalImage.GetWidth();
        int originalHeight = originalImage.GetHeight();

        double aspectRatio = static_cast<double>(originalWidth) / static_cast<double>(originalHeight);
        int newWidth = targetWidth;
        int newHeight = targetHeight;

        // Kiszámítjuk az új méreteket, hogy megtartsuk a képarányt
        if (originalWidth > targetWidth || originalHeight > targetHeight)
        {
            if (aspectRatio > 1.0)
            {
                // Szélesség alapján skálázzuk az új méretet
                newWidth = targetWidth;
                newHeight = static_cast<int>(targetWidth / aspectRatio);
            }
            else
            {
                // Magasság alapján skálázzuk az új méretet
                newHeight = targetHeight;
                newWidth = static_cast<int>(targetHeight * aspectRatio);
            }
        }

        // Méretezzük az eredeti képet az új méretekre
        wxImage resizedImage = originalImage.Scale(newWidth, newHeight);

        // Üres terület hozzáadása és háttérszínnel való töltése
        if (newWidth < targetWidth || newHeight < targetHeight)
        {
            wxImage finalImage(targetWidth, targetHeight);
            finalImage.SetRGB(wxRect(0, 0, targetWidth, targetHeight), backgroundColor.Red(), backgroundColor.Green(), backgroundColor.Blue());

            finalImage.Paste(resizedImage, (targetWidth - newWidth) / 2, (targetHeight - newHeight) / 2);
            if (!cache_name.empty())
            {
                finalImage.SaveFile(cache_name);
            }
            return finalImage;
        }
        if (!cache_name.empty())
        {
            resizedImage.SaveFile(cache_name);
        }
        return resizedImage;
    }
}
#endif
