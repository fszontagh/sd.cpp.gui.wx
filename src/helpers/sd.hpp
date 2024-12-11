#ifndef STABLE_DIFFUSION_WRAPPER_H
#define STABLE_DIFFUSION_WRAPPER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../libs/json.hpp"

inline static void writeCriticalLog(const std::string& log, std::string filename) {
    std::filesystem::path p = std::filesystem::current_path() / filename;
    std::ofstream logFile(p, std::ios::app);
    logFile << log << std::endl;
    logFile.close();
}

// Enumerations
enum rng_type_t { STD_DEFAULT_RNG,
                  CUDA_RNG };

enum sample_method_t {
    EULER_A,
    EULER,
    HEUN,
    DPM2,
    DPMPP2S_A,
    DPMPP2M,
    DPMPP2Mv2,
    IPNDM,
    IPNDM_V,
    LCM,
    N_SAMPLE_METHODS
};

enum schedule_t {
    DEFAULT,
    DISCRETE,
    KARRAS,
    EXPONENTIAL,
    AYS,
    GITS,
    N_SCHEDULES
};

enum sd_type_t {
    SD_TYPE_F32  = 0,
    SD_TYPE_F16  = 1,
    SD_TYPE_Q4_0 = 2,
    SD_TYPE_Q4_1 = 3,
    // SD_TYPE_Q4_2 = 4, support has been removed
    // SD_TYPE_Q4_3 = 5, support has been removed
    SD_TYPE_Q5_0     = 6,
    SD_TYPE_Q5_1     = 7,
    SD_TYPE_Q8_0     = 8,
    SD_TYPE_Q8_1     = 9,
    SD_TYPE_Q2_K     = 10,
    SD_TYPE_Q3_K     = 11,
    SD_TYPE_Q4_K     = 12,
    SD_TYPE_Q5_K     = 13,
    SD_TYPE_Q6_K     = 14,
    SD_TYPE_Q8_K     = 15,
    SD_TYPE_IQ2_XXS  = 16,
    SD_TYPE_IQ2_XS   = 17,
    SD_TYPE_IQ3_XXS  = 18,
    SD_TYPE_IQ1_S    = 19,
    SD_TYPE_IQ4_NL   = 20,
    SD_TYPE_IQ3_S    = 21,
    SD_TYPE_IQ2_S    = 22,
    SD_TYPE_IQ4_XS   = 23,
    SD_TYPE_I8       = 24,
    SD_TYPE_I16      = 25,
    SD_TYPE_I32      = 26,
    SD_TYPE_I64      = 27,
    SD_TYPE_F64      = 28,
    SD_TYPE_IQ1_M    = 29,
    SD_TYPE_BF16     = 30,
    SD_TYPE_Q4_0_4_4 = 31,
    SD_TYPE_Q4_0_4_8 = 32,
    SD_TYPE_Q4_0_8_8 = 33,
    SD_TYPE_TQ1_0    = 34,
    SD_TYPE_TQ2_0    = 35,
    SD_TYPE_COUNT,
};
enum sd_log_level_t { SD_LOG_DEBUG,
                      SD_LOG_INFO,
                      SD_LOG_WARN,
                      SD_LOG_ERROR };
typedef void (*sd_log_cb_t)(enum sd_log_level_t level, const char* text, void* data);
typedef void (*sd_progress_cb_t)(int step, int steps, float time, void* data);

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channel;
    uint8_t* data;
} sd_image_t;

typedef struct sd_ctx_t sd_ctx_t;
typedef struct upscaler_ctx_t upscaler_ctx_t;
typedef uint8_t* (*ConvertFunction)(
    const char*,    // input path
    const char*,    // vae path
    const char*,    // output path
    enum sd_type_t  // wtype
);

typedef uint8_t* (*PreprocessCannyFunction)(
    uint8_t*,  // image
    int,       // width
    int,       // height
    float,     // high_threshold
    float,     // low_threshold
    float,     // weak
    float,     // strong
    bool       // inverse
);

