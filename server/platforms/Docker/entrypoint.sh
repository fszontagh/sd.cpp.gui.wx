#!/bin/bash
set -e

INIT_FLAG="/app/.initialized"

mkdir -p /app /app/log /app/data /app/models /app/models/checkpoints /app/models/loras /app/models/vae /app/models/embeddings /app/models/taesd /app/models/controlnet /app/models/esrgan

if [ "$DOWNLOAD_BASE_MODELS" = "True" ]; then
    if [ ! -f "$INIT_FLAG" ]; then
        echo "First run, downloading base models..."
        /app/download_base_models.sh /app/config.json
        touch "$INIT_FLAG"
    else
        echo "Skipping base model download (already initialized)"
    fi
else
    echo "Skipping base model download (DOWNLOAD_BASE_MODELS=False)"
fi

exec /app/@SERVER_BINARY_NAME@ /app/config.json