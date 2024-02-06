#ifndef __MAINFRAME_HPP__UTILS
#define __MAINFRAME_HPP__UTILS
#include <string>
#include <filesystem>
#include <iostream>
#include <random>

#include <opencv2/opencv.hpp>
#include <stable-diffusion.h>

#include <nlohmann/json.hpp>

namespace sd_gui_utils
{

    typedef struct VoidHolder
    {
        void *p1;
        void *p2;
    } VoidHolder;

    struct ModelFileInfo
    {
        std::string name;
        std::string path;
        std::string url;
        std::string poster;
        std::string sha256;
        uintmax_t size;
        std::string size_f;
    };

    inline void to_json(nlohmann::json &j, const ModelFileInfo &p)
    {
        j = nlohmann::json{
            {"name", p.name},
            {"path", p.path},
            {"url", p.url},
            {"poster", p.poster},
            {"sha256", p.sha256},
            {"size", p.size},
            {"size_f", p.size_f}
        };
    }

    inline void from_json(const nlohmann::json &j, ModelFileInfo &p)
    {
        j.at("name").get_to(p.name);
        j.at("path").get_to(p.path);
        j.at("url").get_to(p.url);
        j.at("poster").get_to(p.poster);
        j.at("sha256").get_to(p.sha256);
        j.at("size").get_to(p.size);
        j.at("size_f").get_to(p.size_f);
    }

