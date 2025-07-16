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
#include "sd_enums.hpp"

// Enumerations and structs are now in sd_enums.hpp
typedef void (*sd_log_cb_t)(enum sd_log_level_t level, const char* text, void* data);
typedef void (*sd_progress_cb_t)(int step, int steps, float time, void* data);

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channel;
    uint8_t* data;
} sd_image_t;

typedef struct {
    const char* model_path;
    const char* clip_l_path;
    const char* clip_g_path;
    const char* t5xxl_path;
    const char* diffusion_model_path;
    const char* vae_path;
    const char* taesd_path;
    const char* control_net_path;
    const char* lora_model_dir;
    const char* embedding_dir;
    const char* stacked_id_embed_dir;
    bool vae_decode_only;
    bool vae_tiling;
    bool free_params_immediately;
    int n_threads;
    enum sd_type_t wtype;
    enum rng_type_t rng_type;
    enum schedule_t schedule;
    bool keep_clip_on_cpu;
    bool keep_control_net_on_cpu;
    bool keep_vae_on_cpu;
    bool diffusion_flash_attn;
    bool chroma_use_dit_mask;
    bool chroma_use_t5_mask;
    int chroma_t5_mask_pad;
} sd_ctx_params_t;

typedef struct {
    int* layers;
    size_t layer_count;
    float layer_start;
    float layer_end;
    float scale;
} sd_slg_params_t;

typedef struct {
    float txt_cfg;
    float img_cfg;
    float min_cfg;
    float distilled_guidance;
    sd_slg_params_t slg;
} sd_guidance_params_t;

typedef struct {
    const char* prompt;
    const char* negative_prompt;
    int clip_skip;
    sd_guidance_params_t guidance;
    sd_image_t init_image;
    sd_image_t* ref_images;
    int ref_images_count;
    sd_image_t mask_image;
    int width;
    int height;
    enum sample_method_t sample_method;
    int sample_steps;
    float eta;
    float strength;
    int64_t seed;
    int batch_count;
    const sd_image_t* control_cond;
    float control_strength;
    float style_strength;
    bool normalize_input;
    const char* input_id_images_path;
} sd_img_gen_params_t;

typedef struct {
    sd_image_t init_image;
    int width;
    int height;
    sd_guidance_params_t guidance;
    enum sample_method_t sample_method;
    int sample_steps;
    float strength;
    int64_t seed;
    int video_frames;
    int motion_bucket_id;
    int fps;
    float augmentation_level;
} sd_vid_gen_params_t;

typedef struct sd_ctx_t sd_ctx_t;
typedef struct upscaler_ctx_t upscaler_ctx_t;

// Function pointers for new API
typedef void (*SdSetLogCallbackFunction)(void (*)(enum sd_log_level_t, const char*, void*), void*);
typedef void (*SdSetProgressCallbackFunction)(void (*)(int, int, float, void*), void*);
typedef int32_t (*GetNumPhysicalCoresFunction)();
typedef const char* (*SdGetSystemInfoFunction)();

typedef void (*SdCtxParamsInitFunction)(sd_ctx_params_t*);
typedef char* (*SdCtxParamsToStrFunction)(const sd_ctx_params_t*);
typedef sd_ctx_t* (*NewSdCtxFunction)(const sd_ctx_params_t*);
typedef void (*FreeSdCtxFunction)(sd_ctx_t*);

typedef void (*SdImgGenParamsInitFunction)(sd_img_gen_params_t*);
typedef char* (*SdImgGenParamsToStrFunction)(const sd_img_gen_params_t*);
typedef sd_image_t* (*GenerateImageFunction)(sd_ctx_t*, const sd_img_gen_params_t*);

typedef void (*SdVidGenParamsInitFunction)(sd_vid_gen_params_t*);
typedef sd_image_t* (*GenerateVideoFunction)(sd_ctx_t*, const sd_vid_gen_params_t*);

typedef upscaler_ctx_t* (*NewUpscalerCtxFunction)(const char*, int);
typedef void (*FreeUpscalerCtxFunction)(upscaler_ctx_t*);
typedef sd_image_t (*UpscalerFunction)(upscaler_ctx_t*, sd_image_t, uint32_t);

typedef bool (*ConvertFunction)(const char*, const char*, const char*, enum sd_type_t, const char*);
typedef uint8_t* (*PreprocessCannyFunction)(uint8_t*, int, int, float, float, float, float, bool);



#endif  // STABLE_DIFFUSION_WRAPPER_H