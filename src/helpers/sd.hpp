#ifndef STABLE_DIFFUSION_WRAPPER_H
#define STABLE_DIFFUSION_WRAPPER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

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
    bool              // keep_vae_on_cpu

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
                                       const char*            // input_id_images_path

);
typedef sd_image_t* (*Img2ImgFunction)(sd_ctx_t*,  // pointer
                                       sd_image_t,
                                       const char*,           // prompt,
                                       const char*,           // negative_prompt,
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
                                       float,                 // style_strength,
                                       bool,                  // normalize_input,
                                       const char*            // input_id_images_path

);
typedef sd_image_t (*UpscalerFunction)(upscaler_ctx_t*,  // pointer
                                       sd_image_t,       // inpu image
                                       uint32_t          // upscale factor
);
typedef upscaler_ctx_t* (*NewUpscalerCtxFunction)(const char*,    // esrgan path
                                                  int,            // threads
                                                  enum sd_type_t  // wtype
);
typedef void (*FreeUpscalerFunction)(upscaler_ctx_t*);
// Add more function pointers for other functions...

// Function declarations
void sd_set_log_callback(void (*sd_log_cb)(enum sd_log_level_t, const char*, void*),
                         void* data);
void sd_set_progress_callback(void (*cb)(int, int, float, void*), void* data);
int32_t get_num_physical_cores();
const char* sd_get_system_info();
sd_ctx_t* new_sd_ctx(

    const char* model_path,
    const char* clip_l_path,
    const char* t5xxl_path,
    const char* diffusion_model_path,
    const char* vae_path,
    const char* taesd_path,
    const char* control_net_path_c_str,
    const char* lora_model_dir,
    const char* embed_dir_c_str,
    const char* stacked_id_embed_dir_c_str,
    bool vae_decode_only,
    bool vae_tiling,
    bool free_params_immediately,
    int n_threads,
    enum sd_type_t wtype,
    enum rng_type_t rng_type,
    enum schedule_t s,
    bool keep_clip_on_cpu,
    bool keep_control_net_cpu,
    bool keep_vae_on_cpu

);
void free_sd_ctx(sd_ctx_t* sd_ctx);
void free_upscaler_ctx(upscaler_ctx_t* upscaler_ctx_t);
sd_image_t* txt2img(sd_ctx_t* sd_ctx, sd_image_t init_image, const char* prompt, const char* negative_prompt, int clip_skip, float cfg_scale, float guidance, int width, int height, enum sample_method_t sample_method, int sample_steps, float strength, int64_t seed, int batch_count, const sd_image_t* control_cond, float control_strength, float style_strength, bool normalize_input, const char* input_id_images_path);
// Add more function declarations for other functions...

#endif  // STABLE_DIFFUSION_WRAPPER_H