    enum THREAD_STATUS_MESSAGES
    {
        MESSAGE,
        MODEL_LOAD_START,
        MODEL_LOAD_DONE,
        MODEL_LOAD_ERROR,
        GENERATION_START,
        GENERATION_PROGRESS,
        GENERATION_DONE,
        GENERATION_ERROR
    };
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
    struct config
    {
        std::string model = "";
        std::string vae = "";
        std::string lora = "";
        std::string embedding = "";
        std::string presets = "";
        std::string output = "";
        std::string jobs = "";
        bool keep_model_in_memory = true;
        bool save_all_image = true;
        int n_threads = 2;
    };
    inline std::string formatUnixTimestampToDate(long timestamp)
    {
        std::time_t time = static_cast<std::time_t>(timestamp);
        std::tm *timeinfo = std::localtime(&time);

        std::ostringstream oss;
        oss << (timeinfo->tm_year + 1900) << "-" << std::setw(2) << std::setfill('0') << (timeinfo->tm_mon + 1) << "-" << std::setw(2) << std::setfill('0') << timeinfo->tm_mday
            << " " << std::setw(2) << std::setfill('0') << timeinfo->tm_hour << ":" << std::setw(2) << std::setfill('0') << timeinfo->tm_min;

        return oss.str();
    }
    enum DirTypes
    {
        LORA,
        CHECKPOINT,
        VAE,
        PRESETS,
        PROMPTS,
        NEG_PROMPTS
    };
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
    };

    enum SDMode
    {
        TXT2IMG,
        IMG2IMG,
        CONVERT,
        MODE_COUNT
    };

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
        sd_type_t wtype = SD_TYPE_COUNT;
        std::string lora_model_dir;
        std::string output_path = "output.png";
        std::string input_path;
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
        rng_type_t rng_type = CUDA_RNG;
        int64_t seed = 42;
        bool verbose = false;
        bool vae_tiling = false;
        bool control_net_cpu = false;
        bool canny_preprocess = false;

        SDParams() = default;

        // copy constructor...
        SDParams(const SDParams &other)
            : n_threads(other.n_threads), mode(other.mode), model_path(other.model_path),
              vae_path(other.vae_path), taesd_path(other.taesd_path), esrgan_path(other.esrgan_path),
              controlnet_path(other.controlnet_path), embeddings_path(other.embeddings_path),
              wtype(other.wtype), lora_model_dir(other.lora_model_dir), output_path(other.output_path),
              input_path(other.input_path), control_image_path(other.control_image_path),
              prompt(other.prompt), negative_prompt(other.negative_prompt), cfg_scale(other.cfg_scale),
              clip_skip(other.clip_skip), width(other.width), height(other.height), batch_count(other.batch_count),
              sample_method(other.sample_method), schedule(other.schedule), sample_steps(other.sample_steps),
              strength(other.strength), control_strength(other.control_strength), rng_type(other.rng_type),
              seed(other.seed), verbose(other.verbose), vae_tiling(other.vae_tiling),
              control_net_cpu(other.control_net_cpu), canny_preprocess(other.canny_preprocess){
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
                rng_type = other.rng_type;
                seed = other.seed;
                verbose = other.verbose;
                vae_tiling = other.vae_tiling;
                control_net_cpu = other.control_net_cpu;
                canny_preprocess = other.canny_preprocess;
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
            {"model_path", p.model_path},
            {"vae_path", p.vae_path},
            {"taesd_path", p.taesd_path},
            {"esrgan_path", p.esrgan_path},
            {"controlnet_path", p.controlnet_path},
            {"embeddings_path", p.embeddings_path},
            {"wtype", (int)p.wtype},
            {"lora_model_dir", p.lora_model_dir},
            {"output_path", p.output_path},
            {"input_path", p.input_path},
            {"control_image_path", p.control_image_path},
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
            {"rng_type", (int)p.rng_type},
            {"seed", p.seed},
            {"verbose", p.verbose},
            {"vae_tiling", p.vae_tiling},
            {"control_net_cpu", p.control_net_cpu},
            {"canny_preprocess", p.canny_preprocess},

        };
    }

    inline void from_json(const nlohmann::json &j, SDParams &p)
    {
        j.at("n_threads").get_to(p.n_threads);

        p.mode = j.at("mode").get<sd_gui_utils::SDMode>();

        j.at("model_path").get_to(p.model_path);
        j.at("vae_path").get_to(p.vae_path);
        j.at("taesd_path").get_to(p.taesd_path);
        j.at("esrgan_path").get_to(p.esrgan_path);
        j.at("controlnet_path").get_to(p.controlnet_path);
        j.at("embeddings_path").get_to(p.embeddings_path);

        p.wtype = j.at("wtype").get<sd_type_t>();

        j.at("lora_model_dir").get_to(p.lora_model_dir);
        j.at("output_path").get_to(p.output_path);
        j.at("input_path").get_to(p.input_path);
        j.at("control_image_path").get_to(p.control_image_path);
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

        p.rng_type = j.at("rng_type").get<rng_type_t>();

        j.at("seed").get_to(p.seed);
        j.at("verbose").get_to(p.verbose);
        j.at("vae_tiling").get_to(p.vae_tiling);
        j.at("control_net_cpu").get_to(p.control_net_cpu);
        j.at("canny_preprocess").get_to(p.canny_preprocess);
    }
    /* JSONize SD Params*/

    // sd c++

    /*cv::Mat GetSquareImage(const unsigned char *img_data, int width, int height, int target_width = 500)
     {
         unsigned char * img_data_ptr = (unsigned char*) &img_data;
         cv::Mat mat(height, width, CV_8UC1, img_data_ptr);
         return GetSquareImage(mat, target_width);
     }
     cv::Mat GetSquareImage(const char *data, int width, int height, int target_width = 500)
     {
         cv::Mat mat(height, width, CV_8UC1, &data[0]);
         return GetSquareImage(mat, target_width);
     }*/
    // opencv stuffs
    inline cv::Mat GetSquareImage(const cv::Mat &img, int target_width = 500)
    {
        int width = img.cols,
            height = img.rows;

        cv::Mat square = cv::Mat::zeros(target_width, target_width, img.type());

        int max_dim = (width >= height) ? width : height;
        float scale = ((float)target_width) / max_dim;
        cv::Rect roi;
        if (width >= height)
        {
            roi.width = target_width;
            roi.x = 0;
            roi.height = static_cast<float>(height) * scale;
            roi.y = (target_width - roi.height) / 2;
        }
        else
        {
            roi.y = 0;
            roi.height = target_width;
            roi.width = static_cast<float>(width) * scale;
            roi.x = (target_width - roi.width) / 2;
        }

        cv::resize(img, square(roi), roi.size());

        return square;
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
        int div = 0;
        double size = bytes;

        while (size >= 1024 && div < (sizeof(sizes) / sizeof(*sizes)))
        {
            size /= 1024;
            div++;
        }

        return std::make_pair(size, sizes[div]);
    }

};
#endif