typedef upscaler_ctx_t* (*NewUpscalerCtxFunction)(const char*,    // esrgan path
                                                  int,            // threads
                                                  enum sd_type_t  // wtype
);
typedef void (*FreeUpscalerCtxFunction)(upscaler_ctx_t*);
// Function pointers
typedef sd_ctx_t* (*NewSdCtxFunction)(
    const char*,      // model_path,
    const char*,      // clip_l_path,
    const char*,      // clip_g_path,
    const char*,      // t5xxl_path,
    const char*,      // diffusion_model_path,
    const char*,      // vae_path,
    const char*,      // taesd_path,
    const char*,      // control_net_path_c_str,
    const char*,      // lora_model_dir,
    const char*,      // embed_dir_c_str,
    const char*,      // stacked_id_embed_dir_c_str,
    bool,             // vae_decode_only,
    bool,             // vae_tiling,
    bool,             // free_params_immediately,
    int,              // n_threads,
    enum sd_type_t,   // wtype,
    enum rng_type_t,  // rng_type,
    enum schedule_t,  // s,
    bool,             // keep_clip_on_cpu,
    bool,             // keep_control_net_cpu,
    bool,             // keep_vae_on_cpu
    bool              // bool diffusion_flash_attn
);
typedef void (*FreeSdCtxFunction)(sd_ctx_t*);
typedef void (*SdSetLogCallbackFunction)(void (*)(enum sd_log_level_t, const char*, void*), void*);
typedef void (*SdSetProgressCallbackFunction)(void (*)(int, int, float, void*), void*);
typedef int32_t (*GetNumPhysicalCoresFunction)();
typedef const char* (*SdGetSystemInfoFunction)();

typedef sd_image_t* (*Txt2ImgFunction)(sd_ctx_t*,             // pointer
                                       const char*,           // prompt,
                                       const char*,           // negative_prompt,
                                       int,                   // clip_skip,
                                       float,                 // cfg_scale,
                                       float,                 // guidance,
                                       int,                   // width,
                                       int,                   // height,
                                       enum sample_method_t,  // sample_method,
                                       int,                   // sample_steps,
                                       int64_t,               // seed,
                                       int,                   // batch_count,
                                       const sd_image_t*,     // control_cond,
                                       float,                 // control_strength,
                                       float,                 // style_strength,
                                       bool,                  // normalize_input,
                                       const char*,           // input_id_images_path
                                       std::vector<int>,      // skip_layers,
                                       float,                 // slg_scale,
                                       float,                 // skip_layer_start,
                                       float                  // skip_layer_end

);

typedef sd_image_t* (*Img2ImgFunction)(sd_ctx_t*,             // pointer
                                       sd_image_t,            // init_image
                                       const char*,           // prompt_c_str,
                                       const char*,           // negative_prompt_c_str,
                                       int,                   // clip_skip,
                                       float,                 // cfg_scale,
                                       float,                 // guidance,
                                       int,                   // width,
                                       int,                   // height,
                                       enum sample_method_t,  // sample_method,
                                       int,                   // sample_steps,
                                       float,                 // strength,
                                       int64_t,               // seed,
                                       int,                   // batch_count,
                                       const sd_image_t*,     // control_cond,
                                       float,                 // control_strength,
                                       float,                 // style_ratio,
                                       bool,                  // normalize_input,
                                       const char*,           // input_id_images_path_c_str
                                       std::vector<int>,      // skip_layers,
                                       float,                 // slg_scale,
                                       float,                 // skip_layer_start,
                                       float                  // skip_layer_end)

);

typedef sd_image_t (*UpscalerFunction)(upscaler_ctx_t*,  // pointer
                                       sd_image_t,       // input image
                                       uint32_t          // upscale factor
);

