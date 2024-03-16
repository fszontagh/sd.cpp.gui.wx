#!/bin/bash

# $1  - builddir
# $2  - dist dir

VARIANT_DIR="ext.stable-diffusion.cpp_";

if [ -e "$1/${VARIANT_DIR}avx/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}avx/bin/libstable-diffusion.so" "$2/stable-diffusion_avx.so"
fi;


if [ -e "$1/${VARIANT_DIR}avx2/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}avx2/bin/libstable-diffusion.so" "$2/stable-diffusion_avx2.so"
fi;

if [ -e "$1/${VARIANT_DIR}avx512/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}avx512/bin/libstable-diffusion.so" "$2/stable-diffusion_avx512.so"
fi;

if [ -e "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" "$2/stable-diffusion_cuda.so"
fi;

if [ -e "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" "$2/stable-diffusion_cuda.so"
fi;

if [ -e "$1/${VARIANT_DIR}rocm/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}rocm/bin/libstable-diffusion.so" "$2/stable-diffusion_rocm.so"
fi;