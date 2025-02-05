#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 config.json"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Config file $1 does not exist"
    exit 1
fi


# 📌 List of modells (format: "filename URL target folder")
MODEL_LIST=$(cat <<EOF
control_v11p_sd15_openpose.pth https://huggingface.co/lllyasviel/ControlNet-v1-1/resolve/main/control_v11p_sd15_openpose.pth controlnet
sd_15_base.safetensors https://huggingface.co/fp16-guy/Stable-Diffusion-v1-5_fp16_cleaned/resolve/main/sd_1.5.safetensors checkpoint
perfectWorld_v6Baked_inp_fp16.safetensors https://huggingface.co/fp16-guy/Perfect_World_fp16_cleaned/resolve/main/perfectWorld_v6Baked_inp_fp16.safetensors checkpoint
picX_real.safetensors https://huggingface.co/fp16-guy/PicX_real/resolve/main/picX_real.safetensors checkpoint
sd_15_inpainting.safetensors https://huggingface.co/fp16-guy/Stable-Diffusion-v1-5_fp16_cleaned/resolve/main/sd_1.5_inpaint.safetensors checkpoint
vae-ft-mse-840000-ema-pruned_fp16.safetensors https://huggingface.co/fp16-guy/anything_kl-f8-anime2_vae-ft-mse-840000-ema-pruned_blessed_clearvae_fp16_cleaned/resolve/main/vae-ft-mse-840000-ema-pruned_fp16.safetensors vae
tinyauto.safetensors https://huggingface.co/madebyollin/taesd/resolve/main/diffusion_pytorch_model.safetensors taesd
RealESRGAN_x4plus_anime_6B.pth https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.2.4/RealESRGAN_x4plus_anime_6B.pth esrgan
sdxl_vae.safetensors https://huggingface.co/madebyollin/sdxl-vae-fp16-fix/blob/main/sdxl_vae.safetensors vae
EOF
)

# 🔍 JSON-ből mappák beolvasása
CHECKPOINTS_PATH=$(jq -r '.model_paths.checkpoints' "$1")
CONTROLNET_PATH=$(jq -r '.model_paths.controlnet' "$1")
ESRGAN_PATH=$(jq -r '.model_paths.esrgan' "$1")
LORA_PATH=$(jq -r '.model_paths.lora' "$1")
TAESD_PATH=$(jq -r '.model_paths.taesd' "$1")
VAES_PATH=$(jq -r '.model_paths.vae' "$1")

declare -A PATHS=(
    ["checkpoint"]="$CHECKPOINTS_PATH"
    ["controlnet"]="$CONTROLNET_PATH"
    ["esrgan"]="$ESRGAN_PATH"
    ["lora"]="$LORA_PATH"
    ["taesd"]="$TAESD_PATH"
    ["vae"]="$VAES_PATH"
)


while read -r filename url folder; do
    dest_path="${PATHS[$folder]}/$filename"

    if [ -z "$dest_path" ]; then
        echo "ERROR: Unknown target directory: $folder"
        continue
    fi


    echo -en "📥 Download ${folder} model: \n ${dest_path}"

    mkdir -p "$(dirname "$dest_path")"
    wget -q -c -O "$dest_path" "$url"

    if [[ $? -eq 0 ]]; then
        echo -en "\r✅ Succesfully downloaded: $dest_path\t\n"
    else
        echo "❌ Failed: $filename -> $dest_path"
    fi

done <<< "$MODEL_LIST"
