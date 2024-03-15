#!/bin/bash

GIT_HASH=$(git rev-parse --short HEAD)
if [ -f .env ]; then
    if ! grep -q "GIT_HASH=" .env; then
        echo "GIT_HASH=$GIT_HASH" >> .env
    else
        grep -v "GIT_HASH=" .env > .env.tmp
        echo "GIT_HASH=$GIT_HASH" >> .env.tmp
        mv .env.tmp .env
    fi
else
    echo "GIT_HASH=$GIT_HASH" > .env
fi