inline const char* modes_str[] = {"txt2img", "img2img", "convert", "upscale"};

enum SDMode { TXT2IMG,
              IMG2IMG,
              CONVERT,
              UPSCALE,
              MODE_COUNT };

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
    std::vector<int> skip_layers  = {7, 8, 9};
    float skip_layer_start        = 0.01;
    float skip_layer_end          = 0.2;
    float slg_scale               = 0.f;
    bool diffusion_flash_attn     = false;

    SDParams() = default;

    SDParams(const SDParams& other) = default;

    SDParams& operator=(const SDParams& other) = default;
};

/* JSONize SD Params*/
inline void to_json(nlohmann ::json& nlohmann_json_j, const SDParams& nlohmann_json_t) {
    nlohmann_json_j["n_threads"]                  = nlohmann_json_t.n_threads;
    nlohmann_json_j["mode"]                       = nlohmann_json_t.mode;
    nlohmann_json_j["model_path"]                 = nlohmann_json_t.model_path;
    nlohmann_json_j["clip_l_path"]                = nlohmann_json_t.clip_l_path;
    nlohmann_json_j["clip_g_path"]                = nlohmann_json_t.clip_g_path;
    nlohmann_json_j["t5xxl_path"]                 = nlohmann_json_t.t5xxl_path;
    nlohmann_json_j["diffusion_model_path"]       = nlohmann_json_t.diffusion_model_path;
    nlohmann_json_j["vae_path"]                   = nlohmann_json_t.vae_path;
    nlohmann_json_j["taesd_path"]                 = nlohmann_json_t.taesd_path;
    nlohmann_json_j["esrgan_path"]                = nlohmann_json_t.esrgan_path;
    nlohmann_json_j["controlnet_path"]            = nlohmann_json_t.controlnet_path;
    nlohmann_json_j["embeddings_path"]            = nlohmann_json_t.embeddings_path;
    nlohmann_json_j["stacked_id_embeddings_path"] = nlohmann_json_t.stacked_id_embeddings_path;
    nlohmann_json_j["input_id_images_path"]       = nlohmann_json_t.input_id_images_path;
    nlohmann_json_j["wtype"]                      = nlohmann_json_t.wtype;
    nlohmann_json_j["lora_model_dir"]             = nlohmann_json_t.lora_model_dir;
    nlohmann_json_j["output_path"]                = nlohmann_json_t.output_path;
    nlohmann_json_j["input_path"]                 = nlohmann_json_t.input_path;
    nlohmann_json_j["control_image_path"]         = nlohmann_json_t.control_image_path;
    nlohmann_json_j["prompt"]                     = nlohmann_json_t.prompt;
    nlohmann_json_j["negative_prompt"]            = nlohmann_json_t.negative_prompt;
    nlohmann_json_j["min_cfg"]                    = nlohmann_json_t.min_cfg;
    nlohmann_json_j["cfg_scale"]                  = nlohmann_json_t.cfg_scale;
    nlohmann_json_j["guidance"]                   = nlohmann_json_t.guidance;
    nlohmann_json_j["style_ratio"]                = nlohmann_json_t.style_ratio;
    nlohmann_json_j["clip_skip"]                  = nlohmann_json_t.clip_skip;
    nlohmann_json_j["width"]                      = nlohmann_json_t.width;
    nlohmann_json_j["height"]                     = nlohmann_json_t.height;
    nlohmann_json_j["batch_count"]                = nlohmann_json_t.batch_count;
    nlohmann_json_j["video_frames"]               = nlohmann_json_t.video_frames;
    nlohmann_json_j["motion_bucket_id"]           = nlohmann_json_t.motion_bucket_id;
    nlohmann_json_j["fps"]                        = nlohmann_json_t.fps;
    nlohmann_json_j["augmentation_level"]         = nlohmann_json_t.augmentation_level;
    nlohmann_json_j["sample_method"]              = nlohmann_json_t.sample_method;
    nlohmann_json_j["schedule"]                   = nlohmann_json_t.schedule;
    nlohmann_json_j["sample_steps"]               = nlohmann_json_t.sample_steps;
    nlohmann_json_j["strength"]                   = nlohmann_json_t.strength;
    nlohmann_json_j["control_strength"]           = nlohmann_json_t.control_strength;
    nlohmann_json_j["rng_type"]                   = nlohmann_json_t.rng_type;
    nlohmann_json_j["seed"]                       = nlohmann_json_t.seed;
    nlohmann_json_j["verbose"]                    = nlohmann_json_t.verbose;
    nlohmann_json_j["vae_tiling"]                 = nlohmann_json_t.vae_tiling;
    nlohmann_json_j["control_net_cpu"]            = nlohmann_json_t.control_net_cpu;
    nlohmann_json_j["normalize_input"]            = nlohmann_json_t.normalize_input;
    nlohmann_json_j["clip_on_cpu"]                = nlohmann_json_t.clip_on_cpu;
    nlohmann_json_j["vae_on_cpu"]                 = nlohmann_json_t.vae_on_cpu;
    nlohmann_json_j["canny_preprocess"]           = nlohmann_json_t.canny_preprocess;
    nlohmann_json_j["color"]                      = nlohmann_json_t.color;
    nlohmann_json_j["skip_layers"]                = nlohmann_json_t.skip_layers;
    nlohmann_json_j["slg_scale"]                  = nlohmann_json_t.slg_scale;
    nlohmann_json_j["skip_layer_start"]           = nlohmann_json_t.skip_layer_start;
    nlohmann_json_j["skip_layer_end"]             = nlohmann_json_t.skip_layer_end;
}
inline void from_json(const nlohmann ::json& nlohmann_json_j, SDParams& nlohmann_json_t) {
    {
        auto iter = nlohmann_json_j.find("n_threads");
        if (iter != nlohmann_json_j.end())
            if (!iter->is_null())
                iter->get_to(nlohmann_json_t.n_threads);
    }
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
        auto iter = nlohmann_json_j.find("clip_g_path");
        if (iter != nlohmann_json_j.end())
            if (!iter->is_null())
                iter->get_to(nlohmann_json_t.clip_g_path);
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
        auto iter = nlohmann_json_j.find("skip_layers");
        if (iter != nlohmann_json_j.end())
            if (!iter->is_null())
                iter->get_to(nlohmann_json_t.skip_layers);
    }
    {
        auto iter = nlohmann_json_j.find("slg_scale");
        if (iter != nlohmann_json_j.end())
            if (!iter->is_null())
                iter->get_to(nlohmann_json_t.slg_scale);
    }
    {
        auto iter = nlohmann_json_j.find("skip_layer_start");
        if (iter != nlohmann_json_j.end())
            if (!iter->is_null())
                iter->get_to(nlohmann_json_t.skip_layer_start);
    }
    {
        auto iter = nlohmann_json_j.find("skip_layer_end");
        if (iter != nlohmann_json_j.end())
            if (!iter->is_null())
                iter->get_to(nlohmann_json_t.skip_layer_end);
    }
    {
        auto iter = nlohmann_json_j.find("diffusion_flash_attn");
        if (iter != nlohmann_json_j.end())
            if (!iter->is_null())
                iter->get_to(nlohmann_json_t.diffusion_flash_attn);
    }
}

inline static std::vector<int> pointerToVector(int* ptr, size_t size) {
    if (ptr == nullptr) {
        return {};
    }
    return std::vector<int>(ptr, ptr + size);
}
inline static int* vectorToPointer(const std::vector<int>& vec) {
    if (vec.empty()) {
        return nullptr;
    }

    int* ptr = new int[vec.size()];
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = vec[i];
    }

    return ptr;
}

#endif  // STABLE_DIFFUSION_WRAPPER_H