set(EXTERNAL_DOWNLOAD_LOCATION ${CMAKE_SOURCE_DIR}/external)

include(ExternalProject)

ExternalProject_Add(stable-diffusion-git
    GIT_REPOSITORY https://github.com/leejet/stable-diffusion.cpp.git
    PREFIX ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion
    DOWNLOAD_DIR ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion
    BINARY_DIR ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion
    CMAKE_ARGS -DCMAKE_CXX_STANDARD=14 -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}/stable-diffusion -DSD_CUBLAS=${SD_CUBLAS} -DSD_HIPBLAS=${SD_HIPBLAS} -DSD_METAL=${SD_METAL} -DSD_BUILD_EXAMPLES=OFF -DSD_FLASH_ATTN=${SD_FLASH_ATTN} -DBUILD_SHARED_LIBS=ON -DSD_BUILD_SHARED_LIBS=ON -DGGML_AVX=${GGML_AVX} -DGGML_AVX2=${GGML_AVX2} -DGGML_AVX512=${GGML_AVX512} -DGGML_AVX512_VBMI=${GGML_AVX512_VBMI} -DGGML_AVX512_VNNI=${GGML_AVX512_VNNI} -DGGML_FMA=${GGML_FMA} -DGGML_CUBLAS=${SD_CUBLAS}
    PATCH_COMMAND git apply "${EXTERNAL_DOWNLOAD_LOCATION}/patches/lora.patch"
)

add_library(ggml SHARED IMPORTED)
add_library(stable-diffusion SHARED IMPORTED)

if(WIN32)

set(GGML_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/ggml/src/${CMAKE_BUILD_TYPE}/ggml.lib)
set(GGML_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/ggml.dll)

set(SD_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/${CMAKE_BUILD_TYPE}/stable-diffusion.lib)
set(SD_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/${CMAKE_BUILD_TYPE}/stable-diffusion.dll)

set_target_properties(stable-diffusion PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/Debug/stable-diffusion.dll
IMPORTED_IMPLIB_DEBUG          ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/Debug/stable-diffusion.lib
IMPORTED_LOCATION_RELEASE      ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/Release/stable-diffusion.dll
IMPORTED_IMPLIB_RELEASE        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/Release/stable-diffusion.lib
INTERFACE_INCLUDE_DIRECTORIES  ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/src/stable-diffusion-git
)

set_target_properties(ggml PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/Debug/stable-diffusion.dll
IMPORTED_IMPLIB_DEBUG          ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/ggml/src/Debug/ggml.lib
IMPORTED_LOCATION_RELEASE      ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/Release/stable-diffusion.dll
IMPORTED_IMPLIB_RELEASE        ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/ggml/src/Release/ggml.lib
)

else()



set(GGML_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/ggml/src/ggml.a)
set(GGML_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/libggml.so)

set(SD_LIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/stable-diffusion.a)
set(SD_SLIB ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/bin/libstable-diffusion.so)

set_target_properties(stable-diffusion PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${SD_SLIB}
IMPORTED_IMPLIB_DEBUG          ${SD_LIB}
IMPORTED_LOCATION_RELEASE      ${SD_SLIB}
IMPORTED_IMPLIB_RELEASE        ${SD_LIB}
INTERFACE_INCLUDE_DIRECTORIES  ${EXTERNAL_DOWNLOAD_LOCATION}/stable-diffusion/src/stable-diffusion-git
)

set_target_properties(ggml PROPERTIES
IMPORTED_CONFIGURATIONS        "DEBUG;RELEASE"
IMPORTED_LOCATION_DEBUG        ${GGML_SLIB}
IMPORTED_IMPLIB_DEBUG          ${GGML_LIB}
IMPORTED_LOCATION_RELEASE      ${GGML_SLIB}
IMPORTED_IMPLIB_RELEASE        ${GGML_LIB}
)


endif()