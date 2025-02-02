#!/bin/bash
set -e

INIT_FLAG="/app/.initialized"
if [ ! -f "$INIT_FLAG" ]; then
    echo "First run, downloading base models..."
    /app/download_base_models.sh /app/config.json
    touch "$INIT_FLAG"
else
    echo "Skipping base model download"
fi
exec /app/@SERVER_BINARY_NAME@ /app/config.json
