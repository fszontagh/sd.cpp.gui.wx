#!/bin/bash
set -x
# $1  - builddir
# $2  - dist dir
# $3  - wxWidgets libdir

VARIANT_DIR="ext.stable-diffusion.cpp_";

if [ -e "$1/${VARIANT_DIR}avx/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}avx/bin/libstable-diffusion.so" "$2/all/stable-diffusion_avx.so"
fi;


if [ -e "$1/${VARIANT_DIR}avx2/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}avx2/bin/libstable-diffusion.so" "$2/all/stable-diffusion_avx2.so"
fi;

if [ -e "$1/${VARIANT_DIR}avx512/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}avx512/bin/libstable-diffusion.so" "$2/all/stable-diffusion_avx512.so"
fi;

if [ -e "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" "$2/all/stable-diffusion_cuda.so"
fi;

if [ -e "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}cuda12/bin/libstable-diffusion.so" "$2/all/stable-diffusion_cuda.so"
fi;

if [ -e "$1/${VARIANT_DIR}rocm/bin/libstable-diffusion.so" ]; then
    cp -p "$1/${VARIANT_DIR}rocm/bin/libstable-diffusion.so" "$2/all/stable-diffusion_rocm.so"
fi;

# cp -p "$3/libwx*" "$2/all/"